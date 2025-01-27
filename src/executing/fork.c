#include "execute.h"


void exit_fork(int exit_code, Command *cmd, Context *ctx) {
	free_cmds(cmd);
	free_av(ctx->env);
	exit(exit_code);
}

void fork_routine(Command *head, Command *cmd, Context *ctx, Pipes *pipes) {
	redirect(cmd);
	redirect_pipe(cmd, pipes);

	if (IS_BUILTIN(cmd->type))
		builtin(cmd, ctx);
	else if (cmd->type == SUBSHELL) {
		Context subctx = {
			.input = cmd->cmd,
			.env = ctx->env,
			.exit = false
		};
		handle_input(&subctx); // todo: create copy of env
	} else if (cmd->type == BASIC) {
		char *path = find_path(ctx->env, cmd->cmd);

		if (path)
			execve(path, cmd->av, ctx->env);
	}

	exit_fork(cmd->exit_code, head, ctx);
}

void create_fork(Command *head, Command *cmd, Context *ctx, Pipes *pipes) {
	pid_t pid = fork();

	if (pid == -1)
		perror("dinosh: fork");
	else if (IS_CHILD(pid))
		fork_routine(head, cmd, ctx, pipes);
	else
		cmd->pid = pid;
}

void wait_everything(Command *head, Command *until, Context *ctx) {
	int exit_status;

	while (head) {
		if (head->pid == -1)
			break;
		if (head->pid) {
			waitpid(head->pid, &exit_status, 0);
			head->pid = 0;
			ctx->code = WEXITSTATUS(exit_status);
		} else {
			ctx->code = head->exit_code;
		}
		if (head == until)
			break;
		head = head->next;
	}
}

void execute(Command *head, Context *ctx) {
	Command *curr = head;
	Command *wait = head;
	Pipes pipes = {
		.curr = {-1, -1},
		.prev = {-1, -1}
	};

	while (curr) {
		DO_PIPE();
		expand(curr, ctx->env);

		if (IS_BUILTIN(curr->type) && !IS_PIPED(curr)) {
			fd_storage(STORE);
			redirect(curr);
			builtin(curr, ctx);
			fd_storage(RESTORE);
		} else {
			create_fork(head, curr, ctx, &pipes);

			xclose(pipes.prev[0]);
			xclose(pipes.prev[1]);

			pipes.prev[0] = pipes.curr[0];
			pipes.prev[1] = pipes.curr[1];
		}

		CHECK_AND_OR();
		curr = curr->next;
	}

	xclose(pipes.prev[0]);
	xclose(pipes.prev[1]);
	wait_everything(wait, NULL, ctx);
}
