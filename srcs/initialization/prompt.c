/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prompt.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaabdall <yaabdall@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/10 03:27:27 by yaabdall          #+#    #+#             */
/*   Updated: 2024/12/23 10:46:10 by yaabdall         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*create_prompt(t_minishell *data)
{
	char	*prompt;
	char	*user;
	char	cwd[MAX_PATHLENGTH];

	if (getcwd(cwd, sizeof(cwd)) == NULL)
	{
		ft_putendl_fd("Failed to get current working directory (getcwd)", 2);
		cwd[0] = '\0';
	}
	user = get_env_value("USER", data);
	if (user == NULL)
		user = "user";
	user = ft_strjoin_gc(CYAN, user, &data->gc);
	prompt = ft_strjoin_gc("", user, &data->gc);
	prompt = ft_strjoin_gc(prompt, DEFAULT " in ", &data->gc);
	prompt = ft_strjoin_gc(prompt, GREEN, &data->gc);
	prompt = ft_strjoin_gc(prompt, cwd, &data->gc);
	prompt = ft_strjoin_gc(prompt, DEFAULT " ⋙  ", &data->gc);
	return (prompt);
}
