#include "minishell.h"

extern int g_exit_status;

void sig_handler(int sig) {
	if (sig == SIGINT) {
		rl_replace_line("", 0);
		printf("\n");
		rl_on_new_line();
		rl_redisplay();
		g_exit_status = 130;
	}
}

int main(int ac, char **av, char **envp) {
	(void)ac;
	(void)av;
	tests_parsing(envp);

	signal(SIGINT, sig_handler);
	signal(SIGQUIT, SIG_IGN);

	Context ctx = {
		.input = NULL,
		.env = copy_env(envp),
		.exit = false,
	};
	ctx.env = modify_env(ctx.env, "?", "0");

	do {
		ctx.input = readline("dinosh> ");
		if (!ctx.input)
			break;
		if (g_exit_status == 130)
			UPDATE_CODE_VAR();
	
		add_history(ctx.input);
		handle_input(&ctx);
		UPDATE_CODE_VAR();

		free(ctx.input);
	} while (!ctx.exit);

	free_av(ctx.env);
}
