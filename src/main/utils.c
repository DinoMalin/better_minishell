#include "minishell.h"

char *clean_join(char *origin, const char *to_join) {
	char *res = ft_strjoin(origin, to_join);
	free(origin);
	return res;
}

