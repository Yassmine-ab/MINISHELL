/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaabdall <yaabdall@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/22 11:22:57 by yaabdall          #+#    #+#             */
/*   Updated: 2024/12/25 12:38:33 by yaabdall         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

pid_t	safe_fork(t_minishell *data)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
		gc_cleanup(&data->gc);
	if (pid == 0)
	{
		data->is_child_process = true;
		signal_child_process();
	}
	return (pid);
}

static void	exec_left_child(t_node *ast, int pipe_fd[2], t_minishell *data)
{
	pid_t	left_child;

	left_child = safe_fork(data);
	if (left_child == 0)
	{
		dup2(pipe_fd[WRITE_END], STDOUT_FILENO);
		close(pipe_fd[READ_END]);
		close(pipe_fd[WRITE_END]);
		execute_ast(ast->left, data, true);
		gc_cleanup(&data->gc);
		exit(data->last_exit_status);
	}
}

static void	exec_right_child(t_node *ast, int pipe_fd[2], t_minishell *data)
{
	pid_t	right_child;

	right_child = safe_fork(data);
	if (right_child == 0)
	{
		dup2(pipe_fd[READ_END], STDIN_FILENO);
		close(pipe_fd[READ_END]);
		close(pipe_fd[WRITE_END]);
		execute_ast(ast->right, data, true);
		gc_cleanup(&data->gc);
		exit(data->last_exit_status);
	}
}

void	execute_pipeline(t_node *ast, t_minishell *data)
{
	int	pipe_fd[2];
	int	status;

	if (pipe(pipe_fd) == -1)
		(gc_cleanup(&data->gc), exit(EXIT_FAILURE));
	exec_left_child(ast, pipe_fd, data);
	exec_right_child(ast, pipe_fd, data);
	close(pipe_fd[READ_END]);
	close(pipe_fd[WRITE_END]);
	waitpid(-1, &status, 0);
	waitpid(-1, &status, 0);
	handle_child_exit(status, data);
}
