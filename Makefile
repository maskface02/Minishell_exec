SRC_DIRS = Minishell_exec/builtins Minishell_exec/helper_utils
SRCS = main.c $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
OBJS = $(SRCS:.c=.o)
NAME = minishell_exec
CC = cc -g
CFLAGS = -Wall -Wextra -Werror
LDFLAGS = -lreadline -L/usr/lib
INCLUDES = -I/usr/include/readline

all: $(NAME)

%.o: %.c exec.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(LDFLAGS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
