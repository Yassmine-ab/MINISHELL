/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: petitcoeur <petitcoeur@student.42.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 15:57:04 by besch             #+#    #+#             */
/*   Updated: 2024/12/16 03:36:26 by petitcoeur       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	pwd_invalid_option(char *key, t_minishell *data)
{
	ft_putstr_fd("pwd: ", STDERR_FILENO);
	ft_putchar_fd(key[0], STDERR_FILENO);
	ft_putchar_fd(key[1], STDERR_FILENO);
	ft_putstr_fd(": invalid option\n", STDERR_FILENO);
	ft_putstr_fd("usage: pwd [-LP]\n", STDERR_FILENO);
	data->last_exit_status = 2;
}

void	ft_pwd(t_node *cmd_args, t_minishell *data)
{
	char	*path;

	if (cmd_args)
	{
		if ((cmd_args->value[0] == '-' && cmd_args->value[1]
				&& cmd_args->value[1] != '-')
			|| (cmd_args->value[0] == '-'
				&& cmd_args->value[1] == '-' && cmd_args->value[2]))
		{
			pwd_invalid_option(cmd_args->value, data);
			return ;
		}
	}
	path = getcwd(NULL, 0);
	if (path)
	{
		ft_putstr_fd(path, STDOUT_FILENO);
		ft_putchar_fd('\n', STDOUT_FILENO);
		free(path);
		data->last_exit_status = 0;
		return ;
	}
	ft_putstr_fd("pwd: can't get current path\n", STDERR_FILENO);
	data->last_exit_status = 1;
}
