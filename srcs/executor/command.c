/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besch <besch@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 03:42:20 by yaabdall          #+#    #+#             */
/*   Updated: 2024/12/27 18:18:17 by besch            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	execute_builtins(t_node *current, t_minishell *data)
{
	if (ft_strncmp(current->value, "cd", 3) == 0)
		return (ft_cd(current->args, data), 1);
	else if (ft_strncmp(current->value, "echo", 5) == 0)
		return (ft_echo(current->args, data), 1);
	else if (ft_strncmp(current->value, "env", 4) == 0)
		return (ft_env(current->args, data), 1);
	else if (ft_strncmp(current->value, "exit", 5) == 0)
		return (ft_exit(current->args, data), 1);
	else if (ft_strncmp(current->value, "export", 7) == 0)
		return (ft_export(current->args, data), 1);
	else if (ft_strncmp(current->value, "pwd", 4) == 0)
		return (ft_pwd(current->args, data), 1);
	else if (ft_strncmp(current->value, "unset", 6) == 0)
		return (ft_unset(current->args, data), 1);
	return (0);
}

static void
	execute_command_in_child(t_node *ast, char **args, t_minishell *data)
{
	char	*path;

	path = get_command_path(args[0], data);
	if (path == NULL)
	{
		if (data->last_exec_error == EXEC_NOT_REGULAR)
			(safe_close(&data->saved_stdin), safe_close(&data->saved_stdout), \
				error(ast->value, ": Not a regular file", 126, data));
		if (data->last_exec_error == EXEC_NO_PERMISSION)
			(safe_close(&data->saved_stdin), safe_close(&data->saved_stdout), \
				error(ast->value, ": Permission denied", 126, data));
		(safe_close(&data->saved_stdin), safe_close(&data->saved_stdout));
		error(ast->value, ": Command not found", 127, data);
	}
	if (ast->redirections)
		execute_redirections(ast->redirections, data);
	safe_close(&data->saved_stdout);
	safe_close(&data->saved_stdin);
	execve(path, args, data->envp);
	error(ast->value, ": Command execution failed", 127, data);
}

static void	execute_extern_command(t_node *ast, char **args, t_minishell *data)
{
	pid_t	pid;
	int		status;

	pid = safe_fork(data);
	if (pid == 0)
	{
		data->is_child_process = true;
		execute_command_in_child(ast, args, data);
	}
	else
	{
		waitpid(pid, &status, 0);
		handle_child_exit(status, data);
	}
}

static bool	is_builtins(char *command)
{
	if (ft_strncmp(command, "cd", 3) == 0)
		return (true);
	else if (ft_strncmp(command, "echo", 5) == 0)
		return (true);
	else if (ft_strncmp(command, "env", 4) == 0)
		return (true);
	else if (ft_strncmp(command, "exit", 5) == 0)
		return (true);
	else if (ft_strncmp(command, "export", 7) == 0)
		return (true);
	else if (ft_strncmp(command, "pwd", 4) == 0)
		return (true);
	else if (ft_strncmp(command, "unset", 6) == 0)
		return (true);
	return (false);
}

void	execute_command(t_node *ast, t_minishell *data, bool in_child)
{
	char	**args;

	if (ast->is_single_quoted == false)
		ast->value = expand_variables(ast->value, data);
	concatenate_adjacent_nodes(ast, data);
	concatenate_adjacent_nodes(ast->args, data);
	args = get_command_args(ast, data);
	data->in_command = true;
	save_fds(&data->saved_stdin, &data->saved_stdout);
	if (ast->redirections && is_builtins(ast->value) == true)
		execute_redirections(ast->redirections, data);
	if (is_builtins(ast->value) == true)
		execute_builtins(ast, data);
	else
	{
		if (in_child)
			execute_command_in_child(ast, args, data);
		else
			execute_extern_command(ast, args, data);
	}
	restore_fds(&data->saved_stdin, &data->saved_stdout);
	data->in_command = false;
}
