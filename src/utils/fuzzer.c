#include "minishell.h"
#include "tests.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
	char *input = malloc(size+1);
	ft_memcpy(input, data, size);
	input[size] = 0;

	Node *parsing_data = tokenize(input);

	if (has_parsing_errors(parsing_data)) {
		free_list(parsing_data);
		free(input);
		return 0;
	}

	Command *cmd = process(parsing_data);

	free_list(parsing_data);
	free_cmds(cmd);
	free(input);

	return 0;
}
