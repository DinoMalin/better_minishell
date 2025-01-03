#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

#define STRINGS_DIFFER(x, y)	((!x && y) || (x && !y) \
								|| (x && y && ft_strcmp(x, y)))

#define FAIL(log)					\
{									\
	ft_fprintf(2, log "\n");		\
	return 0;						\
}

#define AV(...) (char*[]){__VA_ARGS__, NULL}
#define EMPTY_AV (char*[]){NULL}
#define COMMAND(cmd, av, in, out, in_type, out_type) \
	(Command){cmd, av, in, out, in_type, out_type, {0,0}, NULL}
