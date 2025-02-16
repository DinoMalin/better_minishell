#include "builtins.h"

#define CHDIR(x)						\
	{									\
		if (chdir(x) == -1) {			\
			free(oldpwd);				\
			BUILTIN_PERROR("cd");		\
		}								\
	}

void update_pwd(char *old, Context *ctx) {
	char *new = getcwd(NULL, 0);

	modify_env(&ctx->env, "PWD", new, 0, -1);
	modify_env(&ctx->env, "OLDPWD", old, 0, -1);

	free(new);
	free(old);
}

void cd(Command *cmd, Context *ctx) {
	if (cmd->ac > 2)
		BUILTIN_ERROR("cd: too many args");

	char *oldpwd = getcwd(NULL, 0);
	if (cmd->ac < 2) {
		char *home = ft_getenv(ctx->env, "HOME");
		if (!home) {
			free(oldpwd);
			BUILTIN_ERROR("cd: HOME not set");
		}
		CHDIR(home);
		update_pwd(oldpwd, ctx);
		return;
	}
	if (!ft_strcmp(cmd->av[1], "-")) {
		CHDIR(ft_getenv(ctx->env, "OLDPWD"));
	} else
		CHDIR(cmd->av[1]);

	update_pwd(oldpwd, ctx);
}
