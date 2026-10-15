/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa_gc.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaabdall <yaabdall@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/15 10:04:34 by yaabdall          #+#    #+#             */
/*   Updated: 2024/11/18 04:57:39 by yaabdall         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gc.h"

static size_t	ft_len(long n)
{
	size_t	len;

	if (!n)
		return (1);
	len = 0;
	if (n < 0)
	{
		n *= -1;
		len++;
	}
	while (n > 0)
	{
		n /= 10;
		len++;
	}
	return (len);
}

char	*ft_itoa_gc(int n, t_gc *gc)
{
	long	nbr;
	size_t	len;
	char	*str;

	nbr = (long)n;
	len = ft_len(nbr);
	str = gc_malloc(sizeof(char) * (len + 1), gc);
	if (n == 0)
		str[0] = '0';
	if (nbr < 0)
	{
		str[0] = '-';
		nbr *= -1;
	}
	str[len] = '\0';
	while (nbr > 0)
	{
		str[len - 1] = nbr % 10 + '0';
		nbr /= 10;
		len--;
	}
	return (str);
}
