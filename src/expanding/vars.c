#include "expand.h"

static char *ft_getenv_alloc(char **envp, char *target) {
	char *res = ft_getenv(envp, target);
	if (!res)
		return ft_strdup("");

	res = ft_strdup(res);
	for (int i = 0; res[i]; i++) {
		if (res[i] == '*')
			res[i] *= -1;
	}
	return res;
}

int get_variable(char *str) {
	ADD_SPECIAL_VAR("?");

	int size;
	for (size = 0; str[size]; size++) {
		if (!	((str[size] >= 'A' && str[size] <= 'Z')
			||	(str[size] >= '0' && str[size] <= '9')
			||	str[size] == '_'))
			break;
	}
	return size;
}

char *extract(char *dest, char *src, int size) {
	char *sub = ft_substr(src, 0, size);
	dest = clean_join(dest, sub);
	free(sub);

	return dest;
}

char *expand_vars(char *str, char **envp) {
	int not_var = 0;
	char *res = ft_strdup("");

	while (*str) {
		if (*str == '$') {
			res = extract(res, str - not_var, not_var);

			int var_size = get_variable(str+1);
			char *var = ft_substr(str+1, 0, var_size);
			char *expanded_var = ft_getenv_alloc(envp, var);
			res = clean_join(res, expanded_var);

			free(var);
			free(expanded_var);
			str += var_size + 1;
			not_var = 0;
		} else {
			not_var++;
			str++;
		}
	}

	return extract(res, str - not_var, not_var);
}
