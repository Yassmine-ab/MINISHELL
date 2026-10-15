/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gc_malloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besch <besch@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/11 20:30:49 by yaabdall          #+#    #+#             */
/*   Updated: 2024/12/27 19:44:21 by besch            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gc.h"

void	*gc_malloc(size_t size, t_gc *gc)
{
	void		*ptr;

	ptr = malloc(size);
	if (!ptr)
	{
		perror("Memory allocation failed");
		gc_cleanup(gc);
		exit(EXIT_FAILURE);
	}
	gc_add(gc, ptr, size);
	return (ptr);
}
