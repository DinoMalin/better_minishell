NAME = minishell

LIBFT = lib/libft.a

CC = cc

CFLAGS =	-Wall -Wextra -Werror -g \
			-Iinclude

LFLAGS =	-Llib -lft

SRC_DIR = src/
OBJ_DIR = obj/
MAIN	= main/main
FILES	= main/utils main/tests \
		parsing/tokenize parsing/expand parsing/parse \
		processing/process
FUZZER_FILES := $(FILES)
FILES += $(MAIN)

SRCS = $(addprefix $(SRC_DIR), $(addsuffix .c, $(FILES)))
OBJS = $(addprefix $(OBJ_DIR), $(addsuffix .o, $(FILES)))


FUZZER_SRCS = $(addprefix $(SRC_DIR), $(addsuffix .c, $(FUZZER_FILES)))
FUZZER_OBJS = $(addprefix $(OBJ_DIR), $(addsuffix .o, $(FUZZER_FILES)))

SH=bash

all: $(NAME)

bonus: all

clean :
	@rm -rf $(OBJ_DIR)

fclean : clean
	@rm -rf $(NAME)
	@make fclean -C lib

re: fclean all

$(NAME): $(OBJS) $(LIBFT)
	@echo -e "\e[32m✔ Compilating sources files...\e[37m"
	@$(CC) -o $@ $(OBJS) $(LFLAGS)
	@echo -e "\e[32m✔ Executable created.\e[37m"

fuzzer: $(FUZZER_OBJS) $(LIBFT)
	@echo -e "\e[32m✔ Compilating sources files...\e[37m"
	clang -o $@ $(FUZZER_OBJS) $(LFLAGS) -fsanitize=address,fuzzer
	@echo -e "\e[32m✔ Executable created.\e[37m"
	

$(LIBFT):
	@make -C lib

obj/%.o: src/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

.PHONY: all clean fclean re bonus
