#include "libft.h"
#include <stdbool.h>
#include <stdio.h>

typedef enum {
	arg,
	to,
	from,
	error,
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
