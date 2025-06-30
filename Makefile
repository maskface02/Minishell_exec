SRC_DIRS = main.c Minishell_exec/builtins Minishell_exec/helper_utils
SRCS = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
OBJS = $(SRCS:.c=.o)
NAME = minishell_exec
CC = cc -g
CFLAGS = -Wall -Wextra -Werror
INCLUDE = -I/usr/include/readline -lreadline 

all: $(NAME)

%.o: %.c exec.h
	${CC} ${CFLAGS} -c $< -o $@

$(NAME): $(OBJS) $(LIBFT)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(INCLUDE)


clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
