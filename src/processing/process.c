#include "process.h"

void add_arg(Command *cmd, Node *new) {
	Node *cpy = ft_calloc(1, sizeof(Node));
	cpy->content = ft_strdup(new->content);
	cpy->token = new->token;
	cpy->index = new->index;

	if (cmd->args) {
		Node *last = cmd->args;
		while (last->next) {
			last = last->next;
		}
		last->next = cpy;
	} else {
		cmd->args = cpy;
	}
}

Command *init_cmd(Transmission from) {
	Command *res = ft_calloc(1, sizeof(Command));

	res->av = ft_calloc(1, sizeof(char *));
	res->redirs = ft_calloc(1, sizeof(t_redir));
	res->from = from;
	res->pid = -1;
	return res;
}

void analyze_command(Command *cmd, Node **data, int *arg_index) {
	if (is_redir((*data)->token)) {
		check_redir(cmd, data);
		return;
	}

	if (*arg_index == 0) {
		cmd->cmd = ft_strdup((*data)->content);
		check_type(cmd, (*data)->token);
	}

	add_arg(cmd, *data);
	(*arg_index)++;
}

Command *process(Node *data) {
	int data_index = 0;
	int arg_index = 0; // count the args w/o redirs
	Error error = no_error;
	Transmission from = ANY;

	Command *head = NULL;
	Command *curr = NULL;

	while (data) {
		Node *next = data->next;
		PROCESS_TRANSMISSION(t_pipe, PIPE);
		PROCESS_TRANSMISSION(t_and, AND);
		PROCESS_TRANSMISSION(t_or, OR);
		if (data_index == 0) {
			Command *new = init_cmd(from);
			ADD_COMMAND(head, new, curr);
			curr = new;
		}
		if (data->token != t_pipe) {
			analyze_command(curr, &data, &arg_index);
			data_index++;
		}
		if (data)
			data = next;
	}
	
	init_ac(head);
	return head;
}
