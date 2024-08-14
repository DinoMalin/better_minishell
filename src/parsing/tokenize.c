#include "minishell.h"

bool are_we_there_yet(Node *node, char *str, int index) {
	if (node->token == double_quotes && str[index] == '"' && index != 0)
		return true;
	if (node->token == single_quotes && str[index] == '\'' && index != 0)
		return true;
	if (node->token != single_quotes && node->token != double_quotes && end_arg(str[index]))
		return true;
	if (!str[index]) {
		if (node->token == double_quotes)
			node->error.type = double_quote;
		if (node->token == single_quotes)
			node->error.type = single_quote;
		return true;
	}
	return false;
}

Token check_token(char *str) {
	if (str[0] == '"')
		return double_quotes;
	if (str[0] == '\'')
		return single_quotes;
	if (!ft_strcmp(str, ">"))
		return to;
	if (!ft_strcmp(str, "<"))
		return from;
	if (!ft_strcmp(str, "|"))
		return tpipe;
	return arg;
}

Node *get_arg(char *str) {
	int		size = 0;
	Node	*node = ft_calloc(1, sizeof(Node));

	node->token = check_token(str);
	while (!are_we_there_yet(node, str, size)) {
		size++;
	}

	node->content = ft_substr(str, 0, size);
	return node;
}

void clean_node(Node *node) {
	if (node->token == single_quotes) {
		char *new_content = ft_strtrim(node->content, "'");
		free(node->content);
		node->content = new_content;

	} else if (node->token == double_quotes) {
		char *new_content = ft_strtrim(node->content, "\"");
		free(node->content);
		node->content = new_content;
	}
}

Node *tokenize(char *str) {
    int     size = 0;
    Node    *curr = NULL;
    Node    *head = NULL;
    Node    *new_node = NULL;

    while (str[size]) {
        while (str[size] == ' ')
            size++;
		if (!str[size])
			break;
		new_node = get_arg(str + size);
		size++;
		if (!head) {
			curr = new_node;
			head = curr;
		} else {
			curr->next = new_node;
			curr = curr->next;
		}
		size += ft_strlen(curr->content);
		clean_node(curr);
    }

    return head;
}

