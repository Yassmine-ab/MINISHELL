/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaabdall <yaabdall@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/22 10:49:41 by yaabdall          #+#    #+#             */
/*   Updated: 2024/12/25 12:31:04 by yaabdall         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	concatenate_adjacent_nodes(t_node *node, t_minishell *data)
{
	t_node	*current;
	t_node	*temp;
	char	*combined;

	current = node;
	while (current && current->next)
	{
		if (current->space_after == false)
		{
			combined = ft_strjoin_gc(current->value, \
			current->next->value, &data->gc);
			gc_free(current->value, &data->gc);
			current->value = combined;
			temp = current->next;
			current->next = temp->next;
			gc_free(temp->value, &data->gc);
			gc_free(temp, &data->gc);
		}
		else
			current = current->next;
	}
}

char	**get_command_args(t_node *cmd_node, t_minishell *data)
{
	int		arg_count;
	t_node	*arg;
	char	**args;
	int		i;

	arg_count = 1;
	arg = cmd_node->args;
	while (arg)
	{
		if (arg->is_single_quoted == false)
			arg->value = expand_variables(arg->value, data);
		arg_count++;
		arg = arg->next;
	}
	args = gc_malloc(sizeof(char *) * (arg_count + 1), &data->gc);
	args[0] = cmd_node->value;
	arg = cmd_node->args;
	i = 1;
	while (arg)
	{
		args[i++] = arg->value;
		arg = arg->next;
	}
	args[i] = NULL;
	return (args);
}

static bool	is_executable(const char *path, t_minishell *data)
{
	struct stat	st;

	if (stat(path, &st) == -1)
		return (data->last_exec_error = EXEC_NO_FILE, false);
	if (!S_ISREG(st.st_mode))
		return (data->last_exec_error = EXEC_NOT_REGULAR, false);
	if (access(path, X_OK) == -1)
		return (data->last_exec_error = EXEC_NO_PERMISSION, false);
	return (data->last_exec_error = EXEC_SUCCESS, true);
}

char	*get_command_path(char *command, t_minishell *data)
{
	char		**paths;
	char		*path_env;
	char		*full_path;
	int			initial_error;
	int			i;

	if (is_executable(command, data) == true)
		return (command);
	initial_error = data->last_exec_error;
	path_env = get_env_value("PATH", data);
	if (path_env == NULL)
		return (NULL);
	paths = ft_split_gc(path_env, ':', &data->gc);
	i = -1;
	while (paths[++i])
	{
		full_path = ft_strjoin_gc(paths[i], "/", &data->gc);
		full_path = ft_strjoin_gc(full_path, command, &data->gc);
		if (is_executable(full_path, data) == true)
			return (full_path);
	}
	data->last_exec_error = initial_error;
	return (NULL);
}
