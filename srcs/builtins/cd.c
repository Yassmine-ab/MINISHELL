/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: petitcoeur <petitcoeur@student.42.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 15:56:51 by besch             #+#    #+#             */
/*   Updated: 2024/12/26 11:28:21 by petitcoeur       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	update_pwd_vars(char *old_path, char *new_path, t_minishell *data)
{
	int		var_pwd_idx;
	int		var_old_pwd_idx;

	var_pwd_idx = get_env_index("PWD", data);
	if (var_pwd_idx != -1)
	{
		set_gc_node_locked(&data->gc, data->envp[var_pwd_idx], false);
		data->envp[var_pwd_idx] = \
		ft_strjoin_gc("PWD=", new_path, &data->gc);
		set_gc_node_locked(&data->gc, data->envp[var_pwd_idx], true);
	}
	var_old_pwd_idx = get_env_index("OLDPWD", data);
	if (var_old_pwd_idx != -1)
	{
		set_gc_node_locked(&data->gc, data->envp[var_old_pwd_idx], false);
		data->envp[var_old_pwd_idx] = \
			ft_strjoin_gc("OLDPWD=", old_path, &data->gc);
		set_gc_node_locked(&data->gc, data->envp[var_old_pwd_idx], true);
	}
	free(old_path);
}

static void	update_pwd(char *old_path, t_minishell *data)
{
	char	*new_path;

	new_path = getcwd(NULL, 0);
	if (new_path)
	{
		update_pwd_vars(old_path, new_path, data);
		free(new_path);
		data->last_exit_status = 0;
		return ;
	}
	ft_putstr_fd("pwd: can't get current path\n", STDERR_FILENO);
	data->last_exit_status = 1;
}

static bool	\
	is_change_directory_ok(char *old_path, char *new_path, t_minishell *data)
{
	if (access(new_path, F_OK) != 0)
	{
		ft_putstr_fd("cd: no such file or directory: ", STDERR_FILENO);
		ft_putstr_fd(new_path, STDERR_FILENO);
		ft_putchar_fd('\n', STDERR_FILENO);
		if (old_path)
			free(old_path);
		data->last_exit_status = 1;
		return (false);
	}
	if (chdir(new_path) != 0)
	{
		ft_putstr_fd("cd: permission denied: ", STDERR_FILENO);
		if (old_path)
			free(old_path);
		data->last_exit_status = 1;
		return (false);
	}
	return (true);
}

void	ft_cd(t_node *cmd_args, t_minishell *data)
{
	char	*old_path;
	char	*new_path;

	if (!cmd_args)
	{
		new_path = get_env_value("HOME", data);
		if (!new_path)
		{
			ft_putstr_fd("cd: HOME not set\n", STDERR_FILENO);
			data->last_exit_status = 1;
			return ;
		}
	}
	else if (cmd_args->next)
	{
		ft_putstr_fd("cd: too many arguments\n", STDERR_FILENO);
		data->last_exit_status = 1;
		return ;
	}
	else
		new_path = cmd_args->value;
	old_path = getcwd(NULL, 0);
	if (is_change_directory_ok(old_path, new_path, data) == true)
		update_pwd(old_path, data);
}
