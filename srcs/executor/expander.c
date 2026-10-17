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

static bool	is_dot_entry(const char *name)
{
	if (name[0] != '.')
		return (false);
	if (name[1] == '\0' || (name[1] == '.' && name[2] == '\0'))
		return (true);
	return (false);
}

static int	compare_entries(const char *s1, const char *s2)
{
	size_t	i;

	i = 0;
	while (s1[i] && s1[i] == s2[i])
		i++;
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}

static void	sort_matches(char **matches, int count)
{
	char	*temp;
	int		i;
	int		j;

	i = -1;
	while (++i < count - 1)
	{
		j = -1;
		while (++j < count - 1 - i)
		{
			if (compare_entries(matches[j], matches[j + 1]) > 0)
			{
				temp = matches[j];
				matches[j] = matches[j + 1];
				matches[j + 1] = temp;
			}
		}
	}
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
	while (entry)
	{
		if (count < MAX_MATCHES && is_dot_entry(entry->d_name) == false
			&& (entry->d_name[0] != '.' || pattern[0] == '.')
			&& fnmatch(pattern, entry->d_name, 0) == 0)
			matches[count++] = ft_strdup_gc(entry->d_name, &data->gc);
		entry = readdir(dir);
	}
	closedir(dir);
	matches[count] = NULL;
	if (count == 0)
		return (gc_free(matches, &data->gc), NULL);
	sort_matches(matches, count);
	return (matches);
}

char	**expand_wildcard(char *pattern, t_minishell *data)
{
	if (pattern == NULL || ft_strchr(pattern, '*') == NULL)
		return (NULL);
	if (ft_strchr(pattern, '/') != NULL)
		return (NULL);
	return (find_wildcard_matches(pattern, data));
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
	return (process_expansion(value, data, expand_env_variable, '$'));
}
