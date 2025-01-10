#include "minishell.h"

void handle_prompt(Prompt *prompt) {
	Node *data = parse(prompt->prompt, prompt->env);
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

	execute(cmd, prompt);
	free_cmds(cmd);
}

int main(int ac, char **av, char **envp) {
	(void)ac;
	(void)av;

	char **new_env = copy_env(envp);
	tests_parsing(envp);

	char *str = "export TEST=dinomalin";

	Prompt prompt;
	prompt.prompt = str;
	prompt.env = new_env;

	handle_prompt(&prompt);

	for (int i = 0; prompt.env[i]; i++) {
		printf("%s\n", prompt.env[i]);
	}
	free_av(prompt.env);
}
