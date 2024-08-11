#include "minishell.h"

void display_list(Node *list) {
	Node	*curr = list;
	int		count = 0;

	while (curr) {
		printf("n%d - %s - %p\n", count, curr->content, curr->next);
		curr = curr->next;
		count++;
	}
}

int main() {
	char str[] = "    dinosaure \"dinosaure\"    ";
	Node *list = parse_command(str);
	display_list(list);
	free_list(list);
}
