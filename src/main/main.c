#include "minishell.h"

void display_token_type(Node *node) {
	if (node->token == arg)
		ft_printf("arg");
	else if (node->token == to)
		ft_printf("to");
	else if (node->token == from)
		ft_printf("from");
	else if (node->token == append)
		ft_printf("append");
	else if (node->token == heredoc)
		ft_printf("heredoc");
	else if (node->token == single_quotes)
		ft_printf("sq");
	else if (node->token == double_quotes)
		ft_printf("dq");
	else if (node->token == t_pipe)
		ft_printf("t_pipe");
}

void display_redir(Redir r) {
	if (r == r_to)
		ft_printf("to");
	else if (r == r_from)
		ft_printf("from");
	else if (r == r_append)
		ft_printf("append");
	else if (r == r_heredoc)
		ft_printf("heredoc");
}

void display_list(Node *list) {
	Node	*curr = list;
	int		count = 0;

	while (curr) {
		ft_printf("n%d: ", count); 
		display_token_type(curr);
		ft_printf("\t[%s]", curr->content);
		ft_printf("\t%d\n", curr->index);
		curr = curr->next;
		count++;
	}
}

void display_processed(Command *cmd) {
	while (cmd) {
		ft_printf("%s ", cmd->cmd);
		for (int i = 0; cmd->av[i]; i++) {
			ft_printf("%s ", cmd->av[i]);
		}
		ft_printf("\nin: %s", cmd->in);
		ft_printf("\nout: %s\n", cmd->out);
		display_redir(cmd->in_type);
		ft_printf(", ");
		display_redir(cmd->out_type);
		ft_printf("\n");
		ft_printf("\n");

		cmd = cmd->next;
	}
}

int main(int ac, char **av, char **envp) {
	if (ac != 2) {
		ft_printf("Usage: %s <test>", av[0]);
		return 1;	
	}

	Node *nodes = parse(av[1], envp);
	display_list(nodes);

	ft_printf("---------------\n");
	Command *cmd = process(nodes);
	display_processed(cmd);
	free_list(nodes);
	free_cmds(cmd);

}
