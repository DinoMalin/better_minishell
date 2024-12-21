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
