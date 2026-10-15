/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gc_add.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaabdall <yaabdall@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/10 23:26:24 by yaabdall          #+#    #+#             */
/*   Updated: 2024/11/17 03:21:32 by yaabdall         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gc.h"

void	gc_add(t_gc *gc, void *ptr, size_t size)
{
	t_gc_node	*new_node;

	new_node = malloc(sizeof(t_gc_node));
	if (!new_node)
	{
		perror("Failed to add a new node");
		gc_cleanup(gc);
		exit(EXIT_FAILURE);
	}
	new_node->ptr = ptr;
	new_node->size = size;
	new_node->next = gc->head;
	new_node->locked = false;
	gc->head = new_node;
}
