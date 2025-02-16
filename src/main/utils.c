#include "minishell.h"

bool read_file(char *file, Context *ctx) {
	int fd = open(file, O_RDONLY);
	if (!fd) {
		perror("dinosh: failed to open file");
		return false;
	}

	char *line = get_next_line(fd);
	while (line) {
		ctx->input = line;
		handle_input(ctx);
		free(line);
		line = get_next_line(fd);
	}

	return true;
}

void init_basic_vars(Context *ctx) {
	modify_env(&ctx->env, "BLACK", "\033[90;1m", INTERN, -1);
	modify_env(&ctx->env, "RED", "\033[91;1m", INTERN, -1);
	modify_env(&ctx->env, "GREEN", "\033[92;1m", INTERN, -1);
	modify_env(&ctx->env, "YELLOW", "\033[93;1m", INTERN, -1);
	modify_env(&ctx->env, "BLUE", "\033[94;1m", INTERN, -1);
	modify_env(&ctx->env, "MAGENTA", "\033[95;1m", INTERN, -1);
	modify_env(&ctx->env, "CYAN", "\033[96;1m", INTERN, -1);
	modify_env(&ctx->env, "WHITE", "\033[97;1m", INTERN, -1);
	modify_env(&ctx->env, "RESET", "\033[0m", INTERN, -1);
	update_code_var(ctx);
}

void update_code_var(Context *ctx) {
	char *s = ft_itoa(ctx->code);
	modify_env(&ctx->env, "?", s, SPECIAL | INTERN, -1);
	free(s);
}

void read_token(Parser *head) {
	if (!head)
		return;
	printf("%d %d [%s] %d\n", head->id, head->expand_id, head->content, head->token);
	read_token(head->next);
}

void handle_input(Context *ctx) {
	Parser *data = tokenize(ctx->input);
	if (token_error(data)) {
		ctx->code = 2;
		free_list(data);
		return;
	}

	Command *cmd = parse(data);
	free_list(data);

	if (parse_error(cmd)) {
		ctx->code = 2;
		update_code_var(ctx);
		free_cmds(cmd);
		return;
	}

	execute(cmd, ctx);
	free_cmds(cmd);
}
