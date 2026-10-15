/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaabdall <yaabdall@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/09 21:42:36 by yaabdall          #+#    #+#             */
/*   Updated: 2024/12/26 13:58:08 by yaabdall         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token	create_token(t_token_type type, char *value)
{
	t_token	token;

	token.type = type;
	if (value)
		token.value = value;
	else
		token.value = "";
	token.is_single_quoted = false;
	token.is_double_quoted = false;
	token.space_after = true;
	return (token);
}

void	skip_whitespace(char **input, int *index)
{
	while ((*input)[*index] && ft_isspace((*input)[*index]))
		(*index)++;
}

static bool	validate_tokens(t_minishell *data)
{
	int	i;

	i = -1;
	while (data->tokens[++i].type != END)
	{
		if (i == 0 && is_operator(data->tokens[i].type))
			return (error(data->tokens[i].value,
					": Unexpected operator at start", 2, data), false);
		if (data->tokens[i + 1].type == END && (is_redir(data->tokens[i].type)
				|| is_operator(data->tokens[i].type)))
			return (error(data->tokens[i].value,
					": Unexpected end of command after operator", 2, data),
				false);
		if ((is_operator(data->tokens[i].type)
				&& is_operator(data->tokens[i + 1].type))
			|| (is_redir(data->tokens[i].type)
				&& is_redir(data->tokens[i + 1].type)))
			return (error(NULL, "Consecutive operators", 2, data), false);
		else if (!is_operator(data->tokens[i].type)
			&& data->tokens[i + 1].type == PARENTHESIS_OPEN)
			return (error(data->tokens[i + 1].value,
					": Unexpected opening parenthesis after token", 2, data),
				false);
	}
	return (true);
}

t_token	*tokenize_input(char *input, t_minishell *data)
{
	int	count;
	int	i;

	count = 0;
	i = 0;
	while (input[i])
	{
		if (ft_isspace(input[i]))
			skip_whitespace(&input, &i);
		else if (ft_strchr("()", input[i]))
		{
			if (process_parentheses(input, &i, &count, data) == false)
				return (NULL);
		}
		else if (ft_strchr("|<>", input[i])
			|| (input[i] == '&' && input[i + 1] == '&'))
			process_operator(input, &i, &count, data);
		else
			process_word(input, &i, &count, data);
	}
	data->tokens[count] = create_token(END, NULL);
	if (validate_tokens(data) == false)
		return (NULL);
	return (data->tokens);
}
