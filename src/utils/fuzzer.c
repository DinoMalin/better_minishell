#include "../executing/execute.h"
#include "tests.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
	char *input = malloc(size+1);
	ft_memcpy(input, data, size);
	input[size] = 0;

	Context ctx = {
		.env = NULL,
	};
	Parser *parsing_data = tokenize(input);

	if (has_token_errors(parsing_data)) {
		free_list(parsing_data);
		free(input);
		return 0;
	}

	Command *cmd = parse(parsing_data);
	free_list(parsing_data);
	if (parse_error(cmd)) {
		free_cmds(cmd, false);
		free(input);
		return 0;
	}

	Command *curr = cmd;
	while (curr) {
		init_command(&ctx, curr);
		curr = curr->next;
	}

	free_cmds(cmd, false);
	free_env(ctx.env);
	free(input);
	return 0;
}
