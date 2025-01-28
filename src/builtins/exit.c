#include "builtins.h"

#define BUILTIN_ERROR_CODE(msg, code)		\
	{										\
		dprintf(2, "dinosh: "msg"\n");		\
		cmd->exit_code = code;				\
		return;								\
	}

bool is_number(char *str) {
	if (!*str)
		return false;

	if (*str == '+' || *str == '-' || ft_isdigit(*str))
		str++;
	else
		return false;

	while (*str) {
		if (!ft_isdigit(*str))
			return false;
		str++;
	}

	return true;
}

void builtin_exit(Command *cmd, Context *ctx) {
	if (cmd->ac > 2)
		BUILTIN_ERROR("exit: too many args");
	if (cmd->ac == 2) {
		if (!is_number(cmd->av[1]))
			BUILTIN_ERROR_CODE("exit: numeric argument required", 2);
		cmd->exit_code = ft_atoi(cmd->av[1]);
	}

	ctx->exit = true;
}
