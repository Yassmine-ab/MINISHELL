/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besch <besch@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 04:15:58 by petitcoeur        #+#    #+#             */
/*   Updated: 2024/12/27 18:30:49 by besch            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	safe_close(int *fd)
{
	if (fd && *fd != -1)
	{
		if (close(*fd) == -1)
			ft_putendl_fd("Error closing file descriptor", 2);
		*fd = -1;
	}
}

void	save_fds(int *saved_stdin, int *saved_stdout)
{
	*saved_stdin = dup(STDIN_FILENO);
	*saved_stdout = dup(STDOUT_FILENO);
}

void	restore_fds(int *saved_stdin, int *saved_stdout)
{
	dup2(*saved_stdin, STDIN_FILENO);
	dup2(*saved_stdout, STDOUT_FILENO);
	safe_close(saved_stdin);
	safe_close(saved_stdout);
}

void	redirect_heredoc(t_minishell *data)
{
	dup2(data->heredoc_fd[READ_END], STDIN_FILENO);
	safe_close(&data->heredoc_fd[WRITE_END]);
	safe_close(&data->heredoc_fd[READ_END]);
}

void	handle_child_exit(int status, t_minishell *data)
{
	if (WIFEXITED(status))
		data->last_exit_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
	{
		data->last_exit_status = 128 + WTERMSIG(status);
		data->child_end_with_signal = true;
	}
}
