#include "minishell.h"

#define IS_CHILD(x) (!x)
#define IS_PIPED(x) (x->from == PIPE || x->to == PIPE)
#define IS_BUILTIN(x) (	   x == ECHO	|| x == CD		|| x == PWD || x == EXPORT	\
						|| x == UNSET	|| x == ENV		|| x == ENV || x == EXIT	\
						|| x == SET		|| x == TYPE	|| x == FG	|| x == JOBS	\
						|| x == BG		|| x == UNALIAS)
#define xclose(x) {if (x != -1) close(x);}
#define IS_AMBIGUOUS(x) (x->next->expand_id == x->expand_id && x->expand_id != -1)

#define UPDATE_VAR_DURABILITY(env)					\
	{												\
		Env *var = env;								\
		while (var) {								\
			Env *next = var->next;					\
			if (var->durability != -1)				\
				var->durability--;					\
			if (!var->durability) {					\
				if (var->old_value) {				\
					free(var->value);				\
					var->value = var->old_value;	\
					var->old_value = NULL;			\
					var->durability = -1;			\
				} else {							\
					delete_var(&env, var->var);		\
				}									\
			}										\
			var = next;								\
		}											\
	}

#define DELETE_ARG(head, curr, prec)		\
	{										\
			if (curr == cmd->args) {		\
				head = curr->next;			\
				prec = curr->next;			\
			} else {						\
				prec->next = curr->next;	\
			}								\
			free_node(curr);				\
	}

#define DO_PIPE()							\
	{										\
		pipes.curr[0] = -1;					\
		pipes.curr[1] = -1;					\
		if (curr->to == PIPE) {				\
			if (pipe(pipes.curr) < 0) {		\
				perror("dinosh: pipe");		\
				xclose(pipes.prev[0]);		\
				xclose(pipes.prev[1]);		\
				return;						\
			}								\
		}									\
	}

#define CHECK_TRANSMISSION()						\
	{												\
		if (curr->to == BACKGROUND) {				\
			add_job(ctx, curr, RUNNING);			\
		}											\
		if (curr->to == AND							\
			|| curr->to == OR						\
			|| curr->to == SEMICOLON) {				\
			wait_everything(wait, curr, ctx);		\
			wait = curr->next;						\
		}											\
		if (curr->to == AND && ctx->code == 1)		\
			break;									\
		if (curr->to == OR && ctx->code == 0)		\
			break;									\
	}

#define TO_FLAGS O_WRONLY | O_CREAT | O_TRUNC
#define APPEND_FLAGS O_WRONLY | O_CREAT | O_APPEND
#define FROM_FLAGS O_RDONLY

typedef enum {
	STORE,
	RESTORE
} StorageAction;

bool	init_command(Context *ctx, Command *cmd);
void	redirect(Command *cmd);
void	redirect_pipe(Command *cmd, Pipes *pipes);
void	fd_storage(StorageAction action);
int		heredoc(char *heredoc_file, char *lim);
void	fill_heredoc(Command *head);
