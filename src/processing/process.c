#include "minishell.h"

Command *init_cmd() {
	Command *res = ft_calloc(1, sizeof(Command));

	res->av = ft_calloc(1, sizeof(char *));
	res->in = ft_calloc(1, sizeof(char *));
	res->out = ft_calloc(1, sizeof(char *));
	res->in_type = r_from;
	res->out_type = r_to;
	return res;
}

void check_redir(Command *cmd, Node **data) {
	Token type = (*data)->token;
	(*data) = (*data)->next;

	if (!(*data)) {
		cmd->error.type = empty_redir;
		return;
	}

	if (is_redir((*data)->token)) {
		cmd->error.type = redir_toward_redir;
		return;
	}

	if (type == from || type == heredoc) {
		cmd->in = clean_strsjoin(cmd->in, ft_strdup((*data)->content));
	} else if (type == to || type == append) {
		cmd->out = clean_strsjoin(cmd->out, ft_strdup((*data)->content));
	}

	if (type == from)
		cmd->in_type = r_from;
	else if (type == heredoc)
		cmd->in_type = r_heredoc;
	else if (type == to)
		cmd->out_type = r_to;
	else if (type == append)
		cmd->out_type = r_append;
}

void analyze_command(Command *cmd, Node **data, int *arg_index) {
	if (is_redir((*data)->token)) {
		check_redir(cmd, data);
		return;
	}

	if (*arg_index == 0)
		cmd->cmd = ft_strdup((*data)->content);
	else
		cmd->av = clean_strsjoin(cmd->av, ft_strdup((*data)->content));
	(*arg_index)++;
}

Command *process(Node *data) {
	int data_index = 0;
	int arg_index = 0; // count the args w/o redirs
	Command *head = NULL;
	Command *curr = NULL;
	Command *new = NULL;

	while (data) {
		if (data->token == t_pipe) {
			arg_index = 0;
			data_index = 0;
		} else if (data_index == 0) {
			new = init_cmd();
			if (!head)
				head = new;
			else
				curr->next = new;
			curr = new;
			analyze_command(curr, &data, &arg_index);
			data_index++;
		} else {
			analyze_command(curr, &data, &arg_index);
			data_index++;
		}

		if (data)
			data = data->next;
	}
	
	return head;
}
