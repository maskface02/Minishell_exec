#include "exec.h"

void	gc_add(t_gc_node **gc, void *ptr)
{
	t_gc_node	*node;

	node = malloc(sizeof(t_gc_node));
	if (!node)
	{
		write(2, "minishell: gc_add malloc failed\n", 31);
    gc_clean(gc);
		exit(1);
	}
	node->ptr = ptr;
	node->next = *gc;
	*gc = node;
}

void	gc_remove(t_gc_node **gc, void *ptr)
{
	t_gc_node	*current;
	t_gc_node	*prev;

	current = *gc;
	prev = NULL;
	while (current)
	{
		if (current->ptr == ptr)
		{
			if (prev)
				prev->next = current->next;
			else
				*gc = current->next;
			free(current);
			return ;
		}
		prev = current;
		current = current->next;
	}
}

void	gc_clean(t_gc_node **gc)
{
	t_gc_node	*current;
	t_gc_node	*next;

	current = *gc;
	while (current)
	{
		next = current->next;
		free(current->ptr);
		free(current);
		current = next;
	}
	*gc = NULL;
}

void	*gc_malloc(t_gc_node **gc, size_t size)
{
	void	*ptr;

	ptr = malloc(size);
	if (!ptr)
	{
		write(2, "minishell: malloc failed\n", 25);
		gc_clean(gc);
		exit(1);
	}
	gc_add(gc, ptr);
	return (ptr);
}
