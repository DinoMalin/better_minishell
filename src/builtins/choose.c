#include "builtins.h"

void builtin(Command *cmd, Context *ctx) {
	BUILTIN(ECHO, echo);
	BUILTIN(CD, cd);
	BUILTIN(PWD, pwd);
	BUILTIN(EXPORT, export);
	BUILTIN(UNSET, unset);
	BUILTIN(ENV, env);
	BUILTIN(EXIT, builtin_exit);
	BUILTIN(SET, set);
	BUILTIN(TYPE, type);
	BUILTIN(JOBS, jobs);
	BUILTIN(FG, fg);
	BUILTIN(BG, bg);
	BUILTIN(UNALIAS, unalias);
}
