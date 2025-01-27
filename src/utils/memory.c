#include "minishell.h"

void free_av(char **av) {
 	int i = 0;
	while (av[i]) {
		free(av[i]);
		i++;
	}
	free(av);
}

void free_redir(t_redir *redir) {
	int i = 0;
	while (redir[i].file) {
		free(redir[i].file);
		i++;
	}
	free(redir);
}

void free_cmd(Command *cmd) {
	xfree(cmd->cmd);
	free_av(cmd->av);
	free_redir(cmd->redirs);
	free_list(cmd->args);
	free(cmd);
}

void free_cmds(Command *list) {
	while (list) {
		Command *next = list->next;
		free_cmd(list);
		list = next;
	}
}

void free_node(Node *node) {
	free(node->content);
	free(node);
}

void free_list(Node *list) {
	while (list) {
		Node *next = list->next;
		free_node(list);
		list = next;
	}
}
