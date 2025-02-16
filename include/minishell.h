#include "libft.h"
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>

#define ft_isspace(x) ((x >= '\t' && x <= '\r') || x == ' ')
#define IS_REDIR(x) (x == t_append || x == t_heredoc || x == t_to || x == t_from || x == t_to_fd || x == t_from_fd)

typedef enum {
	t_word,
	t_to,
	t_append,
	t_to_fd,
	t_from_fd,
	t_from,
	t_heredoc,
	t_single_quotes,
	t_double_quotes,
	t_subshell,
	t_and,
	t_or,
	t_pipe,
	t_semicolon,
	t_wildcard,
	t_unknown,
	t_unexpected,
	t_missing_parameter,
} Token;

typedef enum {
	no_error,
	unclosed_token,
	empty_redir,
	unexpected_token,
	unknown_token,
	empty_subshell,
	start_pipe,
	ambiguous_redirect,
	eheredoc,
	eopen,
	numeric_argument,
	missing_parameter
} Error;

/* === Parsing linked list ===*/
typedef struct Parser {
	char			*content;
	Token			token;
	struct Parser	*next;
	Error			error;
	int				id;
	int				expand_id;
} Parser;

typedef enum {
	r_to,
	r_to_fd,
	r_append,
	r_from,
	r_from_fd,
	r_heredoc,
} Redir;

typedef enum {
	BASIC,
	SUBSHELL,
	VAR,
	ECHO,
	CD,
	PWD,
	EXPORT,
	UNSET,
	ENV,
	EXIT
} Type;

typedef struct {
	char	*file;
	Redir	type;
	int		n;
	int		word;
} t_redir;

typedef enum {
	ANY,
	PIPE,
	AND,
	OR,
	SEMICOLON,
} Transmission;

/* === Executing linked list ===*/
typedef struct Command {
	Parser			*args;
	char			**av;
	int				ac;
	t_redir			*redirs;
	Error			error;
	Type			type;
	Transmission	from;
	Transmission	to;
	int				exit_code; // used for builtins
	pid_t			pid;
	struct Command	*next;
} Command;

typedef enum {
	EXTERN,
	SPECIAL,
	INTERN
} Special;

typedef struct Env {
	char		*var;
	char		*value;
	Special		type;
	struct Env	*next;
	int			durability;
} Env;

typedef struct {
	char	*input;
	Env		*env;
	bool	exit;
	int		code;
} Context;

/* ====== MINISHELL ====== */
Parser	*mini_tokenizer(char *str);
Parser	*tokenize(char *str);
Command	*parse(Parser *data);
void	merge_one_node(Parser *head);
void	merge(Parser *head);
void	expand(Command *cmd, Env *env);
void	execute(Command *cmd, Context *ctx);
bool	read_file(char *file, Context *ctx);

/* ====== SIGNALS ====== */
void	sig_handler(int sig);
int		rl_hook();

/* ====== BUILTINS ====== */
Env		*create_env(char **env);
void	modify_env(Env **env, char *target, char *new_value, Special type, int dur);
void	delete_var(Env **env, char *target);
char	*ft_getenv(Env *env, char *target);
char	**get_envp(Env *env);
void	builtin(Command *cmd, Context *ctx);

/* ====== MEMORY ====== */
void	free_av(char **av);
void	free_cmds(Command *list);
void	free_node(Parser *node);
void	free_list(Parser *list);
void	free_env(Env *env);

/* ====== UTILS ====== */
char	*clean_join(char *origin, const char *to_join);
char	**clean_strsjoin(char **origin, char *to_join);
t_redir	*clean_redirjoin(t_redir *origin, t_redir to_join);
void	handle_input(Context *ctx);
void	update_code_var(Context *ctx);
void	init_basic_vars(Context *ctx);
bool	this_id_has_wildcard(Parser *head);
void	read_token(Parser *head);
bool	is_number(char *str);
bool	var_is_valid(char *name);

/* ====== ERROR ====== */
bool	token_error(Parser *head);
bool	parse_error(Command *head);
bool	has_token_errors(Parser *head);
bool	command_error(Command *head);

/* ====== TESTS ====== */
void	tests_parsing(char **envp);
