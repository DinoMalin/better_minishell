#include "minishell.h"
#include "tests.h"

int comp_av(char **av1, char **av2) {
	int i;
	for (i = 0; av1[i] && av2[i]; i++) {
		if (ft_strcmp(av1[i], av2[i]))
			return false;
	}
	if (av1[i] || av2[i])
		return false;
	return true;
}

int comp_redir(t_redir *r1, t_redir *r2) {
	int i;
	for (i = 0; r1[i].file && r2[i].file; i++) {
		if (ft_strcmp(r1[i].file, r2[i].file))
			return false;
		if (r1[i].type != r2[i].type)
			return false;
	}
	if (r1[i].file || r2[i].file)
		return false;
	return true;
}

int comp_cmd(Command *cmd1, Command *cmd2) {
	if (STRINGS_DIFFER(cmd1->cmd, cmd2->cmd))
		FAIL("wrong cmd->cmd");
	if (!comp_av(cmd1->av, cmd2->av))
		FAIL("wrong cmd->av");
	if (!comp_redir(cmd1->redirs, cmd2->redirs))
		FAIL("wrong cmd->redirs");
	if (cmd1->type != cmd2->type)
		FAIL("wrong cmd->type");
	if (cmd1->from != cmd2->from)
		FAIL("wrong cmd->from");
	if (cmd1->to != cmd2->to)
		FAIL("wrong cmd->to");

	return 1;
}

