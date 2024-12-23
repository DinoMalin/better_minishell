#include "minishell.h"

void merge(Node *head) {
	Node *next;
	Node *after_next;

	while (head && head->next) {
		next = head->next;
		while (next && head->index == next->index) {
			head->content = clean_join(head->content, next->content);

			after_next = next->next;
			head->next = after_next;
			free_node(next);
			next = after_next;
		}
		head = head->next;
	}
}

Node *parse(char *str, char **envp) {
	Node *res_head = tokenize(str);
	Node *curr = res_head;

	while (curr) {
		if (can_expand(curr)) {
			char *new_content = expand(curr->content, envp);
			free(curr->content);
			curr->content = new_content;
		}
		curr = curr->next;
	}

	if (res_head)
		merge(res_head);
	return res_head;
}
