#include "minishell.h"

void handle_input(Context *ctx) {
	Node *data = tokenize(ctx->input);
	if (parsing_error(data)) {
		free_list(data);
		return;
	}

	Command *cmd = process(data);
	free_list(data);

	if (process_error(cmd)) {
		free_cmds(cmd);
		return;
	}

	execute(cmd, ctx);
	free_cmds(cmd);
}
