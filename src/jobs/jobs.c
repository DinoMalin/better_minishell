#include "minishell.h"

void print_job(Job *job) {
	ft_printf("[%d] %c %s ",
		job->index,
		job->is_current ? '+' : '-',
		job->state == RUNNING ? "Running" : "Stopped"
	); // ft_printf because flush problems in builtins

	for (int i = 0; job->cmd->av[i]; i++) {
		ft_printf("%s", job->cmd->av[i]);
		if (job->cmd->av[i+1])
			ft_printf(" ");
	}

	ft_printf("\n");
}

void print_pid(Job *job) {
	printf("[%d] %d\n", job->index, job->cmd->pid);
}

void add_job(Context *ctx, Command *cmd) {
	Job *new = malloc(sizeof(Job));

	int index = 1;
	for (Job *job=ctx->jobs; job; job=job->next) {
		if (job->index != index) {
			index = job->index;
			break;
		}
		index++;
	}

	for (Job *job=ctx->jobs; job; job=job->next) {
		if (job->is_current)
			job->is_current = false;
	}

	new->index = index;
	new->is_current = true;
	new->state = RUNNING;
	new->cmd = cmd;
	new->next = NULL;

	if (!ctx->jobs)
		ctx->jobs = new;
	else {
		Job *job = ctx->jobs;
		while (job->next && job->index != new->index - 1) {
			job = job->next;
		}
		Job *next = job->next;
		job->next = new;
		new->next = next;
	}
	print_pid(new);
}
