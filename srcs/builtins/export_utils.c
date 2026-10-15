/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: petitcoeur <petitcoeur@student.42.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 02:48:46 by petitcoeur        #+#    #+#             */
/*   Updated: 2024/12/25 06:24:35 by petitcoeur       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	bubble_sort(char **envp, int size)
{
	char	*temp;
	int		i;
	int		j;

	i = -1;
	while (++i < size - 1)
	{
		j = -1;
		while (++j < size - 1 - i)
		{
			if (ft_strncmp(envp[j], envp[j + 1], ft_strlen(envp[j]) + 1) > 0)
			{
				temp = envp[j];
				envp[j] = envp[j + 1];
				envp[j + 1] = temp;
			}
		}
	}
}

static void	display_export_vars(char **sorted_envp, t_minishell *data)
{
	char	*equal_sign;
	char	*key;
	char	*value;
	int		i;

	i = -1;
	while (sorted_envp[++i])
	{
		ft_putstr_fd("export ", STDOUT_FILENO);
		equal_sign = ft_strchr(sorted_envp[i], '=');
		if (!equal_sign)
			ft_putstr_fd(sorted_envp[i], STDOUT_FILENO);
		else
		{
			key = ft_substr_gc
				(sorted_envp[i], 0, equal_sign - sorted_envp[i], &data->gc);
			value = get_env_value(key, data);
			ft_putstr_fd(key, STDOUT_FILENO);
			ft_putstr_fd("=\"", STDOUT_FILENO);
			ft_putstr_fd(value, STDOUT_FILENO);
			ft_putchar_fd('"', STDOUT_FILENO);
		}
		ft_putstr_fd("\n", STDOUT_FILENO);
	}
}

void	print_export(t_minishell *data)
{
	char	**sorted_envp;
	int		env_count;
	int		i;

	env_count = ft_tabstrlen(data->envp);
	sorted_envp = gc_malloc(sizeof(char *) * (env_count + 1), &data->gc);
	i = -1;
	while (++i < env_count)
		sorted_envp[i] = ft_strdup_gc(data->envp[i], &data->gc);
	sorted_envp[i] = NULL;
	bubble_sort(sorted_envp, env_count);
	display_export_vars(sorted_envp, data);
	i = -1;
	while (sorted_envp[++i])
		gc_free(sorted_envp[i], &data->gc);
	gc_free(sorted_envp, &data->gc);
}

static void	update_env_plus(char *key, char *value_to_add, t_minishell *data)
{
	int		i;

	i = get_env_index(key, data);
	set_gc_node_locked(&data->gc, data->envp[i], false);
	if (get_env_value(key, data))
		data->envp[i] = \
		ft_strjoin_gc(data->envp[i], value_to_add, &data->gc);
	else
	{
		data->envp[i] = ft_strjoin_gc(data->envp[i], "=", &data->gc);
		data->envp[i] = \
		ft_strjoin_gc(data->envp[i], value_to_add, &data->gc);
	}
	set_gc_node_locked(&data->gc, data->envp[i], true);
}

void	process_var_key_return(int var_key_checks_return, \
char *arg, char *equal_sign, t_minishell *data)
{
	char	*value;
	char	*key;

	if (var_key_checks_return == 0)
	{
		key = ft_substr_gc(arg, 0, equal_sign - arg, &data->gc);
		value = ft_strdup_gc(equal_sign + 1, &data->gc);
		if (!value)
			value = ft_strdup_gc("", &data->gc);
		if (get_env_index(key, data) == -1)
			add_env(key, value, data);
		else
			update_env(key, value, data);
	}
	else if (var_key_checks_return == 2)
	{
		key = ft_substr_gc(arg, 0, equal_sign - 1 - arg, &data->gc);
		value = ft_strdup_gc(equal_sign + 1, &data->gc);
		if (!value)
			value = ft_strdup_gc("", &data->gc);
		if (get_env_index(key, data) == -1)
			add_env(key, value, data);
		else
			update_env_plus(key, value, data);
	}
}
