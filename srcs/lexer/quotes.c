/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quotes.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaabdall <yaabdall@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/10 02:54:14 by yaabdall          #+#    #+#             */
/*   Updated: 2024/12/22 19:41:56 by yaabdall         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	ft_strappend(char **str, char c, t_gc *gc)
{
	char	*new_str;
	size_t	len;

	if (str == NULL || *str == NULL)
		return ;
	len = ft_strlen(*str);
	new_str = gc_malloc((len + 2) * sizeof(char), gc);
	ft_memcpy(new_str, *str, len);
	new_str[len] = c;
	new_str[len + 1] = '\0';
	gc_free(str, gc);
	*str = new_str;
}

static int	find_matching_char(char *input, int char_index)
{
	char	match_char;

	match_char = input[char_index];
	char_index++;
	while (input[char_index])
	{
		if (match_char == '"' && input[char_index] == '\\'
			&& input[char_index + 1] == '"')
			char_index++;
		else if (input[char_index] == match_char)
			return (char_index);
		char_index++;
	}
	return (-1);
}

static int	handle_unclosed_char(char **input, int start, t_gc *gc)
{
	char	*additional_input;
	int		end;

	while (1)
	{
		ft_putstr_fd("Unclosed quote detected. Continue input > ", 1);
		additional_input = get_next_line(0);
		if (additional_input == NULL)
			return (-1);
		if (*additional_input)
			*input = ft_strjoin_gc(*input, additional_input, gc);
		gc_free(&additional_input, gc);
		end = find_matching_char(*input, start);
		if (end != -1)
			return (end);
	}
	return (-1);
}

char	*process_single_quotes(char *input, int *i, t_minishell *data)
{
	char		*value;
	int			end;

	value = ft_strdup_gc("", &data->gc);
	end = find_matching_char(input, *i);
	if (end == -1)
		end = handle_unclosed_char(&input, *i, &data->gc);
	while (++(*i) < end)
		ft_strappend(&value, input[*i], &data->gc);
	(*i)++;
	return (value);
}

char	*process_double_quotes(char *input, int *i, t_minishell *data)
{
	char		*value;
	int			end;

	value = ft_strdup_gc("", &data->gc);
	end = find_matching_char(input, *i);
	if (end == -1)
		end = handle_unclosed_char(&input, *i, &data->gc);
	while (++(*i) < end)
	{
		if (input[*i] == '\\' && (*i + 1 < end))
		{
			(*i)++;
			if (input[*i] == '"' || input[*i] == '\\')
				ft_strappend(&value, input[*i], &data->gc);
			else
			{
				ft_strappend(&value, '\\', &data->gc);
				ft_strappend(&value, input[*i], &data->gc);
			}
		}
		else
			ft_strappend(&value, input[*i], &data->gc);
	}
	(*i)++;
	return (value);
}
