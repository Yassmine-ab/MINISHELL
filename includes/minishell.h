/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaabdall <yaabdall@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/11 02:04:44 by yaabdall          #+#    #+#             */
/*   Updated: 2024/12/28 16:33:46 by yaabdall         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

/* -------------------------------------------------------------------------- */
/*                                  INCLUDES                                  */
/* -------------------------------------------------------------------------- */

# include <stdio.h>
# include <unistd.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <stdbool.h>
# include <errno.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <dirent.h>
# include "get_next_line_bonus.h"
# include "gc.h"
# include <fnmatch.h>
# include <termios.h>
# include <signal.h>
# include <fcntl.h>

/* -------------------------------------------------------------------------- */
/*                                  DEFINES                                   */
/* -------------------------------------------------------------------------- */

// COLORS
# define COLOR_ENABLED 1
# if COLOR_ENABLED
#  define DEFAULT "\001\033[0m"
#  define RED "\001\033[1;31m"
#  define GREEN "\033[1;32m\002"
#  define YELLOW "\001\033[1;33m"
#  define BLUE "\001\033[1;34m"
#  define MAGENTA "\001\033[1;35m"
#  define ROSE "\001\033[1;95m"
#  define CYAN "\033[1;36m\002"
#  define WHITE "\001\033[1;37m"
# else
#  define DEFAULT ""
#  define RED ""
#  define GREEN ""
#  define YELLOW ""
#  define BLUE ""
#  define MAGENTA ""
#  define ROSE ""
#  define CYAN ""
#  define WHITE ""
# endif

// MAX VALUES
# define MAX_PATHLENGTH 4096
# define MAX_TOKENS 4096
# define MAX_MATCHES 4096

// PIPE FILE DESCRIPTORS
# define READ_END 0
# define WRITE_END 1

/* -------------------------------------------------------------------------- */
/*                               EXTERN VARIABLE                              */
/* -------------------------------------------------------------------------- */

extern volatile sig_atomic_t	g_signal_received;

/* -------------------------------------------------------------------------- */
/*                                   ENUMS                                    */
/* -------------------------------------------------------------------------- */

// TOKEN TYPES ENUM
typedef enum e_token_type
{
	COMMAND,
	ARGUMENT,
	PIPE,
	STDIN,
	STDOUT,
	STDOUT_APPEND,
	FILENAME,
	HEREDOC,
	LIMITER,
	AND,
	OR,
	PARENTHESIS_OPEN,
	PARENTHESIS_CLOSE,
	END
}	t_token_type;

// AST NODE TYPES ENUM
typedef enum e_node_type
{
	NODE_COMMAND,
	NODE_ARG,
	NODE_PIPE,
	NODE_REDIR,
	NODE_FILE,
	NODE_HEREDOC,
	NODE_LIMITER,
	NODE_AND,
	NODE_OR,
	NODE_GROUP
}	t_node_type;

typedef enum e_exec_error
{
	EXEC_SUCCESS = 1,
	EXEC_NO_FILE = 2,
	EXEC_NOT_REGULAR = 3,
	EXEC_NO_PERMISSION = 4
}	t_exec_error;

/* -------------------------------------------------------------------------- */
/*                                 STRUCTURES                                 */
/* -------------------------------------------------------------------------- */

// TOKEN STRUCTURE
typedef struct s_token
{
	t_token_type		type;
	char				*value;
	bool				is_single_quoted;
	bool				is_double_quoted;
	bool				space_after;
}	t_token;

// AST NODE STRUCTURE
typedef struct s_node
{
	t_node_type			type;
	char				*value;
	char				*hd;
	bool				is_single_quoted;
	bool				is_double_quoted;
	bool				space_after;
	struct s_node		*left;
	struct s_node		*right;
	struct s_node		*args;
	struct s_node		*redirections;
	struct s_node		*next;
}	t_node;

// DATA STRUCTURE
typedef struct s_minishell
{
	char				**envp;
	char				**argv;
	char				*line;
	char				*result;
	int					argc;
	int					fd;
	int					heredoc_fd[2];
	int					saved_stdin;
	int					saved_stdout;
	int					open_parentheses;
	int					last_exit_status;
	bool				is_child_process;
	bool				child_end_with_signal;
	bool				in_command;
	bool				in_single_quotes;
	bool				in_double_quotes;
	t_gc				gc;
	t_token				*tokens;
	t_token_type		current_type;
	t_node				*node;
	t_exec_error		last_exec_error;
}	t_minishell;

