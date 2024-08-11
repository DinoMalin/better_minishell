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

int main(int ac, char **av, char **envp) {
	(void)ac;
	(void)av;
	
	char t1[] = "test $USER test";
	char t2[] = "$USER";
	char t3[] = "$USER-test";
	char t4[] = "test-$USER";
	char t5[] = "$DOESNOTEXIST test";
	char t6[] = "";
	
	char *str1 = expand(t1, envp);
	char *str2 = expand(t2, envp);
	char *str3 = expand(t3, envp);
	char *str4 = expand(t4, envp);
	char *str5 = expand(t5, envp);
	char *str6 = expand(t6, envp);
	ft_printf("%s => %s\n", t1, str1);
	ft_printf("%s => %s\n", t2, str2);
	ft_printf("%s => %s\n", t3, str3);
	ft_printf("%s => %s\n", t4, str4);
	ft_printf("%s => %s\n", t5, str5);
	ft_printf("%s => %s\n", t6, str6);
	free(str1);
	free(str2);
	free(str3);
	free(str4);
	free(str5);
	free(str6);
}
