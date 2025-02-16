#include "builtins.h"

Env *create_env(char **env) {
	Env *head = NULL;
	Env *last = NULL;

	for (int i = 0; env[i]; i++) {
		Env *new = ft_calloc(1, sizeof(Env));

		int equal = ft_strchr(env[i], '=') - env[i];
		int len_value = ft_strlen(env[i]) - equal;
		new->var = ft_substr(env[i], 0, equal);
		new->value = ft_substr(env[i], equal + 1, len_value);
		new->durability = -1;

		if (!i) {
			head = new;
			last = new;
		} else {
			last->next = new;
			last = new;
		}
	}

	return head;
}

char *ft_getenv(Env *env, char *target) {
	while (env) {
		if (!ft_strcmp(target, env->var))
			return env->value;
		env = env->next;
	}
	return NULL;
}

void modify_env(Env **env, char *target, char *new_value, Special special, int dur) {
	Env *curr = *env;
	Env *last = curr;

	while (curr) {
		if (!ft_strcmp(target, curr->var)) {
			free(curr->value);
			curr->value = ft_strdup(new_value);
			curr->type = special;
			return;
		}
		last = curr;
		curr = curr->next;
	}

	Env *new = ft_calloc(1, sizeof(Env));
	new->var = ft_strdup(target);
	new->value = ft_strdup(new_value);
	new->type = special;
	new->durability = dur;

	if (!*env) {
		*env = new;
	} else {
		last->next = new;
	}
}

void delete_var(Env **env, char *target) {
	Env *curr = *env;
	Env *last = *env;

	while (curr) {
		if (!ft_strcmp(target, curr->var)) {
			if (curr == *env)
				*env = curr->next;
			else
				last->next = curr->next;

			free(curr->var);
			free(curr->value);
			free(curr);

			return;
		}
		last = curr;
		curr = curr->next;
	}
}

char **get_envp(Env *env) {
	int size = 0;
	Env *curr = env;
	while (curr) {
		if (!(curr->type & INTERN || curr->type & SPECIAL))
			size++;
		curr = curr->next;
	}

	char **res = malloc((size + 1) * sizeof(char *));
	res[size] = NULL;

	int i = 0;
	while (env) {
		if (!(env->type & INTERN || env->type & SPECIAL)) {
			char *new = ft_strdup(env->var);
			new = clean_join(new, "=");
			new = clean_join(new, env->value);
			res[i] = new;

			i++;
		}
		env = env->next;
	}
	return res;
}
