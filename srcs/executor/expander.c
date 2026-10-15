/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaabdall <yaabdall@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 21:54:13 by yaabdall          #+#    #+#             */
/*   Updated: 2024/12/28 17:27:42 by yaabdall         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int
	expand_env_variable(size_t *size, int i, char *str, t_minishell *data)
{
	char	*var_name;
	char	*value;
	int		start;

	if (str[++i] == '?')
		value = ft_itoa_gc(data->last_exit_status, &data->gc);
	else if ((ft_isdigit(str[i]) && (str[i] - '0') < data->argc)
		|| str[i] == '_')
		value = data->argv[0];
	else if (ft_isalnum(str[i]))
	{
		start = i;
		while (str[i] && (ft_isalnum(str[i])
				|| str[i] == '_'))
			i++;
		var_name = ft_substr_gc(str, start, i - start, &data->gc);
		value = get_env_value(var_name, data);
		if (value == NULL)
			value = "";
		i--;
	}
	else
		return (strncat_realloc(&data->result, "$", size, &data->gc), i);
	return (strncat_realloc(&data->result, value, size, &data->gc), i + 1);
}

static char	**find_wildcard_matches(char *pattern, t_minishell *data)
{
	DIR				*dir;
	struct dirent	*entry;
	char			**matches;
	int				count;

	matches = gc_calloc(MAX_MATCHES + 1, sizeof(char *), &data->gc);
	count = 0;
	dir = opendir(".");
	if (dir == NULL)
		return (ft_putendl_fd("Unable to open directory", 2), NULL);
	entry = readdir(dir);
	while (1)
	{
		entry = readdir(dir);
		if (entry == NULL)
			break ;
		if ((entry->d_name[0] != '.' && pattern[0] != '.')
			&& fnmatch(pattern, entry->d_name, 0) == 0)
			matches[count++] = ft_strdup_gc(entry->d_name, &data->gc);
	}
	closedir(dir);
	if (count == 0)
		matches[count++] = ft_strdup_gc(ft_strchr(pattern, '*'), &data->gc);
	matches[count] = NULL;
	return (matches);
}

static int	expand_wildcard(size_t *size, int i, char *str, t_minishell *data)
{
	char		**matches;
	char		*pattern;
	const int	start_writing = i;
	int			start_pattern;
	int			match_idx;

	while (i > 0 && ft_isspace(str[i - 1]) == 0
		&& ft_strchr("\"'()|<>&", str[i - 1]) == NULL)
		i--;
	start_pattern = i;
	while (str[i]
		&& ft_isspace(str[i]) == 0 && ft_strchr("\"'()|<>&", str[i]) == NULL)
		i++;
	pattern = ft_substr_gc(str, start_pattern, i - start_pattern, &data->gc);
	printf("pattern: %s\n", pattern);
	matches = find_wildcard_matches(pattern, data);
	if (matches)
	{
		match_idx = -1;
		while (matches[++match_idx])
		{
			if (match_idx > 0)
				strncat_realloc(&data->result, " ", size, &data->gc);
			if (match_idx == 0)
				strncat_realloc(&data->result, &matches[match_idx][start_writing], size, &data->gc);
			else
				strncat_realloc(&data->result, matches[match_idx], size, &data->gc);
		}
	}
	return (i);
}

static char	*process_expansion(char *value, t_minishell *data, \
int (*expander)(size_t *, int, char *, t_minishell *), char expansion_char)
{
	size_t	size;
	char	append[2];
	int		i;

	ft_memset(append, 0, 2);
	size = ft_strlen(value) + 1;
	data->result = gc_malloc((sizeof(char) * size), &data->gc);
	data->result[0] = '\0';
	i = 0;
	while (value[i])
	{
		if (value[i] == expansion_char)
			i = expander(&size, i, value, data);
		else
		{
			while (value[i] && value[i] != expansion_char)
			{
				append[0] = value[i++];
				strncat_realloc(&data->result, append, &size, &data->gc);
			}
		}
	}
	return (data->result);
}

char	*expand_variables(char *value, t_minishell *data)
{
	if (value == NULL)
		return (NULL);
	value = process_expansion(value, data, expand_env_variable, '$');
	value = process_expansion(value, data, expand_wildcard, '*');
	return (value);
}
