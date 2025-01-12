#include "minishell.h"

#define IS_CHILD(x) (!x)
#define IS_PIPED(x) (x->from == PIPE && x->to == PIPE)
#define IS_BUILTIN(x) (x == ECHO || x == CD || x == PWD || x == EXPORT \
						|| x == UNSET|| x == ENV || x == ENV || x == EXIT)
#define xclose(x) {if (x != -1) close(x);}

#define DO_PIPE()							\
	{										\
		if (curr->to == PIPE) {				\
			if (pipe(pipes.curr) < 0) {		\
				perror("dinosh: pipe");		\
				xclose(pipes.prev[0]);		\
				xclose(pipes.prev[1]);		\
				return;						\
			}								\
		}									\
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

char	*find_path(char **env, char *cmd);
void	redirect(Command *cmd);
void	redirect_pipe(Command *cmd, Pipes *pipes);
void	fd_storage(StorageAction action);
