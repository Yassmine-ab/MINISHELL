/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gc_cleanup_except_locked.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaabdall <yaabdall@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/26 14:48:18 by yaabdall          #+#    #+#             */
/*   Updated: 2024/12/26 14:48:19 by yaabdall         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gc.h"

void	gc_cleanup_except_locked(t_gc *gc)
{
	t_gc_node	*current;
	t_gc_node	*prev;
	t_gc_node	*next;

	current = gc->head;
	prev = NULL;
	while (current)
	{
		next = current->next;
		if (!current->locked)
		{
			free(current->ptr);
			free(current);
			if (prev)
				prev->next = next;
			else
				gc->head = next;
		}
		else
			prev = current;
		current = next;
	}
}
