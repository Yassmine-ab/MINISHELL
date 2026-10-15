/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besch <besch@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 15:57:42 by besch             #+#    #+#             */
/*   Updated: 2024/12/26 20:02:41 by besch            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	print_unset_error(char *key, t_minishell *data)
{
	ft_putstr_fd("unset: ", STDERR_FILENO);
	ft_putstr_fd(key, STDERR_FILENO);
	ft_putstr_fd(": not found\n", STDERR_FILENO);
	data->last_exit_status = 1;
}

static char	**remove_env_variable(char **envp, int index, t_minishell *data)
{
	char	**new_envp;
	int		i;
	int		j;

	i = -1;
	j = -1;
	while (data->envp[++i])
		set_gc_node_locked(&data->gc, data->envp[i], false);
	set_gc_node_locked(&data->gc, data->envp, false);
	new_envp = gc_malloc(sizeof(char *) * ft_tabstrlen(envp), &data->gc);
	set_gc_node_locked(&data->gc, new_envp, true);
	i = -1;
	while (envp[++j])
	{
		set_gc_node_locked(&data->gc, envp[j], false);
		if (j != index)
		{
			new_envp[++i] = ft_strdup_gc(envp[j], &data->gc);
			set_gc_node_locked(&data->gc, new_envp[i], true);
		}
	}
	new_envp[i] = NULL;
	return (new_envp);
}

void	ft_unset(t_node *cmd_args, t_minishell *data)
{
	int		index;
	char	**new_envp;

	while (cmd_args)
	{
		index = get_env_index(cmd_args->value, data);
		if (index == -1)
			print_unset_error(cmd_args->value, data);
		else
		{
			new_envp = remove_env_variable(data->envp, index, data);
			free_envp(data);
			data->envp = new_envp;
		}
		cmd_args = cmd_args->next;
	}
}
