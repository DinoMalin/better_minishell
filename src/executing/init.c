#include "execute.h"

void init_av(Command *cmd) {
	Parser *curr = cmd->args;

	while (curr) {
		Parser *next = curr->next;
		cmd->av = clean_strsjoin(cmd->av, ft_strdup(curr->content));
		curr = next;
		cmd->ac++;
	}
}

void add_redir(Command *cmd, Token type, char *name) {
	Redir r_type = 0;
	if (type == t_from)
		r_type = r_from;
	else if (type == t_heredoc)
		r_type = r_heredoc;
	else if (type == t_to)
		r_type = r_to;
	else if (type == t_append)
		r_type = r_append;

	t_redir redirection = (t_redir){ft_strdup(name), r_type, 0, 0};
	cmd->redirs = clean_redirjoin(cmd->redirs, redirection);
}

void add_redir_fd(Command *cmd, char *n, char *word, Token token) {
	bool should_close = !ft_strcmp(word, "-");
	if ((!is_number(n) || !is_number(word)) && !should_close) {
		dprintf(2, "dinosh: numeric argument required\n");
		cmd->error = numeric_argument;
		return;
	}
	int fd_n = ft_atoi(n);
	int fd_word = should_close ? -1 : ft_atoi(word);
	Redir r_type = token == t_to_fd ? r_to_fd : r_from_fd;
	t_redir redirection = (t_redir){ft_strdup(""), r_type, fd_n, fd_word};
	cmd->redirs = clean_redirjoin(cmd->redirs, redirection);
}

// I don't check if file exists because it's supposed to be checked in the parsing
void init_redirs(Command *cmd) {
	Parser *curr = cmd->args;
	Parser *prec = curr;

	while (curr) {
		if (IS_REDIR(curr->token)) {
			Parser *file = curr->next;
			if (file->next && IS_AMBIGUOUS(file)) {
				dprintf(2, "dinosh: ambiguous redirect\n");
				cmd->error = ambiguous_redirect;
			}
			if (curr->token == t_to_fd || curr->token == t_from_fd) {
				Parser *next = file->next;
				add_redir_fd(cmd, curr->content, file->content, curr->token);
				DELETE_ARG(cmd->args, curr, prec);
				DELETE_ARG(cmd->args, file, prec);
				curr = next;
			} else {
				Parser *next = file->next;
				add_redir(cmd, curr->token, file->content);
				DELETE_ARG(cmd->args, curr, prec);
				DELETE_ARG(cmd->args, file, prec);
				curr = next;
			}
			continue;
		}
		prec = curr;
		curr = curr->next;
	}
}

void fill_heredoc(Command *cmd) {
	Parser *curr = cmd->args;
	while (curr) {
		if (curr->token == t_heredoc) {
			unlink(HEREDOC_FILE);
			Parser *file = curr->next;
			merge_one_node(file);
			if (!heredoc(file->content)) {
				cmd->error = eheredoc;
				return;
			}
		}
		curr = curr->next;
	}
}

bool add_command(Context *ctx, Command *cmd) {
	Parser *curr = cmd->args;

	while (curr) {
		char *content = ft_strchr(curr->content, '=');
		if (!content)
			break;

		char *var = ft_substr(curr->content, 0, content - curr->content);
		if (!var_is_valid(var)) {
			free(var);
			dprintf(2, "dinosh: not a valid identifier\n");
			return false;
		}

		Parser *next = curr->next;
		modify_env(&ctx->env, var, content + 1, 0);
		DELETE_ARG(cmd->args, curr, cmd->args);
		free(var);
		curr = next;
	}
	return true;
}

bool init_command(Context *ctx, Command *cmd) {
	fill_heredoc(cmd);
	expand(cmd, ctx->env);
	init_redirs(cmd);
	if (command_error(cmd)) {
		cmd->pid = 0;
		return false;
	}

	merge(cmd->args);
	add_command(ctx, cmd);
	if (command_error(cmd)) {
		cmd->pid = 0;
		return false;
	}

	init_av(cmd);
	return true;
}
