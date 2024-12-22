#include "minishell.h"

void free_cmd(Command *cmd) {
	int i = 0;

	xfree(cmd->cmd);
	while (cmd->av[i]) {
		free(cmd->av[i]);
		i++;
	}
	free(cmd->av);
	xfree(cmd->in);
	xfree(cmd->out);
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
