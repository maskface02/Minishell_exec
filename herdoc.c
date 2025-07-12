#include "exec.h"
#define REDIR_IN 1
#define REDIR_OUT 2
#define REDIR_APPEND 3
#define REDIR_HEREDOC 4


void	free_commands(t_command *cmd)
{
	t_redir		*redir;
	t_redir		*next_redir;
	t_command	*next_cmd;

	while (cmd)
	{
		redir = cmd->redirs;
		while (redir)
		{
			next_redir = redir->next;
			if (redir->heredoc_file_name)
				free(redir->heredoc_file_name);
			free(redir);
			redir = next_redir;
		}
		next_cmd = cmd->next;
		free(cmd);
		cmd = next_cmd;
	}
}

void	cleanup_heredocs(t_command *cmd)
{
	t_redir		*redir;

	while (cmd)
	{
		redir = cmd->redirs;
		while (redir)
		{
			if (redir->type == REDIR_HEREDOC && redir->heredoc_file_name)
			{
				unlink(redir->heredoc_file_name);
				free(redir->heredoc_file_name);
				redir->heredoc_file_name = NULL;
			}
			redir = redir->next;
		}
		cmd = cmd->next;
	}
}


void	print_file_content(const char *filename)
{
	int		fd;
	ssize_t	bytes_read;
	ssize_t	written;
	ssize_t	result;
	char	buffer[1025];

	fd = open(filename, O_RDONLY);
	if (fd < 0)
	{
		perror("print_file_content: open");
		return ;
	}
	while (1)
	{
		bytes_read = read(fd, buffer, sizeof(buffer) - 1);
		if (bytes_read < 0)
		{
			perror("read");
			break ;
		}
		if (!bytes_read)
			break ;
		written = 0;
		while (written < bytes_read)
		{
			result = write(2, buffer + written, bytes_read
					- written);
			if (result <= 0)
			{
				perror("write");
				break ;
			}
			written += result;
		}
	}
	close(fd);
}


char	*generate_tmp_filename(void)
{
	int				random_fd;
	char			*filename;
	char			*ptr;
	char			*base;
	char			*chars;
	unsigned char	rand_byte;
	int				i;
	ssize_t			read_result;

	random_fd = open("/dev/random", O_RDONLY);
	if (random_fd < 0)
	{
		perror("generate_tmp_filename: open");
		return (NULL);
	}
	filename = malloc(34);
	if (!filename)
	{
		close(random_fd);
		return (NULL);
	}
	ptr = filename;
	base = "/tmp/minishell_heredoc_";
	while (*base)
		*ptr++ = *base++;
	chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	i = -1;
	while (++i < 10)
	{
		read_result = read(random_fd, &rand_byte, 1);
		if (read_result != 1)
		{
			perror("generate_tmp_filename: read");
			free(filename);
			close(random_fd);
			return (NULL);
		}
		*ptr++ = chars[rand_byte % 62];
	}
	*ptr = '\0';
	close(random_fd);
	return (filename);
}

int	preprocess_heredocs(t_command *cmd) 
{
	char	*line;
	char	*filename;
	int		i;
	int		fd;
	char	*t;
	char	*l;

	while (cmd)
	{
		while (cmd->redirs)
		{
			if (cmd->redirs->type == REDIR_HEREDOC)
			{
				filename = NULL;
				i = 0;
				fd = -1;
				while (i < 10)
				{
					if (filename)
						free(filename);
					filename = generate_tmp_filename();
					if (!filename)
						return (0);
					fd = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0600);
					if (fd >= 0)
						break ;
					if (errno != EEXIST)
					{
						perror("preprocess_heredocs: open");
						free(filename);
						return (0);
					}
					i++;
				}
				if (fd < 0)
					return (free(filename), 0);
				while (1)
				{
					line = readline("> ");
					if (!line)
					{
						write(1, "\n", 1);
						break ;
					}
					t = cmd->redirs->target;
					l = line;
					while (*t && *l && *t == *l)
					{
						t++;
						l++;
					}
					if (*t == '\0' && *l == '\0')
					{
						free(line);
						break ;
					}
					if (ft_strlen(line) > 0)
						write(fd, line, ft_strlen(line));
					write(fd, "\n", 1);
					free(line);
				}
				close(fd);
				cmd->redirs->heredoc_file_name = filename;
			}
			cmd->redirs = cmd->redirs->next;
		}
		cmd = cmd->next;
	}
	return (1);
}




int	main(void)
{
	t_command	*cmd;
	t_redir		*heredoc;

	cmd = malloc(sizeof(t_command));
	if (!cmd)
	{
		perror("malloc");
		return (1);
	}
	cmd->args = NULL;
	cmd->redirs = NULL;
	cmd->next = NULL;
	heredoc = malloc(sizeof(t_redir));
	if (!heredoc)
	{
		perror("malloc");
		free(cmd);
		return (1);
	}
	heredoc->type = REDIR_HEREDOC;
	heredoc->target = "EOF";
	heredoc->heredoc_file_name = NULL;
	heredoc->next = NULL;
	cmd->redirs = heredoc;
	if (preprocess_heredocs(cmd))
	{
		printf("Temporary file name: %s\n", heredoc->heredoc_file_name);
		printf("file content:\n");
		print_file_content(heredoc->heredoc_file_name);
	}
	else
		printf("Heredoc processing failed\n");
	cleanup_heredocs(cmd);
	free_commands(cmd);
	return (0);
}




