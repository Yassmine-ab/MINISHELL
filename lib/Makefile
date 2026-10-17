# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ VARIABLES ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

NAME =			lib.a
CC =			cc
FLAGS =			-Wall -Wextra -Werror

SRC =			$(wildcard *.c) $(wildcard garbage_collector/*.c)
OBJ =			$(SRC:.c=.o)
INCLUDES =		-I. -Iincludes


# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ RULES ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

%.o:			%.c
				@$(CC) $(FLAGS) $(INCLUDES) -c $< -o $@

$(NAME):		$(OBJ)
				@ar rcs $(NAME) $(OBJ)

all:			$(NAME)

clean:
				@rm -f $(OBJ)

fclean:			clean
				@rm -f $(NAME)

re:				fclean all

.PHONY:			all clean fclean re
