#include "builtins.h"

Alias *create_alias(char *name, char *value, void *next) {
	Alias *new = malloc(sizeof(Alias));
	new->name = name;
	new->value = value;
	new->next = next;
	return new;
}

char *is_alias(char *name, Alias *alias) {
	while (alias) {
		if (!ft_strcmp(name, alias->name))
			return alias->value;
		alias = alias->next;
	}
	return NULL;
}

void help_alias() {
	ft_putstr_fd("alias: alias [-p] [name[=value] ... ]\n \
    Define or display aliases.\n \
    \n \
    Without arguments, `alias' prints the list of aliases in the reusable\n \
    form `alias NAME=VALUE' on standard output.\n \
    \n \
    Otherwise, an alias is defined for each NAME whose VALUE is given.\n \
    A trailing space in VALUE causes the next word to be checked for\n \
    alias substitution when the alias is expanded.\n \
    \n \
    Options:\n \
      -p        print all defined aliases in a reusable format\n \
    \n \
    Exit Status:\n \
    alias returns true unless a NAME is supplied for which no alias has been\n \
    defined.\n", 1);
}

void find_alias(Command *cmd, char *name, Alias *alias) {
	for (Alias *i = alias; i; i = i->next) {
		if (!ft_strcmp(name, i->name)) {
			printf("alias %s=%s\n", i->name, i->value);
			return;
		}
	}
	BUILTIN_ERROR("alias: %s: not found", name);
}

void push_alias(char *name, char *value, Context *ctx) {
	char *empty = "\'\'";
	if (!value[0])
		value = empty;
	Alias *temp = ctx->alias;
	Alias *prev;
	
	while (temp) {
		if (!ft_strcmp(temp->name, name)) {
			free(temp->value);
			temp->value = ft_strdup(value);
			return;
		}
		prev = temp;
		temp = temp->next;
	}

	Alias *new = create_alias(ft_strdup(name), ft_strdup(value), NULL);
	if (ctx->alias)
		prev->next = new;
	else
		ctx->alias = new;
}

void print_alias(Alias *alias) {
	for (Alias *i = alias; i; i = i->next)
		printf("alias %s=%s\n", i->name, i->value);
}

void alias(Command *cmd, Context *ctx) {
	unsigned char parameter = 0;
	if (cmd->ac < 2) {
		print_alias(ctx->alias);
		return;
	}

	if (cmd->av[1][0] == '-' && ft_strcmp(cmd->av[1], "-p") 
			&& ft_strcmp(cmd->av[1], "--help"))
		BUILTIN_ERROR("alias: %s: invalid option\n\
alias: usage: alias [-p] [name[=value] ... ]", cmd->av[1]);

	if (!ft_strcmp(cmd->av[1], "-p")) {
		parameter = 1;
		print_alias(ctx->alias);
		return;
	}

	if (!ft_strcmp(cmd->av[1], "--help")) {
		parameter = 1;
		help_alias();
		return;
	}

	char *equal_ptr;
	for (uint i = 1 + parameter; cmd->av[i]; i++) {
		equal_ptr = ft_strchr(cmd->av[i], '=');
		if (!equal_ptr)
			find_alias(cmd, cmd->av[i], ctx->alias);
		else {
			*equal_ptr = '\0';
			push_alias(cmd->av[i], equal_ptr + 1, ctx);
		}
	}
}
