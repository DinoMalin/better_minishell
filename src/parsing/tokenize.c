#include "minishell.h"

bool are_we_there_yet(Node *node, char *str, int index) {
	if (node->token == double_quotes && index > 1 && str[index - 1] == '"')
		return true;
	if (node->token == single_quotes && index > 1 && str[index - 1] == '\'')
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
	if (!ft_strncmp(str, "\"", 1))
		return double_quotes;
	if (!ft_strncmp(str, "'", 1))
		return single_quotes;
	if (!ft_strncmp(str, ">>", 2))
		return append;
	if (!ft_strncmp(str, "<<", 2))
		return heredoc;
	if (!ft_strncmp(str, ">", 1))
		return to;
	if (!ft_strncmp(str, "<", 1))
		return from;
	if (!ft_strncmp(str, "|", 1))
		return tpipe;
	return arg;
}

Node *get_arg(char *str) {
	int		size = 0;
	Node	*node = ft_calloc(1, sizeof(Node));

	node->token = check_token(str);
	if (is_operator(node->token)) {
		node->content = OPERATOR(node->token);
		return node;
	}
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

int skip_whitespace(char *str) {
	int res = 0;

	while (isspace(str[res])) {
		res++;
	}

	return res;
}

Node *tokenize(char *str) {
    int     size = 0;
    Node    *curr = NULL;
    Node    *head = NULL;
    Node    *new_node = NULL;
	int		index = 0;

    while (str[size]) {
		int skip = skip_whitespace(str+size);
		if (skip)
			index++;
		str += skip;
		if (!str[size])
			break;

		new_node = get_arg(str + size);
		new_node->index = index;
		if (is_operator(new_node->token))
			index++;
		size++;

		if (!head) {
			curr = new_node;
			head = curr;
		} else {
			curr->next = new_node;
			curr = curr->next;
		}

		size += ft_strlen(curr->content) - 1;
		clean_node(curr);
    }

    return head;
}

