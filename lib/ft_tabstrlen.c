/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_tabstrlen.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaabdall <yaabdall@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/24 19:11:09 by yaabdall          #+#    #+#             */
/*   Updated: 2024/12/13 19:52:32 by yaabdall         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_tabstrlen(char **tabstr)
{
	size_t	i;

	i = 0;
	while (tabstr && tabstr[i])
		i++;
	return (i);
}
