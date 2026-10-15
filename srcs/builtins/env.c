/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaabdall <yaabdall@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 15:58:01 by besch             #+#    #+#             */
/*   Updated: 2024/12/19 09:23:31 by yaabdall         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_env(t_node *cmd_args, t_minishell *data)
{
	char	*equal_sign;
	int		i;

	if (cmd_args)
	{
		ft_putstr_fd("env: too many arguments\n", STDERR_FILENO);
		data->last_exit_status = 1;
		return ;
	}
	i = -1;
	while (data->envp[++i])
	{
		equal_sign = ft_strchr(data->envp[i], '=');
		if (equal_sign)
		{
			ft_putstr_fd(data->envp[i], STDOUT_FILENO);
			ft_putstr_fd("\n", STDOUT_FILENO);
		}
	}
	data->last_exit_status = 0;
}
