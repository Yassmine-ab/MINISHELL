/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   environment_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besch <besch@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/26 18:01:30 by besch             #+#    #+#             */
/*   Updated: 2024/12/26 18:02:26 by besch            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_envp(t_minishell *data)
{
	int	i;

	i = 0;
	while (data->envp[i])
	{
		gc_free(data->envp[i], &data->gc);
		i++;
	}
	gc_free(data->envp, &data->gc);
}

char	*get_env_value(char *env_key, t_minishell *data)
{
	int		i;
	char	*env_var;
	char	*equal_sign;

	i = 0;
	while (data->envp[i])
	{
		env_var = data->envp[i];
		if (ft_strncmp(env_var, env_key, ft_strlen(env_key)) == 0
			&& (env_var[ft_strlen(env_key)] == '='))
		{
			equal_sign = ft_strchr(env_var, '=');
			if (equal_sign)
				return (ft_strdup_gc(equal_sign + 1, &data->gc));
		}
		i++;
	}
	return (NULL);
}

int	get_env_index(char *env_key, t_minishell *data)
{
	int		i;

	i = 0;
	while (data->envp[i])
	{
		if (ft_strncmp(data->envp[i], env_key, ft_strlen(env_key)) == 0)
			return (i);
		i++;
	}
	return (-1);
}
