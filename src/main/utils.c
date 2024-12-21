#include "minishell.h"

char *clean_join(char *origin, const char *to_join) {
	char *res = ft_strjoin(origin, to_join);
	free(origin);
	return res;
}

void free_node(Node *node) {
	free(node->content);
	free(node);
}

void free_list(Node *list) {
	Node	*head = list;

	while (head) {
		Node *next = head->next;
		free_node(head);
		head = next;
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
