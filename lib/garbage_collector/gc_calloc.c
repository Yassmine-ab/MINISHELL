/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gc_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaabdall <yaabdall@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/28 15:42:14 by yaabdall          #+#    #+#             */
/*   Updated: 2024/12/28 15:42:34 by yaabdall         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gc.h"

void	*gc_calloc(size_t nmemb, size_t size, t_gc *gc)
{
	void	*ptr;

	ptr = calloc(nmemb, size);
	if (!ptr)
	{
		perror("Memory allocation failed");
		gc_cleanup(gc);
		exit(EXIT_FAILURE);
	}
	gc_add(gc, ptr, nmemb * size);
	return (ptr);
}
