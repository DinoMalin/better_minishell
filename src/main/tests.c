#include "minishell.h"
#include "tests.h"

int comp_cmd(Command *cmd1, Command *cmd2) {
	int i;

	if (STRINGS_DIFFER(cmd1->cmd, cmd2->cmd))
		FAIL("wrong cmd->cmd");

	for (i = 0; cmd1->av[i] && cmd2->av[i]; i++) {
		if (ft_strcmp(cmd1->av[i], cmd2->av[i]))
			FAIL("wrong cmd->av");
	}
	if (cmd1->av[i] || cmd2->av[i])
		FAIL("wrong cmd->av");

	if (STRINGS_DIFFER(cmd1->in, cmd2->in))
		FAIL("wrong cmd->in");
	if (STRINGS_DIFFER(cmd1->out, cmd2->out))
		FAIL("wrong cmd->out");

	if (cmd1->in_type != cmd2->in_type)
		FAIL("wrong cmd->in_type");
	if (cmd1->out_type != cmd2->out_type)
		FAIL("wrong cmd->out_type");

	return 1;
}

int comp(char *prompt, Command *expected, char **envp) {
	Node *data = parse(prompt, envp);
	Command *cmd = process(data);
	Command *curr = cmd;
	int i = 0;

	while (curr) {
		if (!comp_cmd(curr, expected+i)) {
			free_list(data);
			free_cmds(cmd);
			return 0;
		}
		i++;
		curr = curr->next;
	}

	free_list(data);
	free_cmds(cmd);
	return 1;
}

