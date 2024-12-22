#include "libft.h"
#include <stdbool.h>
#include <stdio.h>

#define isspace(x) ((x >= '\t' && x <= '\r') || x == ' ')
#define end_arg(x) (x == ' ' || x == '|' || x == '<' || x == '>' || x == '\'' || x == '"')
#define can_expand(x) (x->token == double_quotes || x->token == arg)
#define is_operator(x) (x == t_pipe || x == append || x == heredoc \
					|| x == to || x == from)
#define is_redir(x) (x == append || x == heredoc || x == to || x == from)
#define OPERATOR(x) x == t_pipe ?	ft_strdup("|") : \
					x == append ?	ft_strdup(">>") : \
					x == heredoc ?	ft_strdup("<<") : \
					x == to ?		ft_strdup(">") : \
									ft_strdup("<")
#define xfree(x) if (x) {free(x);}

typedef enum {
	arg,
	to,
	append,
	from,
	heredoc,
	single_quotes,
	double_quotes,
	t_pipe,
} Token;

typedef enum {
	single_quote,
	double_quote,
} Error_Type;

typedef struct {
	int			index;
	Error_Type	type;
} Error;

/* === Parsing linked list ===*/
typedef struct Node {
	char		*content;
	Token		token;
	struct Node	*next;
	Error		error;
	int			index;	// isn't used to count : the index will sometime be
} Node;					// non-consecutives. It's used to know if two args
						// must be merged

typedef enum {
	r_to,
	r_append,
	r_from,
	r_heredoc,
} Redir;

/* === Executing linked list ===*/
typedef struct Command {
	char			*cmd;
	char			**av;
	char			*in;
	char			*out;
	struct Command	*next;
	Redir			in_type;
	Redir			out_type;
} Command;


/* ====== PARSING ====== */
Node	*tokenize(char *str);
char	*expand(char *str, char **envp);
Node	*parse(char *str, char **envp);

/* ====== PROCESSING ====== */
Command	*process(Node *data);

/* ====== UTILS ====== */
char	*clean_join(char *origin, const char *to_join);
void	free_cmds(Command *list);
void	free_node(Node *node);
void	free_list(Node *list);
char	**strsjoin(char **origin, char *str);
char	**clean_strsjoin(char **origin, char *to_join);

/* ====== TESTS ====== */
void	tests(char **envp);
