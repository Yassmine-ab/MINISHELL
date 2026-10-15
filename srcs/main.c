/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaabdall <yaabdall@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/15 11:06:43 by yaabdall          #+#    #+#             */
/*   Updated: 2024/12/27 17:24:10 by yaabdall         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_signal_received = 0;

static void	parse_and_execute(t_minishell *data)
{
	t_node	*ast_root;
	int		i;

	i = 0;
	init_ignore_signal();
	ast_root = parse_expression(&i, data);
	if (g_signal_received == 0)
		execute_ast(ast_root, data, false);
	gc_cleanup_except_locked(&data->gc);
	ast_root = NULL;
}

static void	start_minishell(int argc, char **argv, t_minishell *data)
{
	char		*prompt;

	while (1)
	{
		init_signal_interactive_mode();
		data_init(argc, argv, data->envp, data);
		prompt = create_prompt(data);
		data->line = readline(prompt);
		if (g_signal_received)
			signal_to_action(data);
		if (data->line == 0)
			ft_exit(NULL, data);
		if (data->line[0])
		{
			add_history(data->line);
			if (tokenize_input(data->line, data))
				parse_and_execute(data);
		}
		free(data->line);
	}
}

int	main(int argc, char **argv, char **envp)
{
	t_minishell	data;

	if (isatty(STDIN_FILENO) == false)
		return (printf("Minishell does" RED " not " DEFAULT "accept input from "
				"non-interactive mode.\n"), 1);
	if (argc > 1)
		return (printf("Minishell does" RED " not " DEFAULT "accept arguments. "
				"Running in interactive mode only.\n"), 1);
	ft_memset(&data, 0, sizeof(t_minishell));
	data.envp = envp;
	rl_outstream = stderr;
	start_minishell(argc, argv, &data);
	rl_clear_history();
	gc_cleanup(&data.gc);
	return (data.last_exit_status);
}

// static void	print_ast(t_node *node, int depth)
// {
// 	const char	*colors[] = {
// 		RED,
// 		GREEN,
// 		YELLOW,
// 		BLUE,
// 		MAGENTA,
// 		CYAN,
// 		DEFAULT
// 	};
// 	const char	*color;
// 	int			num_colors;
// 	int			i;

// 	num_colors = sizeof(colors) / sizeof(colors[0]);
// 	color = colors[depth % num_colors];
// 	if (!node)
// 		return ;
// 	i = -1;
// 	while (++i < depth)
// 		printf("  ");
// 	printf("%s", color);
// 	if (node->value)
// 		printf("%s\n", node->value);
// 	else
// 		printf("(group)\n");
// 	(print_ast(node->left, depth + 1), print_ast(node->right, depth + 1));
// 	if (node->next)
// 		print_ast(node->next, depth);
// 	if (node->args)
// 		print_ast(node->args, depth + 1);
// 	if (node->redirections)
// 		print_ast(node->redirections, depth + 1);
// 	if (depth == 0)
// 		(printf(DEFAULT), fflush(stdout));
// }
