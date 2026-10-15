/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   words.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaabdall <yaabdall@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/10 19:30:54 by yaabdall          #+#    #+#             */
/*   Updated: 2024/12/24 22:59:30 by yaabdall         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*extract_value(char *input, int *i, t_minishell *data)
{
	char	*value;
	char	*temp;

	value = ft_strdup_gc("", &data->gc);
	while (input[*i]
		&& ft_isspace(input[*i]) == 0 && ft_strchr("()|<>", input[*i]) == NULL)
	{
		if (input[*i] == '&' && input[*i + 1] && input[*i + 1] == '&')
			break ;
		else if (input[*i] == '\'')
			return (data->in_single_quotes = true,
				process_single_quotes(input, i, data));
		else if (input[*i] == '"')
			return (data->in_double_quotes = true,
				process_double_quotes(input, i, data));
		else
		{
			temp = ft_substr_gc(input, *i, 1, &data->gc);
			value = ft_strjoin_gc(value, temp, &data->gc);
			(*i)++;
			if (input[*i] == '\'' || input[*i] == '"')
				break ;
		}
	}
	return (value);
}

void	process_word(char *input, int *i, int *count, t_minishell *data)
{
	char	*value;

	value = extract_value(input, i, data);
	data->tokens[*count] = create_token(data->current_type, value);
	data->tokens[*count].is_single_quoted = data->in_single_quotes;
	data->tokens[*count].is_double_quoted = data->in_double_quotes;
	if (ft_isspace(input[*i]))
		data->tokens[*count].space_after = 1;
	else
		data->tokens[*count].space_after = 0;
	if (data->current_type == COMMAND && data->tokens[*count].space_after == 1)
		data->current_type = ARGUMENT;
	else if ((data->current_type == FILENAME || data->current_type == LIMITER)
		&& data->tokens[*count].space_after == 1)
		data->current_type = COMMAND;
	(*count)++;
	skip_whitespace(&input, i);
}
