NAME = minishell

LIBFT = lib/libft.a

CC = cc

CFLAGS =	-Wall -Wextra -Werror -g \
			-Iinclude

LFLAGS =	-Llib -lft -lreadline

SRC_DIR = src/
OBJ_DIR = obj/
MAIN	= main/main
FILES	= main/errors main/input main/signals							\
		utils/memory utils/utils										\
		parsing/tokenize parsing/expand parsing/parse parsing/wildcard	\
		processing/process processing/check								\
		builtins/env_utils builtins/choose								\
		builtins/echo builtins/cd										\
		builtins/pwd builtins/export									\
		builtins/unset builtins/env builtins/exit						\
		executing/fork executing/path executing/redirect

FUZZER_FILES := $(FILES) utils/tests
TESTS_FILES := $(FILES) utils/tests utils/main_tests
FILES += $(MAIN)

SRCS = $(addprefix $(SRC_DIR), $(addsuffix .c, $(FILES)))
OBJS = $(addprefix $(OBJ_DIR), $(addsuffix .o, $(FILES)))


FUZZER_SRCS = $(addprefix $(SRC_DIR), $(addsuffix .c, $(FUZZER_FILES)))
FUZZER_OBJS = $(addprefix $(OBJ_DIR), $(addsuffix .o, $(FUZZER_FILES)))

TESTS_SRCS = $(addprefix $(SRC_DIR), $(addsuffix .c, $(TESTS_FILES)))
TESTS_OBJS = $(addprefix $(OBJ_DIR), $(addsuffix .o, $(TESTS_FILES)))

SHELL=bash

all: $(NAME)

bonus: all

clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -rf $(NAME)
	@make fclean -C lib

re: fclean all

$(NAME): $(OBJS) $(LIBFT)
	@echo -e "\e[32m✔ Compilating sources files...\e[37m"
	@$(CC) -o $@ $(OBJS) $(LFLAGS)
	@echo -e "\e[32m✔ Executable created.\e[37m"

fuzzer: CFLAGS += -fsanitize=address,fuzzer
fuzzer: $(FUZZER_OBJS) $(LIBFT)
	@echo -e "\e[32m✔ Compilating sources files...\e[37m"
	@clang -o $@ $(FUZZER_OBJS) $(LFLAGS) $(CFLAGS)
	@echo -e "\e[32m✔ Fuzzer created.\e[37m"

tests: $(TESTS_OBJS) $(LIBFT)
	@echo -e "\e[32m✔ Compilating sources files...\e[37m"
	@clang -o $@ $(TESTS_OBJS) $(LFLAGS) $(CFLAGS)
	@echo -e "\e[32m✔ Tests created.\e[37m"

$(LIBFT):
	@make -C lib

obj/%.o: src/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

.PHONY: all clean fclean re bonus
