#include "minishell.h"

bool are_we_there_yet(Node *node, char *str, int index) {
	if (node->token == double_quotes && str[index] == '"' && index != 0)
		return true;
	if (node->token == single_quotes && str[index] == '\'' && index != 0)
		return true;
	if (str[index] == ' ')
		return true;
	if (node->token == double_quotes && str[index] == '\'')
		node->token = error;
	else if (node->token == single_quotes && str[index] == '"')
		node->token = error;
	if (!str[index]) {
		if (node->token == double_quotes)
			node->error.type = double_quote;
		if (node->token == single_quotes)
			node->error.type = single_quote;
		return true;
	}
	if (str[index] == ' ')
		return true;
	return false;
}

Token check_token(char *str) {
	if (str[0] == '"')
		return double_quotes;
	if (str[0] == '\'')
		return single_quotes;
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

Node *parse_command(char *str) {
	int		size = 0;
	Node	*head = NULL;
	Node	*head_address = NULL;

	while (str[size] == ' ')
		size++;
	head = get_arg(str + size);
	head_address = head;
	while (str[size]) {
		size += ft_strlen(head->content);
		while (str[size] == ' ')
			size++;
		if (str[size]) {
			head->next = get_arg(str + size);
			size++;
		}
		head = head->next;
	}

	return head_address;
}

void display_list(Node *list) {
	Node	*head = list;
	int		count = 0;

	while (head) {
		printf("n%d - %s\n", count, head->content);
		head = head->next;
		count++;
	}
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

int main() {
	char str[] = "    dinosaure \"dinosaure\"    ";
	Node *list = parse_command(str);
	display_list(list);
	free_list(list);
}
