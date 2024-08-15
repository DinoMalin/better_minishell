#include "minishell.h"

void display_token_type(Node *node) {
	if (node->token == arg)
		ft_printf("arg");
	else if (node->token == to)
		ft_printf("to");
	else if (node->token == from)
		ft_printf("from");
	else if (node->token == single_quotes)
		ft_printf("sq");
	else if (node->token == double_quotes)
		ft_printf("dq");
	else if (node->token == tpipe)
		ft_printf("tpipe");
}

void display_list(Node *list) {
	Node	*curr = list;
	int		count = 0;

	while (curr) {
		ft_printf("n%d: ", count); 
		display_token_type(curr);
		ft_printf("\t[%s]\n", curr->content);
		curr = curr->next;
		count++;
	}
}

int main(int ac, char **av, char **envp) {
	if (ac != 2) {
		ft_printf("Usage: %s <test>", av[0]);
		return 1;	
	}

	Node *nodes = parse_nodes(av[1], envp);
	display_list(nodes);
	free_list(nodes);
}