/* -------------------------------------------------------------------------- */
/*                             FUNCTION PROTOTYPES                            */
/* -------------------------------------------------------------------------- */

/* ----------------------------- Initialization ----------------------------- */
void	data_init(int argc, char **argv, char **envp, t_minishell *data);

/* --------------------------------- Prompt --------------------------------- */
char	*create_prompt(t_minishell *data);

/* --------------------------------- Lexer ---------------------------------- */
t_token	*tokenize_input(char *input, t_minishell *data);
t_token	create_token(t_token_type type, char *value);
void	skip_whitespace(char **input, int *index);
void	process_operator(char *input, int *i, int *count, t_minishell *data);
void	process_word(char *input, int *i, int *count, t_minishell *data);
char	*process_single_quotes(char *input, int *i, t_minishell *data);
char	*process_double_quotes(char *input, int *i, t_minishell *data);
bool	process_parentheses(char *input, int *i, int *count, t_minishell *data);

/* -------------------------------- Parser ---------------------------------- */
t_node	*create_node(t_node_type type, t_token token, t_gc *gc);
t_node	*parse_expression(int *i, t_minishell *data);
t_node	*parse_command(int *i, t_minishell *data, t_node *redir_before);
t_node	*parse_redirections(int *i, t_minishell *data);

/* ------------------------------- Executor --------------------------------- */
void	execute_ast(t_node *ast, t_minishell *data, bool in_child);
void	execute_command(t_node *cmd_node, t_minishell *data, bool in_child);
void	execute_pipeline(t_node *ast, t_minishell *data);
void	execute_redirections(t_node *redir_node, t_minishell *data);
void	handle_child_exit(int status, t_minishell *data);
void	concatenate_adjacent_nodes(t_node *node, t_minishell *data);
char	*get_command_path(char *command, t_minishell *data);
char	**get_command_args(t_node *cmd_node, t_minishell *data);
void	execute_heredoc(t_node *redir, t_minishell *data);
pid_t	safe_fork(t_minishell *data);

/* ------------------------------- Expander --------------------------------- */
char	*expand_variables(char *value, t_minishell *data);

/* ------------------------------ Environment ------------------------------- */
void	free_envp(t_minishell *data);
char	*get_env_value(char *env_key, t_minishell *data);
int		get_env_index(char *env_key, t_minishell *data);
void	update_env(char *key, char *new_value, t_minishell *data);
void	add_env(char *key, char *value, t_minishell *data);

/* -------------------------------- Signal ---------------------------------- */
void	init_signal_interactive_mode(void);
void	init_ignore_signal(void);
void	init_signal_heredoc(void);
void	init_signal_exec(void);
int		sig_exit(void);
void	signal_child_process(void);
void	signal_to_action(t_minishell *data);

/* ------------------------------- Builtins --------------------------------- */
void	ft_echo(t_node *args, t_minishell *data);
void	ft_cd(t_node *args, t_minishell *data);
void	ft_pwd(t_node *args, t_minishell *data);
void	ft_env(t_node *args, t_minishell *data);
void	ft_unset(t_node *args, t_minishell *data);
void	ft_export(t_node *args, t_minishell *data);
void	ft_exit(t_node *args, t_minishell *data);

/* ------------------------------- Utilities -------------------------------- */
void	set_gc_node_locked(t_gc *gc, void *ptr, bool lock);
void	error(char *context, char *error_msg, int status, t_minishell *data);
void	strncat_realloc(char **result, char *append, size_t *size, t_gc *gc);
bool	is_operator(t_token_type type);
bool	is_redir(t_token_type type);
void	safe_close(int *fd);
void	save_fds(int *saved_stdin, int *saved_stdout);
void	restore_fds(int *saved_stdin, int *saved_stdout);
void	redirect_heredoc(t_minishell *data);
void	print_export(t_minishell *data);
void	process_var_key_return(int var_key_checks_return, \
char *arg, char *equal_sign, t_minishell *data);

#endif
