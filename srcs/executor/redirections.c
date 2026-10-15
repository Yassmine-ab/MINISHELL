/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besch <besch@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 03:42:11 by yaabdall          #+#    #+#             */
/*   Updated: 2024/12/27 19:49:52 by besch            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	empty_heredoc(char *limiter, int count_line)
{
	ft_putstr_fd("warning: here-document at line ", STDERR_FILENO);
	ft_putnbr_fd(count_line, STDERR_FILENO);
	ft_putstr_fd(" delimited by end-of-file (wanted `", STDERR_FILENO);
	ft_putstr_fd(limiter, STDERR_FILENO);
	ft_putendl_fd("')", STDERR_FILENO);
}

static void	process_heredoc_lines(t_node *redir, \
	size_t hd_size, char *limiter, t_minishell *data)
{
	char	*line;

	int (count) = 0;
	while (++count > 0)
	{
		line = readline("heredoc > ");
		if (line == NULL)
		{
			empty_heredoc(limiter, count);
			break ;
		}
		if ((ft_strncmp(line, limiter, ft_strlen(limiter)) == 0
				&& line[ft_strlen(limiter)] == '\0') || g_signal_received)
		{
			gc_free(line, &data->gc);
			break ;
		}
		if (redir->right->is_single_quoted == false
			&& redir->right->is_double_quoted == false)
			line = expand_variables(line, data);
		strncat_realloc(&redir->hd, line, &hd_size, &data->gc);
		strncat_realloc(&redir->hd, "\n", &hd_size, &data->gc);
		gc_free(line, &data->gc);
	}
}

void	execute_heredoc(t_node *redir, t_minishell *data)
{
	char	*limiter;
	size_t	hd_size;

	limiter = redir->right->value;
	redir->hd = ft_strdup_gc("", &data->gc);
	hd_size = ft_strlen(redir->hd);
	process_heredoc_lines(redir, hd_size, limiter, data);
}

static void	redirect_fd(t_node *red, t_minishell *data)
{
	if (red->right->is_single_quoted == false)
		red->right->value = expand_variables(red->right->value, data);
	if (ft_strncmp(red->value, ">", 2) == 0)
	{
		data->fd = open(red->right->value, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (data->fd == -1)
			error(red->right->value, ": Failed to open file for writing",
				STDERR_FILENO, data);
		(dup2(data->fd, STDOUT_FILENO), safe_close(&data->fd));
	}
	else if (ft_strncmp(red->value, ">>", 3) == 0)
	{
		data->fd = open(red->right->value, O_WRONLY | O_CREAT | O_APPEND, 0644);
		if (data->fd == -1)
			error(red->right->value, ": Failed to open file for appending",
				STDERR_FILENO, data);
		(dup2(data->fd, STDOUT_FILENO), safe_close(&data->fd));
	}
	else if (ft_strncmp(red->value, "<", 2) == 0 && data->in_command == true)
	{
		data->fd = open(red->right->value, O_RDONLY, 0644);
		if (data->fd == -1)
			error(red->right->value, ": No such file or directory", 1, data);
		(dup2(data->fd, STDIN_FILENO), safe_close(&data->fd));
	}
}

void	execute_redirections(t_node *redir, t_minishell *data)
{
	while (redir)
	{
		if (redir->type == NODE_REDIR)
			redirect_fd(redir, data);
		else if (redir->type == NODE_HEREDOC)
		{
			if (pipe(data->heredoc_fd) == -1)
				(gc_cleanup(&data->gc), exit(EXIT_FAILURE));
			ft_putstr_fd(redir->hd, data->heredoc_fd[1]);
			redirect_heredoc(data);
		}
		redir = redir->next;
	}
}
