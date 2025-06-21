void sort_env(t_env **exp)
{
  t_env *tmp;
  char *swp;

  tmp = *exp;
  while (*exp)
  {
    if (ft_strcmp((*exp)->value, (*exp)->next->value) > 0)
    {
      swp = (*exp)->next->value;
      (*exp)->next->value = (*exp)->value;
      (*exp)->value = swp;
      *exp = tmp;
    }
    else
      (*exp) = (*exp)->next; 
  }
  *exp = tmp;
}

int	copy_sort(t_env *env, t_env **exp)
{
	t_env	*tmp;
	t_env	*new_node;


	tmp = env;
	while (env)
	{
		new_node = create_node(env->value);
		if (!new_node)
			return (free_env(*exp), 0);
		add_back(exp, new_node);
		env = env->next;
	}
  sort_env(exp);
	return (1);
}
