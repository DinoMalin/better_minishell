#include "builtins.h"

void export(Command *cmd, Context *ctx) {
	if (cmd->ac < 2)
		BUILTIN_ERROR("export: not enough args");

	for (int i = 0; cmd->av[i]; i++) {
		char *content = ft_strchr(cmd->av[i], '=');
		if (content) {
			char *var = ft_substr(cmd->av[i], 0, content - cmd->av[i]);
			if (!var_is_valid(var)) {
				free(var);
				BUILTIN_ERROR("export: not a valid identifier");
			}
			modify_env(&ctx->env, var, content + 1, INTERN);
			free(var);
		}
	}
}
