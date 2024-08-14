#include "libft.h"
#include <stdbool.h>
#include <stdio.h>

#define end_arg(x) (x == ' ' || x == '|')
#define can_expand(x) (x->token == double_quotes || x->token == arg)

typedef enum {
	arg,
	to,
	from,
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
Node	*parse_nodes(char *str, char **envp);

/* ====== UTILS ====== */
char	*clean_join(char *origin, const char *to_join);
void	free_list(Node *list);

