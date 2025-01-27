#include "tokenize.h"

bool skip_whitespace(char **str) {
	bool res = false;
	while (ft_isspace(**str)) {
		(*str)++;
		res = true;
	}
	return res;
}

char *until(char **str, char *sep) {
	char *end = ft_strnstr(*str, sep, strlen(*str));
	if (!end)
		return NULL;

	char *res = ft_substr(*str, 0, end - *str);
	*str = end + 1;

	return res;
}

char *operator(char **str, char *op) {
	*str += ft_strlen(op);
	return (ft_strdup(op));
}

char *parse_word(char **str) {
	char *end = *str;
	while (*end && !ft_strchr(WORD_END, *end)) {
		end++;
	}
	char *res = ft_substr(*str, 0, end - *str);
	*str = end;
	if (!ft_strlen(res)) {
		free(res);
		return NULL;
	}
	return res;
}

Node *tokenize(char *str) {
	int index = 0;
	Node *curr = NULL;
	Node *head = NULL;

	while (*str) {
		if (skip_whitespace(&str))
			index++;
		PARSE_TOKEN("\"", "\"", t_double_quotes);
		PARSE_TOKEN("'", "'", t_single_quotes);
		PARSE_TOKEN("(", ")", t_subshell);
		PARSE_OPERATOR(">>", t_append);
		PARSE_OPERATOR("<<", t_heredoc);
		PARSE_OPERATOR(">", t_to);
		PARSE_OPERATOR("<", t_from);
		PARSE_OPERATOR("&&", t_and);
		PARSE_OPERATOR("||", t_or);
		PARSE_OPERATOR("|", t_pipe);
		PARSE_WORD();
	}

	return head;
}
