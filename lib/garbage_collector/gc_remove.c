/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gc_remove.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaabdall <yaabdall@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/17 03:11:21 by yaabdall          #+#    #+#             */
/*   Updated: 2024/11/17 03:13:43 by yaabdall         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gc.h"

void	gc_remove(t_gc *gc, void *ptr)
{
	t_gc_node	*current;
	t_gc_node	*prev;

	current = gc->head;
	prev = NULL;
	while (current)
	{
		if (current->ptr == ptr)
		{
			if (prev)
				prev->next = current->next;
			else
				gc->head = current->next;
			free(current);
			return ;
		}
		prev = current;
		current = current->next;
	}
}
