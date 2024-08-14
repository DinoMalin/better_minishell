#include "minishell.h"

char *clean_join(char *origin, const char *to_join) {
	char *tmp = origin;
	char *res = ft_strjoin(origin, to_join);
	free(tmp);
	return res;
}

void free_list(Node *list) {
	Node	*head = list;

	while (head) {
		Node *next = head->next;
		free(head->content);
		free(head);
		head = next;
	}
}
