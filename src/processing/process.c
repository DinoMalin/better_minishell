#include "minishell.h"

Command *init_cmd(char *cmd) {
	Command *res = ft_calloc(1, sizeof(Command));

	res->cmd = ft_strdup(cmd);
	res->av = ft_calloc(1, sizeof(char *));
	res->in_type = r_from;
	res->out_type = r_to;
	return res;
}

void check_redir(Command *cmd, Node **data) {
	Token type = (*data)->token;
	(*data) = (*data)->next;

	if (type == from) {
		cmd->in = ft_strdup((*data)->content);
		cmd->in_type = r_from;
	} else if (type == heredoc) {
		cmd->in = ft_strdup((*data)->content);
		cmd->in_type = r_heredoc;
	} else if (type == to) {
		cmd->out = ft_strdup((*data)->content);
		cmd->out_type = r_to;
	} else if (type == append) {
		cmd->out = ft_strdup((*data)->content);
		cmd->out_type = r_append;
	}
}

void analyze_command(Command *cmd, Node **data) {
	if (is_redir((*data)->token)) {
		check_redir(cmd, data);
		return;
	}

	cmd->av = clean_strsjoin(cmd->av, (*data)->content);
}

Command *process(Node *data) {
	int arg_index = 0;
	Command *head = NULL;
	Command *curr = NULL;
	Command *new = NULL;

	while (data) {
		if (data->token == t_pipe) {
			arg_index = 0;
		} else if (arg_index == 0) {
			new = init_cmd(data->content);
			if (!head)
				head = new;
			else
				curr->next = new;
			curr = new;
			arg_index++;
		} else {
			analyze_command(curr, &data);
			arg_index++;
		}

		data = data->next;
	}
	
	return head;
}