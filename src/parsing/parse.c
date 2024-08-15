#include "minishell.h"

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

	return res_head;
}
