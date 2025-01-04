#include "minishell.h"

void free_av(char **av) {
 	int i = 0;
	while (av[i]) {
		free(av[i]);
		i++;
	}
	free(av);
}

void free_cmd(Command *cmd) {
	free_av(cmd->av);
	free_av(cmd->in);
	free_av(cmd->out);
	xfree(cmd->cmd);
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

char **strsjoin(char **origin, char *str) {
	int len = 0;

	while (origin[len]) {
		len++;
	}
	
	char **res = malloc((len + 2) * sizeof(char *));
	res[len + 1] = NULL;
	res[len] = str;
	while (len) {
		res[len - 1] = origin[len - 1];
		len--;
	}

	return res;
}

char **clean_strsjoin(char **origin, char *to_join) {
	char **res = strsjoin(origin, to_join);
	free(origin);
	return res;
}

char *clean_join(char *origin, const char *to_join) {
	char *res = ft_strjoin(origin, to_join);
	free(origin);
	return res;
}
