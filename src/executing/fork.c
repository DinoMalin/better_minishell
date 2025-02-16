#include "execute.h"

void exit_fork(Command *head, Context *ctx) {
	free_cmds(head);
	free_env(ctx->env);
	exit(ctx->code);
}

void fork_routine(Command *head, Command *cmd, Context *ctx, Pipes *pipes) {
	redirect_pipe(cmd, pipes);
	redirect(cmd);

	if (cmd->error) {
		ctx->code = 1;
		exit_fork(head, ctx);
	}

	if (IS_BUILTIN(cmd->type))
		builtin(cmd, ctx);
	else if (cmd->type == SUBSHELL) {
		Context subctx = {
			.input = cmd->av[0],
			.env = ctx->env,
			.exit = false
		};
		handle_input(&subctx);
		exit_fork(head, &subctx);
	} else if (cmd->type == BASIC) {
		char *path = find_path(ctx->env, cmd->av[0]);

		if (!path)
			ctx->code = 127;
		else {
			char **envp = get_envp(ctx->env);
			execve(path, cmd->av, get_envp(ctx->env));
			for (int i = 0; envp[i]; i++) {
				free(envp[i]);
			}
			free(envp);
			ctx->code = 126;
		}
	}

	exit_fork(head, ctx);
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
			if (head->error)
				ctx->code = 1;
			else
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
		if (!init_command(ctx, curr)) {
			curr = curr->next;
			continue;
		}

		if (IS_BUILTIN(curr->type) && !IS_PIPED(curr)) {
			curr->pid = 0;
			fd_storage(STORE);
			redirect(curr);
			if (!curr->error)
				builtin(curr, ctx);
			fd_storage(RESTORE);
		} else {
			create_fork(head, curr, ctx, &pipes);

			xclose(pipes.prev[0]);
			xclose(pipes.prev[1]);

			pipes.prev[0] = pipes.curr[0];
			pipes.prev[1] = pipes.curr[1];
		}
		CHECK_TRANSMISSION();
		UPDATE_VAR_DURABILITY(ctx->env);
		update_code_var(ctx);

		curr = curr->next;
	}

	xclose(pipes.prev[0]);
	xclose(pipes.prev[1]);
	wait_everything(wait, NULL, ctx);
	update_code_var(ctx);
}
