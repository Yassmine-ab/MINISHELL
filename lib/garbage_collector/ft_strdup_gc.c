/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup_gc.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaabdall <yaabdall@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/15 10:01:11 by yaabdall          #+#    #+#             */
/*   Updated: 2024/11/17 00:29:50 by yaabdall         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gc.h"

char	*ft_strdup_gc(const char *s1, t_gc *gc)
{
	size_t		i;
	char		*str;

	if (!s1)
		return (ft_strdup_gc("", gc));
	i = -1;
	str = gc_malloc(sizeof(char) * (ft_strlen(s1) + 1), gc);
	while (s1[++i])
		str[i] = s1[i];
	str[i] = '\0';
	return (str);
}
