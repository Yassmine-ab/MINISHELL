/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split_gc.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaabdall <yaabdall@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/19 10:46:38 by yaabdall          #+#    #+#             */
/*   Updated: 2024/12/28 15:42:21 by yaabdall         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gc.h"

static size_t	ft_word_len(char const *s, char c)
{
	size_t	i;

	i = 0;
	while (s[i] && s[i] != c)
		i++;
	return (i);
}

static size_t	ft_nb_words(char const *s, char c)
{
	size_t	i;
	size_t	nb;

	i = 0;
	nb = 0;
	while (s[i])
	{
		if (s[i] != c)
		{
			nb++;
			while (s[i] && s[i] != c)
				i++;
		}
		else
			i++;
	}
	return (nb);
}

static char	**ft_free(char **s, size_t i, t_gc *gc)
{
	while (i > 0)
	{
		i--;
		gc_free(s[i], gc);
	}
	gc_free(s, gc);
	return (NULL);
}

char	**ft_split_gc(char *s, char c, t_gc *gc)
{
	size_t	i;
	size_t	j;
	char	**tab_str;

	i = -1;
	j = 0;
	if (!s)
		return (NULL);
	tab_str = (char **)gc_malloc(sizeof(char *) * (ft_nb_words(s, c) + 1), gc);
	if (!tab_str)
		return (NULL);
	while (++i < ft_nb_words(s, c))
	{
		while (s[j] && s[j] == c)
			j++;
		tab_str[i] = ft_substr_gc(s, j, ft_word_len(&s[j], c), gc);
		if (!tab_str[i])
			return (ft_free(tab_str, i, gc));
		while (s[j] && s[j] != c)
			j++;
	}
	tab_str[i] = NULL;
	return (tab_str);
}
