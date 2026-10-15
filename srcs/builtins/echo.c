/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: petitcoeur <petitcoeur@student.42.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 15:56:38 by besch             #+#    #+#             */
/*   Updated: 2024/12/25 06:24:17 by petitcoeur       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_valid_n_flag(const char *arg)
{
	int	i;

	if (!arg || arg[0] != '-' || arg[1] != 'n')
		return (0);
	i = 1;
	while (arg[++i])
		if (arg[i] != 'n')
			return (0);
	return (1);
}

void	ft_echo(t_node *cmd_args, t_minishell *data)
{
	bool	newline;

	(void)data;
	newline = true;
	while (cmd_args && cmd_args->value
		&& is_valid_n_flag(cmd_args->value))
	{
		newline = false;
		cmd_args = cmd_args->next;
	}
	while (cmd_args)
	{
		ft_putstr_fd(cmd_args->value, STDOUT_FILENO);
		if (cmd_args->next)
			ft_putchar_fd(' ', STDOUT_FILENO);
		cmd_args = cmd_args->next;
	}
	if (newline)
		ft_putchar_fd('\n', STDOUT_FILENO);
	data->last_exit_status = 0;
}
