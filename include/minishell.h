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

#define B_SIZE 10
#define ft_isspace(x) ((x >= '\t' && x <= '\r') || x == ' ')
#define IS_REDIR(x) (x == t_append || x == t_heredoc || x == t_to || x == t_from || x == t_to_fd || x == t_from_fd)

#define ERROR(format, ...) dprintf(2, "dinosh: "format"\n", ##__VA_ARGS__)

#define SETPGRP(fd, gpid)								\
	{													\
		if (tcsetpgrp(fd, gpid) == -1)					\
			perror("dinosh: failed to tcsetpgrp");		\
	}

typedef enum {
	t_word,
	t_to,
	t_append,
	t_to_fd,
	t_from_fd,
	t_from,
	t_heredoc,
	t_var,
	t_wordvar,
	t_subshell,
	t_and,
	t_or,
	t_pipe,
	t_semicolon,
	t_bg,
	t_wildcard,
	t_backslash,
	t_control_group,
	t_arithmetic,
	t_tilde,
	t_control_substitution,
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
	empty_control_group,
	start_pipe,
	ambiguous_redirect,
	eheredoc,
	eopen,
	numeric_argument,
	missing_parameter,
	bad_substitution,
	special
} Error;
	
typedef enum {
	none,
	doubles,
	singles,
} Quotes;

/* === Parsing linked list ===*/
typedef struct Parser {
	char			*content;
	Token			token;
	struct Parser	*next;
	Error			error;
	int				id;
	int				expand_id;
	Quotes			quoting;
	bool			escaped;
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
	CONTROL_GROUP,
	ECHO,
	CD,
	PWD,
	EXPORT,
	UNSET,
	ENV,
	EXIT,
	SET,
	TYPE,
	JOBS,
	FG,
	BG,
	CONTROL_SUBSTITUTION,
	ALIAS,
	UNALIAS
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
	BACKGROUND
} Transmission;

typedef struct {
	int prev[2];
	int curr[2];
} Pipes;

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
	char			*error_message;
	char			*heredoc_file;
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
	char		*old_value;
	Special		type;
	struct Env	*next;
	int			durability;
} Env;

typedef struct Alias {
	char			*name;
	char			*value;
	struct Alias	*next;
} Alias;

typedef enum {
	DONE,
	RUNNING,
	STOPPED,
	CONTINUED
} State;

typedef struct Job {
	int			index;
	bool		is_current;
	State		state;
	pid_t		pid; // I keep it here because I play with the pid field of the command
	Command		*cmd;
	struct Job	*next;
} Job;

typedef struct {
	char	*input;
	Env		*env;
	Alias	*alias;
	bool	exit;
	int		code;
	Job		*jobs;
	pid_t	gpid;
	char	*access;
	bool	interactive;
} Context;

/* ====== MINISHELL ====== */
Parser	*mini_tokenizer(char *str);
Parser	*tokenize(char *str);
Command	*parse(Parser *data);
Type	get_builtin(char *name);
void	merge_one_node(Parser *head);
void	merge(Parser *head);
void	expand(Context *ctx, Command *cmd);
void	execute(Command *cmd, Context *ctx);
bool	read_file(char *file, Context *ctx);
void	builtin(Command *cmd, Context *ctx);

/* ====== SIGNALS ====== */
void	sig_handler(int sig);
int		rl_hook();

/* ====== ENV ====== */
Env		*create_env(char **env);
void	modify_env(Env **env, char *target, char *new_value, Special type, int dur);
void	delete_var(Env **env, char *target);
Env		*getvar(Env *env, char *target);
char	*ft_getenv(Env *env, char *target);
char	**get_envp(Env *env);
void	set_extern(Env *env, char *target);

/* ====== MEMORY ====== */
void	free_av(char **av);
void	free_cmd(Command *cmd);
void	free_cmds(Command *list, bool skip_background);
void	free_node(Parser *node);
void	free_list(Parser *list);
void	free_job(Job *job);
void	free_env(Env *env);
void	free_jobs(Job *job);
void	free_alias(Alias *alias);
void	unalias_all(Context *ctx);

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
char	*find_path(Env *env, char *cmd);
char	*get_random_file_name();
char	*resolve_globing(char *str, char *pattern, bool suffix);
void	add_tokenized_args(Parser *el, char *value, int max);
void	fork_routine(Command *head, Command *cmd, Context *ctx, Pipes *pipes);

/* ====== JOBS ====== */
void	add_job(Context *ctx, Command *cmd, State state);
void	print_job(Job *job, int code);
void	update_jobs(Context *ctx);
void	delete_job(Context *ctx, int id);
void	print_pid(Job *job);
bool	is_job(Context *ctx, Command *cmd);

/* ====== ERROR ====== */
bool	token_error(Parser *head);
bool	parse_error(Command *head);
bool	has_token_errors(Parser *head);
bool	command_error(Command *head);

/* ====== TESTS ====== */
void	tests_parsing(char **envp);
