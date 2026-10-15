/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besch <besch@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/26 18:00:40 by besch             #+#    #+#             */
/*   Updated: 2024/12/27 19:46:47 by besch            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	signal_child_process(void)
{
	rl_event_hook = sig_exit;
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	signal(SIGTSTP, SIG_IGN);
}

static void	sigint_heredoc(int sig)
{
	g_signal_received = 128 + sig;
	rl_done = 1;
}

void	init_signal_heredoc(void)
{
	struct sigaction	sa;

	rl_event_hook = sig_exit;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = &sigint_heredoc;
	sigaction(SIGINT, &sa, NULL);
	sa.sa_handler = SIG_IGN;
	sigaction(SIGQUIT, &sa, NULL);
	sigaction(SIGTSTP, &sa, NULL);
}

void	init_ignore_signal(void)
{
	struct sigaction	sa;

	rl_event_hook = sig_exit;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = SIG_IGN;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
	sigaction(SIGTSTP, &sa, NULL);
}

void	signal_to_action(t_minishell *data)
{
	if (g_signal_received == 128 + SIGINT && data->child_end_with_signal)
		ft_putchar_fd('\n', STDERR_FILENO);
	else if (g_signal_received == 128 + SIGQUIT && data->child_end_with_signal)
		ft_putstr_fd("Quit (core dumped)\n", STDERR_FILENO);
	data->child_end_with_signal = false;
	if (g_signal_received)
		data->last_exit_status = g_signal_received;
	g_signal_received = 0;
}
