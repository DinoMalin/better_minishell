#include "minishell.h"

#define BUILTIN_ERROR(msg)					\
	{										\
		dprintf(2, "dinosh: "msg"\n");		\
		cmd->exit_code = 1;					\
		return;								\
	}


#define BUILTIN_PERROR(msg)				\
	{									\
		perror("dinosh: "msg);			\
		cmd->exit_code = 1;				\
		return;							\
	}

void	echo(Command *cmd);
void	cd(Command *cmd, Context *ctx);
void	pwd(Command *cmd);
void	export(Command *cmd, Context *ctx);
void	unset(Command *cmd, Context *ctx);
void	env(Command *cmd, Env *env);
void	builtin_exit(Command *cmd, Context *ctx);
