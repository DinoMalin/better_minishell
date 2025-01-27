#include "expand.h"

static bool its_a_match(char *str, char *entry) {
	int i = 0;
	int j = 0;

	while (str[i] && entry[j]) {
		if (str[i] == '*') {
			while (str[i] == '*') {
				i++;
			}
			while (entry[j] && entry[j] != str[i]) {
				j++;
			}
			if (!str[i])
				return true;
		}
		while (EQUAL(str[i], entry[j])) {
			i++;
			j++;
		}
		if (NOT_EQUAL(str[i], entry[j]))
			return false;
	}
	while (str[i] == '*') {
		i++;
	}
	if (str[i])
		return false;
	return true;
}

char **get_entries(char *str) {
	DIR *dir = opendir(".");
	struct dirent *entry = NULL;
	char **res = ft_calloc(1, sizeof(char*));

	if (!dir) {
		perror("dinosh: opendir");
		return NULL;
	}

	while ((entry = readdir(dir))) {
		if (!ft_strcmp(entry->d_name, ".") || !ft_strcmp(entry->d_name, ".."))
			continue;
		if (its_a_match(str, entry->d_name)) {
			if (str[0] != '.' && entry->d_name[0] == '.')
				continue;
			res = clean_strsjoin(res, ft_strdup(entry->d_name));
		}
	}

	closedir(dir);
	return res;
}

Node *expand_wildcard(Node *el, int max) {
	char **entries = get_entries(el->content);
	free(el->content);

	if (!entries[0])
		el->content = ft_strdup("");
	else {
		el->content = entries[0];
		Node *next = el->next;

		for (int i = 1; entries[i]; i++) {
			Node *new = ft_calloc(1, sizeof(Node));

			new->content = entries[i];
			new->index = max + i;

			el->next = new;
			el = el->next;
		}

		el->next = next;
	}

	free(entries);
	return el;
}

