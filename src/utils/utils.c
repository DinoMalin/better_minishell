#include "minishell.h"

bool is_number(char *str) {
	if (!*str)
		return false;

	if (*str == '+' || *str == '-' || ft_isdigit(*str))
		str++;
	else
		return false;

	while (*str) {
		if (!ft_isdigit(*str))
			return false;
		str++;
	}

	return true;
}

bool var_is_valid(char *name) {
	int i = 0;
	for (; name[i]; i++) {
		if (!(name[i] >= 'a' && name[i] <= 'z')
		 && !(name[i] >= 'A' && name[i] <= 'Z')
		 && !(name[i] >= '0' && name[i] <= '9' && i != 0)
		 && !(name[i] == '_')) {
			return false;
		}
	}
	if (!i)
		return false;
	return true;
}

