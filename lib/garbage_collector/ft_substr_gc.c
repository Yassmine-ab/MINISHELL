/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr_gc.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaabdall <yaabdall@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/10 23:29:38 by yaabdall          #+#    #+#             */
/*   Updated: 2024/11/17 00:29:42 by yaabdall         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gc.h"

char	*ft_substr_gc(char *s, unsigned int start, size_t len, t_gc *gc)
{
	size_t	i;
	size_t	s_len;
	char	*str;

	i = -1;
	s_len = ft_strlen(s);
	if (!s)
		return (ft_strdup_gc("", gc));
	if (s_len < start)
		len = 0;
	else if (len > s_len - start)
		len = s_len - start;
	str = gc_malloc(sizeof(char) * (len + 1), gc);
	while (++i < len)
		str[i] = s[start + i];
	str[i] = '\0';
	return (str);
}
