/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besch <besch@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/11 01:49:39 by yaabdall          #+#    #+#             */
/*   Updated: 2024/12/27 18:58:39 by besch            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_node	*create_node(t_node_type type, t_token token, t_gc *gc)
{
	t_node	*node;

	node = gc_calloc(1, sizeof(t_node), gc);
	node->type = type;
	node->value = token.value;
	node->is_single_quoted = token.is_single_quoted;
	node->is_double_quoted = token.is_double_quoted;
	node->space_after = token.space_after;
	return (node);
}

static t_node	*parse_heredoc(int *i, t_minishell *data)
{
	t_node	*hd_node;

	hd_node = create_node(NODE_HEREDOC, data->tokens[*i], &data->gc);
	(*i)++;
	hd_node->right = create_node(NODE_LIMITER, data->tokens[*i], &data->gc);
	(*i)++;
	init_signal_heredoc();
	execute_heredoc(hd_node, data);
	return (hd_node);
}

t_node	*parse_redirections(int *i, t_minishell *data)
{
	t_node	*redir_node;
	t_node	*last_redir;

	t_node *(redir_list) = NULL;
	while (is_redir(data->tokens[*i].type))
	{
		if (g_signal_received)
			return (NULL);
		if (data->tokens[*i].type == HEREDOC)
			redir_node = parse_heredoc(i, data);
		else
		{
			redir_node = create_node(NODE_REDIR, data->tokens[*i], &data->gc);
			(*i)++;
			redir_node->right = create_node(NODE_FILE, data->tokens[*i], \
			&data->gc);
			(*i)++;
		}
		if (redir_list == NULL)
			redir_list = redir_node;
		else
			last_redir->next = redir_node;
		last_redir = redir_node;
	}
	return (redir_list);
}

static t_node	*parse_pipeline(int *i, t_minishell *data)
{
	t_node	*left;
	t_node	*pipe_node;
	t_node	*redir_list;

	redir_list = parse_redirections(i, data);
	left = parse_command(i, data, redir_list);
	if (left == NULL)
		left = redir_list;
	while (data->tokens[*i].type == PIPE)
	{
		pipe_node = create_node(NODE_PIPE, data->tokens[*i], &data->gc);
		(*i)++;
		pipe_node->right = parse_command(i, data, redir_list);
		pipe_node->left = left;
		left = pipe_node;
	}
	return (left);
}

t_node	*parse_expression(int *i, t_minishell *data)
{
	t_node		*left;
	t_node_type	op_type;
	t_node		*op_node;

	left = parse_pipeline(i, data);
	while (data->tokens[*i].type == AND || data->tokens[*i].type == OR)
	{
		if (data->tokens[*i].type == AND)
			op_type = NODE_AND;
		else
			op_type = NODE_OR;
		op_node = create_node(op_type, data->tokens[*i], &data->gc);
		(*i)++;
		op_node->right = parse_pipeline(i, data);
		op_node->left = left;
		left = op_node;
	}
	return (left);
}
