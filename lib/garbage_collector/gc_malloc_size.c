/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gc_malloc_size.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaabdall <yaabdall@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/17 03:10:14 by yaabdall          #+#    #+#             */
/*   Updated: 2024/11/17 03:13:33 by yaabdall         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gc.h"

size_t	gc_malloc_size(void *ptr, t_gc *gc)
{
	t_gc_node	*current;

	current = gc->head;
	while (current)
	{
		if (current->ptr == ptr)
			return (current->size);
		current = current->next;
	}
	return (0);
}
