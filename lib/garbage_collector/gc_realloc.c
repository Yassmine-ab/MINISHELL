/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gc_realloc.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaabdall <yaabdall@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/17 03:02:44 by yaabdall          #+#    #+#             */
/*   Updated: 2024/11/17 03:17:34 by yaabdall         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gc.h"

void	*gc_realloc(void *ptr, size_t new_size, t_gc *gc)
{
	void	*new_ptr;
	size_t	old_size;
	size_t	size_to_copy;

	if (!ptr)
		return (gc_malloc(new_size, gc));
	old_size = gc_malloc_size(ptr, gc);
	new_ptr = gc_malloc(new_size, gc);
	if (old_size)
	{
		if (old_size < new_size)
			size_to_copy = old_size;
		else
			size_to_copy = new_size;
		ft_memcpy(new_ptr, ptr, size_to_copy);
		gc_free(ptr, gc);
	}
	return (new_ptr);
}
