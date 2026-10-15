/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: petitcoeur <petitcoeur@student.42.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 15:57:24 by besch             #+#    #+#             */
/*   Updated: 2024/12/25 06:24:58 by petitcoeur       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	export_invalid_option(char *key, t_minishell *data)
{
	ft_putstr_fd("export: ", STDERR_FILENO);
	ft_putchar_fd(key[0], STDERR_FILENO);
	ft_putchar_fd(key[1], STDERR_FILENO);
	ft_putstr_fd(": invalid option\n", STDERR_FILENO);
	ft_putstr_fd("export: usage: export [name[=value] ...] or export -p\n",
		STDERR_FILENO);
	data->last_exit_status = 2;
	return (1);
}

static int	export_invalid_identifier(char *key, t_minishell *data)
{
	ft_putstr_fd("export: `", STDERR_FILENO);
	ft_putstr_fd(key, STDERR_FILENO);
	ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
	data->last_exit_status = 1;
	return (1);
}

static int	var_key_checks(char *key, bool is_equal, t_minishell *data)
{
	int	i;

	if (key[0] == '-' && key[1])
		return (export_invalid_option(key, data));
	if (!ft_isalpha(key[0]) && key[0] != '_')
		return (export_invalid_identifier(key, data));
	i = 0;
	while (key[++i])
	{
		if (is_equal == true && (key[i] == '+' && key[i + 1] == '\0'))
			return (2);
		if (!ft_isalnum(key[i]) && key[i] != '_')
			return (export_invalid_identifier(key, data));
	}
	return (0);
}

static void	handle_export_with_value(char *arg, t_minishell *data)
{
	char	*key;
	char	*equal_sign;
	int		var_key_checks_return;

	equal_sign = ft_strchr(arg, '=');
	key = ft_substr_gc(arg, 0, equal_sign - arg, &data->gc);
	var_key_checks_return = var_key_checks(key, true, data);
	process_var_key_return(var_key_checks_return, arg, equal_sign, data);
}

void	ft_export(t_node *cmd_args, t_minishell *data)
{
	char	*arg;
	char	*equal_sign;
	char	*key;

	if (!cmd_args)
		print_export(data);
	while (cmd_args)
	{
		arg = cmd_args->value;
		equal_sign = ft_strchr(arg, '=');
		if (!equal_sign)
		{
			if (!var_key_checks(arg, false, data))
			{
				key = ft_strdup_gc(arg, &data->gc);
				if (get_env_index(key, data) == -1)
					add_env(key, NULL, data);
				gc_free(key, &data->gc);
			}
		}
		else
			handle_export_with_value(arg, data);
		cmd_args = cmd_args->next;
	}
}
