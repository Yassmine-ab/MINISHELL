/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaabdall <yaabdall@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/24 16:44:17 by yaabdall          #+#    #+#             */
/*   Updated: 2024/12/25 11:07:10 by yaabdall         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_node	*parse_group(int *i, t_minishell *data)
{
	t_node	*group_node;

	group_node = create_node(NODE_GROUP, data->tokens[*i], &data->gc);
	(*i)++;
	group_node->left = parse_expression(i, data);
	if (group_node->left == NULL)
		return (error(NULL, "Empty parentheses", 2, data), NULL);
	(*i)++;
	return (group_node);
}

static void	append_nodes(t_node **current_list, t_node *nodes_to_add)
{
	t_node	*last;

	if (*current_list == NULL)
	{
		*current_list = nodes_to_add;
		return ;
	}
	last = *current_list;
	while (last->next)
		last = last->next;
	last->next = nodes_to_add;
}

static void	parse_arguments(int *i, t_node *cmd_node, t_minishell *data)
{
	t_node	*arg_node;

	while (data->tokens[*i].type == ARGUMENT)
	{
		arg_node = create_node(NODE_ARG, data->tokens[*i], &data->gc);
		append_nodes(&cmd_node->args, arg_node);
		(*i)++;
	}
}

static t_node
	*parse_simple_command(int *i, t_minishell *data, t_node *redir_before)
{
	t_node	*cmd_node;
	t_node	*new_cmd;
	t_node	*redir_after;

	cmd_node = NULL;
	while (data->tokens[*i].type == COMMAND)
	{
		new_cmd = create_node(NODE_COMMAND, data->tokens[*i], &data->gc);
		append_nodes(&cmd_node, new_cmd);
		(*i)++;
	}
	parse_arguments(i, cmd_node, data);
	if (redir_before)
		append_nodes(&cmd_node->redirections, redir_before);
	redir_after = parse_redirections(i, data);
	if (redir_after)
		append_nodes(&cmd_node->redirections, redir_after);
	return (cmd_node);
}

t_node	*parse_command(int *i, t_minishell *data, t_node *redir_before)
{
	t_node	*cmd_node;

	cmd_node = NULL;
	if (data->tokens[*i].type == PARENTHESIS_OPEN)
		cmd_node = parse_group(i, data);
	else if (data->tokens[*i].type == COMMAND)
		cmd_node = parse_simple_command(i, data, redir_before);
	return (cmd_node);
}
