#include "builtins.h"

#define SKIP_VAR(s)										\
	{													\
		char *e = ft_strchr(env[i], '=');				\
		if (e && !ft_strncmp(env[i], s, e - env[i]))	\
			continue;									\
	}

void env(Command *cmd, char **env) {
	if (cmd->ac > 1)
		BUILTIN_ERROR("env: too many args");

	for (int i = 0; env[i]; i++) {
		SKIP_VAR("?");
		printf("%s\n", env[i]);
	}
}
