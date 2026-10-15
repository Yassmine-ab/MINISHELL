/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   environment.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besch <besch@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/03 14:08:19 by petitcoeur        #+#    #+#             */
/*   Updated: 2024/12/26 18:04:44 by besch            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	update_env(char *key, char *new_value, t_minishell *data)
{
	int		i;

	i = get_env_index(key, data);
	set_gc_node_locked(&data->gc, data->envp[i], false);
	data->envp[i] = ft_strjoin_gc(key, "=", &data->gc);
	data->envp[i] = ft_strjoin_gc(data->envp[i], new_value, &data->gc);
	set_gc_node_locked(&data->gc, data->envp[i], true);
}

static char	*create_new_key_value(char *key, char *value, t_minishell *data)
{
	char	*new_key_value;

	if (value == NULL)
		new_key_value = ft_strdup_gc(key, &data->gc);
	else
	{
		new_key_value = ft_strjoin_gc(key, "=", &data->gc);
		if (value)
			new_key_value = ft_strjoin_gc(new_key_value, value, &data->gc);
	}
	set_gc_node_locked(&data->gc, new_key_value, true);
	return (new_key_value);
}

void	add_env(char *key, char *value, t_minishell *data)
{
	char	**new_envp;
	char	*new_key_value;
	int		i;

	i = -1;
	while (data->envp[++i])
		set_gc_node_locked(&data->gc, data->envp[i], false);
	set_gc_node_locked(&data->gc, data->envp, false);
	i = ft_tabstrlen(data->envp);
	new_envp = gc_malloc(sizeof(char *) * (i + 2), &data->gc);
	set_gc_node_locked(&data->gc, new_envp, true);
	i = -1;
	while (data->envp[++i])
	{
		new_envp[i] = ft_strdup_gc(data->envp[i], &data->gc);
		set_gc_node_locked(&data->gc, new_envp[i], true);
	}
	new_key_value = create_new_key_value(key, value, data);
	new_envp[i] = new_key_value;
	new_envp[i + 1] = NULL;
	free_envp(data);
	data->envp = new_envp;
}