int comp(char *input, Command *expected, char **envp) {
	Node *data = parse(input, envp);
	if (has_parsing_errors(data)) {
		free_list(data);
		return 0;
	}

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

void tests_parsing(char **envp) {
	// Basic
	assert(comp("echo test < in | cat > out", (Command[]) {
		CMD("echo", AV("echo", "test"), REDIRS(REDIR("in", r_from)), ECHO, ANY, PIPE),
		CMD("cat", AV("cat"), REDIRS(REDIR("out", r_to)), BASIC, PIPE, ANY)
	}, envp));
	assert(comp("echo\t\r   test\t\t    <\t\tin\t|   cat    >  out", (Command[]) {
		CMD("echo", AV("echo", "test"), REDIRS(REDIR("in", r_from)), ECHO, ANY, PIPE),
		CMD("cat", AV("cat"), REDIRS(REDIR("out", r_to)), BASIC, PIPE, ANY)
	}, envp));

	// Command
	assert(comp("echo test", (Command[]) {
		CMD("echo", AV("echo", "test"), NO_REDIR, ECHO, ANY, ANY)
	}, envp));
	assert(comp("echo ", (Command[]) {
		CMD("echo", AV("echo"), NO_REDIR, ECHO, ANY, ANY)
	}, envp));
	assert(comp("echo", (Command[]) {
		CMD("echo", AV("echo"), NO_REDIR, ECHO, ANY, ANY)
	}, envp));
	assert(comp("", (Command[]) {
		CMD(NULL, NO_AV, NO_REDIR, BASIC, ANY, ANY)
	}, envp));
	assert(comp("> out", (Command[]) {
		CMD(NULL, NO_AV, REDIRS(REDIR("out", r_to)), BASIC, ANY, ANY)
	}, envp));
	assert(comp(">out", (Command[]) {
		CMD(NULL, NO_AV, REDIRS(REDIR("out", r_to)), BASIC, ANY, ANY)
	}, envp));
	assert(comp("> out echo", (Command[]) {
		CMD("echo", AV("echo"), REDIRS(REDIR("out", r_to)), ECHO, ANY, ANY)
	}, envp));
	assert(comp(">out echo", (Command[]) {
		CMD("echo", AV("echo"), REDIRS(REDIR("out", r_to)), ECHO, ANY, ANY)
	}, envp));

	// Redir
	assert(comp("echo test", (Command[]) {
		CMD("echo", AV("echo", "test"), NO_REDIR, ECHO, ANY, ANY)
	}, envp));

	assert(comp("echo test < in", (Command[]) {
		CMD("echo", AV("echo", "test"), REDIRS(REDIR("in", r_from)), ECHO, ANY, ANY)
	}, envp));
	assert(comp("echo test <in", (Command[]) {
		CMD("echo", AV("echo", "test"), REDIRS(REDIR("in", r_from)), ECHO, ANY, ANY)
	}, envp));

	assert(comp("echo test > out", (Command[]) {
		CMD("echo", AV("echo", "test"), REDIRS(REDIR("out", r_to)), ECHO, ANY, ANY)
	}, envp));
	assert(comp("echo test >out", (Command[]) {
		CMD("echo", AV("echo", "test"), REDIRS(REDIR("out", r_to)), ECHO, ANY, ANY)
	}, envp));

	assert(comp("echo test >> out", (Command[]) {
		CMD("echo", AV("echo", "test"), REDIRS(REDIR("out", r_append)), ECHO, ANY, ANY)
	}, envp));
	assert(comp("echo test >>out", (Command[]) {
		CMD("echo", AV("echo", "test"), REDIRS(REDIR("out", r_append)), ECHO, ANY, ANY)
	}, envp));

	assert(comp("echo test << here", (Command[]) {
		CMD("echo", AV("echo", "test"), REDIRS(REDIR("here", r_heredoc)), ECHO, ANY, ANY)
	}, envp));
	assert(comp("echo test <<here", (Command[]) {
		CMD("echo", AV("echo", "test"), REDIRS(REDIR("here", r_heredoc)), ECHO, ANY, ANY)
	}, envp));

	// Multiple redirs
	assert(comp("echo test > out >> append", (Command[]) {
		CMD("echo", AV("echo", "test"), REDIRS(
			REDIR("out", r_to),
			REDIR("append", r_append)
		), ECHO, ANY, ANY)
	}, envp));
	assert(comp("echo test < in << here", (Command[]) {
		CMD("echo", AV("echo", "test"), REDIRS(
			REDIR("in", r_from),
			REDIR("here", r_heredoc)
		), ECHO, ANY, ANY)
	}, envp));
	assert(comp("echo test < in << here > out >> append", (Command[]) {
		CMD("echo", AV("echo", "test"), REDIRS(
			REDIR("in", r_from),
			REDIR("here", r_heredoc),
			REDIR("out", r_to),
			REDIR("append", r_append)
		), ECHO, ANY, ANY)
	}, envp));

	assert(comp("echo test > ", (Command[]) {
		CMD("echo", AV("echo", "test"), NO_REDIR, ECHO, ANY, ANY)
	}, envp));
	assert(comp("echo test >", (Command[]) {
		CMD("echo", AV("echo", "test"), NO_REDIR, ECHO, ANY, ANY)
	}, envp));
	assert(comp("echo test < ", (Command[]) {
		CMD("echo", AV("echo", "test"), NO_REDIR, ECHO, ANY, ANY)
	}, envp));
	assert(comp("echo test <", (Command[]) {
		CMD("echo", AV("echo", "test"), NO_REDIR, ECHO, ANY, ANY)
	}, envp));
	assert(comp("echo test >> ", (Command[]) {
		CMD("echo", AV("echo", "test"), NO_REDIR, ECHO, ANY, ANY)
	}, envp));
	assert(comp("echo test >>", (Command[]) {
		CMD("echo", AV("echo", "test"), NO_REDIR, ECHO, ANY, ANY)
	}, envp));
	assert(comp("echo test << ", (Command[]) {
		CMD("echo", AV("echo", "test"), NO_REDIR, ECHO, ANY, ANY)
	}, envp));
	assert(comp("echo test <<", (Command[]) {
		CMD("echo", AV("echo", "test"), NO_REDIR, ECHO, ANY, ANY)
	}, envp));
	assert(comp(">>>", (Command[]) {
		CMD(NULL, NO_AV, NO_REDIR, BASIC, ANY, ANY),
	}, envp));

	// Env var
	assert(comp("echo $USER", (Command[]) {
		CMD("echo", AV("echo", xgetenv("USER")), NO_REDIR, ECHO, ANY, ANY)
	}, envp));
	assert(comp("echo $LS_COLORS", (Command[]) {
		CMD("echo", AV("echo", xgetenv("LS_COLORS")), NO_REDIR, ECHO, ANY, ANY)
	}, envp));
	assert(comp("echo $UNKNOWN", (Command[]) { // must be non existant
		CMD("echo", AV("echo", xgetenv("UNKNOWN")), NO_REDIR, ECHO, ANY, ANY)
	}, envp));
	assert(comp("echo '$USER'", (Command[]) {
		CMD("echo", AV("echo", "$USER"), NO_REDIR, ECHO, ANY, ANY)
	}, envp));
	assert(comp("echo \"$USER\"", (Command[]) {
		CMD("echo", AV("echo", xgetenv("USER")), NO_REDIR, ECHO, ANY, ANY)
	}, envp));
	assert(comp("echo \"$XDG_\"", (Command[]) { // must be non existant
		CMD("echo", AV("echo", xgetenv("XDG_")), NO_REDIR, ECHO, ANY, ANY)
	}, envp));

	// Quotes
	assert(comp("echo \"test\"", (Command[]) {
		CMD("echo", AV("echo", "test"), NO_REDIR, ECHO, ANY, ANY)
	}, envp));
	assert(comp("echo \"test\"test", (Command[]) {
		CMD("echo", AV("echo", "testtest"), NO_REDIR, ECHO, ANY, ANY)
	}, envp));
	assert(comp("echo 'test'test", (Command[]) {
		CMD("echo", AV("echo", "testtest"), NO_REDIR, ECHO, ANY, ANY)
	}, envp));
	assert(comp("echo '\"test\"'", (Command[]) {
		CMD("echo", AV("echo", "\"test\""), NO_REDIR, ECHO, ANY, ANY)
	}, envp));
	assert(comp("echo \"'test'\"", (Command[]) {
		CMD("echo", AV("echo", "'test'"), NO_REDIR, ECHO, ANY, ANY)
	}, envp));
	assert(comp("echo \"test1 test2 test3\"", (Command[]) {
		CMD("echo", AV("echo", "test1 test2 test3"), NO_REDIR, ECHO, ANY, ANY)
	}, envp));
	assert(comp("echo \"test1 test2 test3 \"test4", (Command[]) {
		CMD("echo", AV("echo", "test1 test2 test3 test4"), NO_REDIR, ECHO, ANY, ANY)
	}, envp));

	// Pipes
	assert(comp("echo test | cat out", (Command[]) {
		CMD("echo", AV("echo", "test"), NO_REDIR, ECHO, ANY, PIPE),
		CMD("cat", AV("cat", "out"), NO_REDIR, BASIC, PIPE, ANY)
	}, envp));
	assert(comp("echo test | ", (Command[]) {
		CMD("echo", AV("echo", "test"), NO_REDIR, ECHO, ANY, PIPE),
	}, envp));
	assert(comp("echo test |", (Command[]) {
		CMD("echo", AV("echo", "test"), NO_REDIR, ECHO, ANY, PIPE),
	}, envp));

	// Builtins
	assert(comp("cat test", (Command[]) {
		CMD("cat", AV("cat", "test"), NO_REDIR, BASIC, ANY, ANY),
	}, envp));
	assert(comp("echo test", (Command[]) {
		CMD("echo", AV("echo", "test"), NO_REDIR, ECHO, ANY, ANY),
	}, envp));
	assert(comp("cd test", (Command[]) {
		CMD("cd", AV("cd", "test"), NO_REDIR, CD, ANY, ANY),
	}, envp));
	assert(comp("pwd test", (Command[]) {
		CMD("pwd", AV("pwd", "test"), NO_REDIR, PWD, ANY, ANY),
	}, envp));
	assert(comp("export test", (Command[]) {
		CMD("export", AV("export", "test"), NO_REDIR, EXPORT, ANY, ANY),
	}, envp));
	assert(comp("unset test", (Command[]) {
		CMD("unset", AV("unset", "test"), NO_REDIR, UNSET, ANY, ANY),
	}, envp));
	assert(comp("env test", (Command[]) {
		CMD("env", AV("env", "test"), NO_REDIR, ENV, ANY, ANY),
	}, envp));
	assert(comp("exit test", (Command[]) {
		CMD("exit", AV("exit", "test"), NO_REDIR, EXIT, ANY, ANY),
	}, envp));

	// Transmissions
	assert(comp("echo test | cat out | wc -l", (Command[]) {
		CMD("echo", AV("echo", "test"), NO_REDIR, ECHO, ANY, PIPE),
		CMD("cat", AV("cat", "out"), NO_REDIR, BASIC, PIPE, PIPE),
		CMD("wc", AV("wc", "-l"), NO_REDIR, BASIC, PIPE, ANY),
	}, envp));
	assert(comp("echo test && cat out || wc -l", (Command[]) {
		CMD("echo", AV("echo", "test"), NO_REDIR, ECHO, ANY, AND),
		CMD("cat", AV("cat", "out"), NO_REDIR, BASIC, AND, OR),
		CMD("wc", AV("wc", "-l"), NO_REDIR, BASIC, OR, ANY),
	}, envp));

	// Subshells
	assert(comp("(cd lib && echo test) && make", (Command[]) {
		CMD("cd lib && echo test", AV("cd lib && echo test"), NO_REDIR, SUBSHELL, ANY, AND),
		CMD("make", AV("make"), NO_REDIR, BASIC, AND, ANY),
	}, envp));
}


