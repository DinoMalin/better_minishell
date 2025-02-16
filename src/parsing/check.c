#include "parsing.h"

void check_type(Command *cmd, Token token) {
	char *name = cmd->args->content;

	if (token == t_subshell)
		cmd->type = SUBSHELL;
	else if (!ft_strcmp("echo", name))
		cmd->type = ECHO;
	else if (!ft_strcmp("cd", name))
		cmd->type = CD;
	else if (!ft_strcmp("pwd", name))
		cmd->type = PWD;
	else if (!ft_strcmp("export", name))
		cmd->type = EXPORT;
	else if (!ft_strcmp("unset", name))
		cmd->type = UNSET;
	else if (!ft_strcmp("env", name))
		cmd->type = ENV;
	else if (!ft_strcmp("exit", name))
		cmd->type = EXIT;
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
