/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besch <besch@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 15:58:18 by besch             #+#    #+#             */
/*   Updated: 2024/12/27 18:04:09 by besch            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static bool	is_number_valid(char *nb)
{
	const long long	max_div_10 = 922337203685477580LL;
	long long		result;
	int				sign;
	int				i;

	result = 0;
	sign = 1;
	i = 0;
	if (!nb || ft_strlen(nb) > 20)
		return (false);
	if (nb[i] == '+' || nb[i] == '-')
		if (nb[i++] == '-')
			sign = -1;
	if (nb[i] == '\0')
		return (false);
	while (nb[i])
	{
		if (!ft_isdigit(nb[i]) || result > max_div_10 || (result == max_div_10
				&& ((sign == 1 && nb[i] > '7') || (sign == -1 && nb[i] > '8'))))
			return (false);
		result = result * 10 + (nb[i++] - '0');
	}
	return (true);
}

static void	check_arg_value(char *arg, t_minishell *data)
{
	if (is_number_valid(arg) == false)
	{
		ft_putstr_fd("exit\n", STDERR_FILENO);
		ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
		ft_putstr_fd(arg, STDERR_FILENO);
		ft_putstr_fd(": numeric argument required\n", STDERR_FILENO);
		gc_cleanup(&data->gc);
		exit(2);
	}
	data->last_exit_status = ft_atoi(arg);
	return ;
}

void	ft_exit(t_node *cmd_args, t_minishell *data)
{
	if (cmd_args)
	{
		if (cmd_args->next)
		{
			ft_putstr_fd("exit\n", STDERR_FILENO);
			ft_putstr_fd
				("minishell: exit: too many arguments\n", STDERR_FILENO);
			data->last_exit_status = 1;
			return ;
		}
		check_arg_value(cmd_args->value, data);
	}
	if (data->line == 0)
		data->last_exit_status = 0;
	(safe_close(&data->saved_stdin), safe_close(&data->saved_stdout));
	(safe_close(&data->fd));
	ft_putstr_fd("exit\n", STDERR_FILENO);
	gc_cleanup(&data->gc);
	exit(data->last_exit_status);
}
