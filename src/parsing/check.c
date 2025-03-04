#include "parsing.h"

Type get_builtin(char *name) {
	BUILTIN("echo", ECHO);
	BUILTIN("cd", CD);
	BUILTIN("pwd", PWD);
	BUILTIN("export", EXPORT);
	BUILTIN("unset", UNSET);
	BUILTIN("env", ENV);
	BUILTIN("exit", EXIT);
	BUILTIN("set", SET);
	BUILTIN("type", TYPE);
	BUILTIN("jobs", JOBS);
	BUILTIN("fg", FG);
	BUILTIN("bg", BG);
	BUILTIN("unalias", UNALIAS);
	return BASIC;
}

void check_type(Command *cmd, Token token) {
	char *name = cmd->args->content;

	if (token == t_subshell)
		cmd->type = SUBSHELL;
	if (token == t_control_group)
		cmd->type = CONTROL_GROUP;
	Type builtin = get_builtin(name);
	if (builtin)
		cmd->type = builtin;
}

void check_redir_errors(Command *cmd, Parser **data) {
	Parser *file = (*data)->next;

	if (!file) {
		cmd->error = empty_redir;
		return;
	}
	if (!CAN_REDIR(file->token)) {
		cmd->error = unexpected_token;
		return;
	}
}
