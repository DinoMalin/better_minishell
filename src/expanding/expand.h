#include "minishell.h"
#include <dirent.h>

#define CAN_EXPAND(x) ((x->token == t_double_quotes || x->token == t_word) && ft_strchr(curr->content, '$'))
#define CAN_WILDCARD(x) (curr->token == t_word && ft_strchr(curr->content, '*'))

#define IS_STAR(x) (x == '*' * -1)
#define EQUAL(x, e) (x && e && (x == e || (IS_STAR(x) && e == '*')))
#define NOT_EQUAL(x, e) (x != e && !(IS_STAR(x) && e == '*') && x != '*')

#define ADD_SPECIAL_VAR(var)			\
{										\
	int len = ft_strlen(var);			\
	if (!ft_strncmp(var, str, len))		\
		return len;						\
}

char	*expand_vars(char *str, char **envp);
Node	*expand_wildcard(Node *el, int max);