void tests(char **envp) {
	// Basic
	assert(comp("echo test < in | cat > out", (Command[]) {
		COMMAND("echo", AV("test", NULL), "in", NULL, r_from, r_to),
		COMMAND("cat", AV(NULL), NULL, "out", r_from, r_to)
	}, envp));
	assert(comp("echo\t\r   test\t\t    <\t\tin\t|   cat    >  out", (Command[]) {
		COMMAND("echo", AV("test", NULL), "in", NULL, r_from, r_to),
		COMMAND("cat", AV(NULL), NULL, "out", r_from, r_to)
	}, envp));

	// Command
	assert(comp("echo test", (Command[]) {
		COMMAND("echo", AV("test", NULL), NULL, NULL, r_from, r_to)
	}, envp));
	assert(comp("echo ", (Command[]) {
		COMMAND("echo", AV(NULL), NULL, NULL, r_from, r_to)
	}, envp));
	assert(comp("echo", (Command[]) {
		COMMAND("echo", AV(NULL), NULL, NULL, r_from, r_to)
	}, envp));
	assert(comp("", (Command[]) {
		COMMAND(NULL, AV(NULL), NULL, NULL, r_from, r_to)
	}, envp));
	assert(comp("> out", (Command[]) {
		COMMAND(NULL, AV(NULL), NULL, "out", r_from, r_to)
	}, envp));
	assert(comp(">out", (Command[]) {
		COMMAND(NULL, AV(NULL), NULL, "out", r_from, r_to)
	}, envp));
	assert(comp("> out echo", (Command[]) {
		COMMAND("echo", AV(NULL), NULL, "out", r_from, r_to)
	}, envp));
	assert(comp(">out echo", (Command[]) {
		COMMAND("echo", AV(NULL), NULL, "out", r_from, r_to)
	}, envp));

	// Redir
	assert(comp("echo test", (Command[]) {
		COMMAND("echo", AV("test", NULL), NULL, NULL, r_from, r_to)
	}, envp));

	assert(comp("echo test < in", (Command[]) {
		COMMAND("echo", AV("test", NULL), "in", NULL, r_from, r_to)
	}, envp));
	assert(comp("echo test <in", (Command[]) {
		COMMAND("echo", AV("test", NULL), "in", NULL, r_from, r_to)
	}, envp));

	assert(comp("echo test > out", (Command[]) {
		COMMAND("echo", AV("test", NULL), NULL, "out", r_from, r_to)
	}, envp));
	assert(comp("echo test >out", (Command[]) {
		COMMAND("echo", AV("test", NULL), NULL, "out", r_from, r_to)
	}, envp));

	assert(comp("echo test >> out", (Command[]) {
		COMMAND("echo", AV("test", NULL), NULL, "out", r_from, r_append)
	}, envp));
	assert(comp("echo test >>out", (Command[]) {
		COMMAND("echo", AV("test", NULL), NULL, "out", r_from, r_append)
	}, envp));

	assert(comp("echo test << here", (Command[]) {
		COMMAND("echo", AV("test", NULL), "here", NULL, r_heredoc, r_to)
	}, envp));
	assert(comp("echo test <<here", (Command[]) {
		COMMAND("echo", AV("test", NULL), "here", NULL, r_heredoc, r_to)
	}, envp));

	assert(comp("echo test > ", (Command[]) {
		COMMAND("echo", AV("test", NULL), NULL, NULL, r_from, r_to)
	}, envp));
	assert(comp("echo test >", (Command[]) {
		COMMAND("echo", AV("test", NULL), NULL, NULL, r_from, r_to)
	}, envp));
	assert(comp("echo test < ", (Command[]) {
		COMMAND("echo", AV("test", NULL), NULL, NULL, r_from, r_to)
	}, envp));
	assert(comp("echo test <", (Command[]) {
		COMMAND("echo", AV("test", NULL), NULL, NULL, r_from, r_to)
	}, envp));
	assert(comp("echo test >> ", (Command[]) {
		COMMAND("echo", AV("test", NULL), NULL, NULL, r_from, r_to)
	}, envp));
	assert(comp("echo test >>", (Command[]) {
		COMMAND("echo", AV("test", NULL), NULL, NULL, r_from, r_to)
	}, envp));
	assert(comp("echo test << ", (Command[]) {
		COMMAND("echo", AV("test", NULL), NULL, NULL, r_from, r_to)
	}, envp));
	assert(comp("echo test <<", (Command[]) {
		COMMAND("echo", AV("test", NULL), NULL, NULL, r_from, r_to)
	}, envp));

	// Env var
	assert(comp("echo $USER", (Command[]) {
		COMMAND("echo", AV(getenv("USER"), NULL), NULL, NULL, r_from, r_to)
	}, envp));
	assert(comp("echo $LS_COLORS", (Command[]) {
		COMMAND("echo", AV(getenv("LS_COLORS"), NULL), NULL, NULL, r_from, r_to)
	}, envp));
	assert(comp("echo $UNKNOWN", (Command[]) { // must be non existant
		COMMAND("echo", AV("", NULL), NULL, NULL, r_from, r_to)
	}, envp));
	assert(comp("echo '$USER'", (Command[]) {
		COMMAND("echo", AV("$USER", NULL), NULL, NULL, r_from, r_to)
	}, envp));
	assert(comp("echo \"$USER\"", (Command[]) {
		COMMAND("echo", AV(getenv("USER"), NULL), NULL, NULL, r_from, r_to)
	}, envp));

	// Quotes
	assert(comp("echo \"test\"", (Command[]) {
		COMMAND("echo", AV("test", NULL), NULL, NULL, r_from, r_to)
	}, envp));
	assert(comp("echo \"test\"test", (Command[]) {
		COMMAND("echo", AV("testtest", NULL), NULL, NULL, r_from, r_to)
	}, envp));
	assert(comp("echo 'test'test", (Command[]) {
		COMMAND("echo", AV("testtest", NULL), NULL, NULL, r_from, r_to)
	}, envp));
	assert(comp("echo '\"test\"'", (Command[]) {
		COMMAND("echo", AV("\"test\"", NULL), NULL, NULL, r_from, r_to)
	}, envp));
	assert(comp("echo \"'test'\"", (Command[]) {
		COMMAND("echo", AV("'test'", NULL), NULL, NULL, r_from, r_to)
	}, envp));
	assert(comp("echo \"test1 test2 test3\"", (Command[]) {
		COMMAND("echo", AV("test1 test2 test3", NULL), NULL, NULL, r_from, r_to)
	}, envp));
	assert(comp("echo \"test1 test2 test3 \"test4", (Command[]) {
		COMMAND("echo", AV("test1 test2 test3 test4", NULL), NULL, NULL, r_from, r_to)
	}, envp));

	// Pipes
	assert(comp("echo test | cat out", (Command[]) {
		COMMAND("echo", AV("test", NULL), NULL, NULL, r_from, r_to),
		COMMAND("cat", AV("out", NULL), NULL, NULL, r_from, r_to)
	}, envp));
	assert(comp("echo test | ", (Command[]) {
		COMMAND("echo", AV("test", NULL), NULL, NULL, r_from, r_to),
	}, envp));
	assert(comp("echo test |", (Command[]) {
		COMMAND("echo", AV("test", NULL), NULL, NULL, r_from, r_to),
	}, envp));
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
	char *input = malloc(size+1);
	ft_memcpy(input, data, size);
	input[size] = 0;

	Node *parsing_data = parse(input, (char*[]){NULL});
	Command *cmd = process(parsing_data);

	free_list(parsing_data);
	free_cmds(cmd);
	free(input);

	return 0;
}
