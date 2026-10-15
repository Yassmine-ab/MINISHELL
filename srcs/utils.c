/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaabdall <yaabdall@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 05:22:40 by yaabdall          #+#    #+#             */
/*   Updated: 2024/12/27 16:53:17 by yaabdall         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	error(char *context, char *error_msg, int status, t_minishell *data)
{
	if (context)
		ft_putstr_fd(context, 2);
	ft_putendl_fd(error_msg, STDERR_FILENO);
	if (data->is_child_process)
	{
		gc_cleanup(&data->gc);
		exit(status);
	}
}

void	set_gc_node_locked(t_gc *gc, void *ptr, bool lock)
{
	t_gc_node	*current;

	current = gc->head;
	while (current)
	{
		if (current->ptr == ptr)
		{
			current->locked = lock;
			break ;
		}
		current = current->next;
	}
}

bool	is_redir(t_token_type type)
{
	return (type == STDIN || type == STDOUT || type == STDOUT_APPEND
		|| type == HEREDOC);
}

bool	is_operator(t_token_type type)
{
	return (type == PIPE || type == AND || type == OR);
}

void	strncat_realloc(char **result, char *append, size_t *size, t_gc *gc)
{
	size_t	new_len;

	new_len = ft_strlen(*result) + ft_strlen(append) + 1;
	if (new_len > *size)
	{
		*size = new_len;
		*result = gc_realloc(*result, *size, gc);
	}
	ft_strlcat(*result, append, *size);
}
