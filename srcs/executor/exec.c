/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaabdall <yaabdall@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/23 14:04:05 by petitcoeur        #+#    #+#             */
/*   Updated: 2024/12/26 13:09:57 by yaabdall         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	execute_and(t_node *ast, t_minishell *data)
{
	execute_ast(ast->left, data, false);
	if (data->last_exit_status == 0)
		execute_ast(ast->right, data, false);
}

static void	execute_or(t_node *ast, t_minishell *data)
{
	execute_ast(ast->left, data, false);
	if (data->last_exit_status != 0)
		execute_ast(ast->right, data, false);
}

static void	execute_group(t_node *ast, t_minishell *data)
{
	execute_ast(ast->left, data, false);
}

void	execute_ast(t_node *ast, t_minishell *data, bool in_child_process)
{
	struct termios	term;

	if (ast == NULL)
		return ;
	if (in_child_process)
		signal_child_process();
	else
		init_signal_exec();
	ft_memset(&term, 0, sizeof(term));
	tcgetattr(STDIN_FILENO, &term);
	if (ast->type == NODE_COMMAND)
		execute_command(ast, data, in_child_process);
	else if (ast->type == NODE_REDIR)
		execute_redirections(ast, data);
	else if (ast->type == NODE_PIPE)
		execute_pipeline(ast, data);
	else if (ast->type == NODE_AND)
		execute_and(ast, data);
	else if (ast->type == NODE_OR)
		execute_or(ast, data);
	else if (ast->type == NODE_GROUP)
		execute_group(ast, data);
	tcsetattr(STDIN_FILENO, TCSANOW, &term);
	signal_to_action(data);
}
