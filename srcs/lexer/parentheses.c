/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parentheses.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaabdall <yaabdall@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/10 02:55:32 by yaabdall          #+#    #+#             */
/*   Updated: 2024/12/26 13:42:23 by yaabdall         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	find_matching_parenthesis(char *input, int index)
{
	int	open_parentheses;

	open_parentheses = 1;
	index++;
	while (input[index])
	{
		if (input[index] == '(')
			open_parentheses++;
		else if (input[index] == ')')
		{
			open_parentheses--;
			if (open_parentheses == 0)
				return (index);
		}
		index++;
	}
	return (-1);
}

bool	process_parentheses(char *input, int *i, int *count, t_minishell *data)
{
	if (input[*i] == '(')
	{
		data->open_parentheses++;
		if (find_matching_parenthesis(input, *i) == -1)
			return (error("(", ": Unmatched opening parenthesis", 2, data),
				false);
		data->tokens[*count] = create_token(PARENTHESIS_OPEN, "(");
	}
	else if (input[*i] == ')')
	{
		if (data->open_parentheses == 0)
			return (error(&input[*i], ": Unmatched closing parenthesis",
					2, data), false);
		data->open_parentheses--;
		data->tokens[*count] = create_token(PARENTHESIS_CLOSE, ")");
	}
	(*i)++;
	(*count)++;
	return (true);
}
