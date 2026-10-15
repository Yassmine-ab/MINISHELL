/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initialization.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaabdall <yaabdall@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/10 03:27:24 by yaabdall          #+#    #+#             */
/*   Updated: 2024/12/28 16:33:33 by yaabdall         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	**envp_is_null(t_minishell *data)
{
	char	**envp;
	char	*path;

	envp = gc_malloc(sizeof(char *) * 5, &data->gc);
	set_gc_node_locked(&data->gc, envp, true);
	path = getcwd(NULL, 0);
	envp[0] = ft_strjoin_gc("PWD=", path, &data->gc);
	set_gc_node_locked(&data->gc, envp[0], true);
	envp[1] = ft_strdup_gc("SHLVL=1", &data->gc);
	set_gc_node_locked(&data->gc, envp[1], true);
	envp[2] = ft_strdup_gc("OLDPWD", &data->gc);
	set_gc_node_locked(&data->gc, envp[2], true);
	envp[3] = ft_strdup_gc("_=/usr/bin/env", &data->gc);
	set_gc_node_locked(&data->gc, envp[3], true);
	envp[4] = NULL;
	return (envp);
}

static void	update_shlvl(t_minishell *data)
{
	int	shlvl_value;

	if (get_env_index("SHLVL", data) == -1)
		add_env("SHLVL", "1", data);
	else
	{
		shlvl_value = ft_atoi(get_env_value("SHLVL", data));
		if (shlvl_value < 0)
			shlvl_value = -1;
		update_env
			("SHLVL", ft_itoa_gc(shlvl_value + 1, &data->gc), data);
	}
}

static void	env_init(char **envp, t_minishell *data)
{
	int	i;

	if (data->gc.head == NULL)
	{
		gc_init(&data->gc);
		if (envp[0] == NULL)
			data->envp = envp_is_null(data);
		else
		{
			data->envp = gc_malloc
				(sizeof(char *) * (ft_tabstrlen(envp) + 1), &data->gc);
			set_gc_node_locked(&data->gc, data->envp, true);
			i = -1;
			while (envp[++i])
			{
				data->envp[i] = ft_strdup_gc(envp[i], &data->gc);
				set_gc_node_locked(&data->gc, data->envp[i], true);
			}
			data->envp[i] = NULL;
			update_shlvl(data);
		}
	}
}

void	data_init(int argc, char **argv, char **envp, t_minishell *data)
{
	env_init(envp, data);
	data->argv = argv;
	data->line = NULL;
	data->result = NULL;
	data->argc = argc;
	data->fd = -1;
	data->heredoc_fd[0] = -1;
	data->heredoc_fd[1] = -1;
	data->saved_stdin = -1;
	data->saved_stdout = -1;
	data->open_parentheses = 0;
	data->is_child_process = false;
	data->child_end_with_signal = false;
	data->in_command = false;
	data->in_single_quotes = false;
	data->in_double_quotes = false;
	data->tokens = gc_malloc(sizeof(t_token) * (MAX_TOKENS + 1), &data->gc);
	data->current_type = COMMAND;
	data->node = NULL;
	data->last_exec_error = EXEC_NO_FILE;
}
