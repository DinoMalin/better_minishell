#include "minishell.h"

#define IS_CHILD(x) (!x)
#define IS_PIPED(x) (x->from == PIPE || x->to == PIPE)
#define IS_BUILTIN(x) (x == ECHO || x == CD || x == PWD || x == EXPORT \
						|| x == UNSET|| x == ENV || x == ENV || x == EXIT)
#define xclose(x) {if (x != -1) close(x);}

#define DELETE_ARG(head, curr, prec)		\
	{										\
			Parser *next = curr->next;		\
			free_node(curr);				\
			if (curr == cmd->args)			\
				head = next;				\
			else							\
				prec->next = next;			\
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

#define FILL_HEREDOC()										\
	{														\
		for (int i = 0; curr->redirs[i].file; i++) {		\
			if (curr->redirs[i].type == r_heredoc) {		\
				unlink(HEREDOC_FILE);						\
				if (!heredoc(curr->redirs[i].file)) {		\
					perror("dinosh: heredoc");				\
					break;									\
				}											\
			}												\
		}													\
	}

#define CHECK_AND_OR()								\
	{												\
		if (curr->to == AND || curr->to == OR) {	\
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
#define HEREDOC_FILE "/tmp/dino_heredoc"

typedef enum {
	STORE,
	RESTORE
} StorageAction;

typedef struct {
	int prev[2];
	int curr[2];
} Pipes;

char	*find_path(Env *env, char *cmd);
void	init_redirs(Command *cmd);
void	init_av(Command *cmd);
void	redirect(Command *cmd);
void	redirect_pipe(Command *cmd, Pipes *pipes);
void	fd_storage(StorageAction action);
int		heredoc(char *lim);
