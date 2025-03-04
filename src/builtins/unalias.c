#include "builtins.h"

void find_unalias(Command *cmd, char *name, Context *ctx) {
	Alias *temp;
	Alias *current = ctx->alias;

	while (current) {
		if (!ft_strcmp(name, current->name)) {
			if (current == ctx->alias)
				ctx->alias = current->next;
			else {
				if (current->next) {
					free(current->name);
					free(current->value);
					current->name = current->next->name;
					current->value = current->next->value;
					temp = current->next;
					current->next = current->next->next;
					free(temp);
				} else {
					temp->next = NULL;
					free_alias(current);
				}
				return;
			}
			free_alias(current);
			return;
		}
		temp = current;
		current = current->next;
	}
	BUILTIN_ERROR("unalias: %s: not found", name);
}

void help_unalias() {
	printf("unalias: unalias [-a] name [name ...]\n \
    Remove each NAME from the list of defined aliases.\n \
    \n \
    Options:\n \
      -a        remove all alias definitions\n \
    \n \
    Return success unless a NAME is not an existing alias.\n");
}

void unalias(Command *cmd, Context *ctx) {
	if (cmd->ac < 2) {
		BUILTIN_ERROR("unalias: usage: unalias [-a] name [name ...]");
	}

	if (cmd->av[1][0] == '-') {
		if (ft_strcmp(cmd->av[1], "-a") && ft_strcmp(cmd->av[1], "--help"))
			BUILTIN_ERROR("unalias: %s: invalid option\n \
unalias: usage: unalias [-a] name [name ...]", cmd->av[1]);
	}

	if (!ft_strcmp(cmd->av[1], "-a")) {
		unalias_all(ctx);
		return;
	}

	if (!ft_strcmp(cmd->av[1], "--help")) {
		help_unalias();
		return;
	}

	for (uint i = 1; cmd->av[i]; i++)
		find_unalias(cmd, cmd->av[i], ctx);
}
