#include "libft.h"
#include <stdbool.h>
#include <stdio.h>

#define end_arg(x) (x == ' ' || x == '|' || x == '<' || x == '>')
#define can_expand(x) (x->token == double_quotes || x->token == arg)
#define is_operator(x) (x == tpipe || x == append || x == heredoc \
					|| x == to || x == from)
#define OPERATOR(x) x == tpipe ?	ft_strdup("|") : \
					x == append ?	ft_strdup(">>") : \
					x == heredoc ?	ft_strdup("<<") : \
					x == to ?		ft_strdup(">") : \
									ft_strdup("<")

typedef enum {
	arg,
	to,
	append,
	from,
	heredoc,
	single_quotes,
	double_quotes,
	tpipe,
} Token;

typedef enum {
	single_quote,
	double_quote,
} Error_Type;

typedef struct {
	int			index;
	Error_Type	type;
} Error;

typedef struct Node {
	char		*content;
	Token		token;
	struct Node	*next;
	Error		error;
} Node;


/* ====== PARSING ====== */
Node	*tokenize(char *str);
char	*expand(char *str, char **envp);
Node	*parse(char *str, char **envp);

/* ====== UTILS ====== */
char	*clean_join(char *origin, const char *to_join);
void	free_list(Node *list);

