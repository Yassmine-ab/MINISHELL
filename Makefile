################################################################################
#                                     COLORS                                   #
################################################################################

DEFAULT			= \033[0m
RED				= \033[1;31m
GREEN			= \033[1;32m
YELLOW			= \033[1;33m
BLUE			= \033[1;34m
MAGENTA			= \033[1;35m
CYAN			= \033[1;36m
LIGHT_CYAN		= \033[1;96m
WHITE			= \033[1;37m

################################################################################
#                                     HEADER                                   #
################################################################################

define HEADER

	$(CYAN)╔═══════ $(WHITE)by yaabdall & besch$(CYAN) ══════════════════════════╗$(DEFAULT)
	$(CYAN)║                                                      ║$(DEFAULT)
	$(CYAN)║                              _           _ _         ║$(DEFAULT)
	$(CYAN)║              🎯      🎯     | |         | | |        ║$(DEFAULT)
	$(LIGHT_CYAN)║         ____  _ ____  _  ___| | _   ____| | |        ║$(DEFAULT)
	$(LIGHT_CYAN)║        |    \| |  _ \| |/___| || \ / _  | | |        ║$(DEFAULT)
	$(LIGHT_CYAN)║        | | | | | | | | |___ | | | ( (/ /| | |        ║$(DEFAULT)
	$(LIGHT_CYAN)║        |_|_|_|_|_| |_|_(___/|_| |_|\____|_|_|        ║$(DEFAULT)
	$(WHITE)║                                                      ║$(DEFAULT)
	$(WHITE)║                                                      ║$(DEFAULT)
	$(WHITE)╚══════════════════════════ $(CYAN)by besch & yaabdall$(WHITE) ═══════╝$(DEFAULT)

endef
export HEADER

################################################################################
#                                 PROGRESS BAR                                 #
################################################################################

define PROGRESS_BAR
    @TOTAL_STEPS=20; CURRENT_STEP=0; \
    while [ $$CURRENT_STEP -lt $$TOTAL_STEPS ]; do \
        CURRENT_STEP=$$(($$CURRENT_STEP + 1)); \
        echo -n "$(GREEN)▰$(DEFAULT)"; \
        sleep 0.01; \
    done; \
    echo " ✔️";
endef

################################################################################
#                                     CONFIG                                   #
################################################################################

NAME =			minishell
NAME_BONUS =	minishell_bonus
CC =			gcc
CFLAGS =		-Wall -Wextra -Werror
AR =			ar rcs
RM =			rm -f

################################################################################
#                                PROGRAM'S SOURCES                             #
################################################################################

# Directory paths
SRC =			./srcs/
INIT =			$(SRC)initialization/
LEXER =			$(SRC)lexer/
PARSER =		$(SRC)parser/
EXEC =			$(SRC)executor/
BUILTINS =		$(SRC)builtins/
ENV =			$(SRC)environment/
SIGNAL =		$(SRC)signal/
OBJ =			./objs/
MYLIB_DIR =		./lib/
INCLUDES =		./includes/ $(MYLIB_DIR) $(MYLIB_DIR)includes/

# Libraries
MYLIB =			-L $(MYLIB_DIR) -l:lib.a
READLINE =		-lreadline

# Source files
SRCX =			$(INIT)initialization.c \
				$(INIT)prompt.c \
				$(LEXER)lexer.c \
				$(LEXER)quotes.c \
				$(LEXER)parentheses.c \
				$(LEXER)operators.c \
				$(LEXER)words.c \
				$(PARSER)parser.c \
				$(PARSER)command.c \
				$(EXEC)exec.c \
				$(EXEC)exec_utils.c \
				$(EXEC)expander.c \
				$(EXEC)command.c \
				$(EXEC)command_utils.c \
				$(EXEC)pipeline.c \
				$(EXEC)redirections.c \
				$(ENV)environment.c \
				$(ENV)environment_utils.c \
				$(SIGNAL)signal.c \
				$(SIGNAL)signal_utils.c \
				$(BUILTINS)echo.c \
				$(BUILTINS)cd.c \
				$(BUILTINS)pwd.c \
				$(BUILTINS)env.c \
				$(BUILTINS)unset.c \
				$(BUILTINS)export.c \
				$(BUILTINS)export_utils.c \
				$(BUILTINS)exit.c \
				$(SRC)utils.c \

SRCS =			$(SRCX) \
				$(SRC)main.c

SRCS_BONUS =	$(SRCX) \
				$(SRC)main_bonus.c

# Converts source file paths to object file paths
OBJS =			$(patsubst $(SRC)%, $(OBJ)%, $(SRCS:.c=.o))
OBJS_BONUS =	$(patsubst $(SRC)%, $(OBJ)%, $(SRCS_BONUS:.c=.o))

################################################################################
#                                     RULES                                    #
################################################################################

# Rule for compiling source files into object files
$(OBJ)%.o:		$(SRC)%.c
				@mkdir -p $(dir $@)
				@$(CC) $(CFLAGS) -c $< -o $@ $(foreach dir,$(INCLUDES),-I$(dir))

# Rule for creating the executable
$(NAME):		$(OBJS)
				@make all --no-print-directory -C $(MYLIB_DIR)
				@echo -n "\n🔗 $(WHITE)Linking $(CYAN)$(NAME)$(DEFAULT) executable\t\t\t"
				@$(CC) $(CFLAGS) $(OBJS) $(MYLIB) $(READLINE) -o $(NAME)
				$(PROGRESS_BAR)
				@echo "\n$$HEADER"

# Default rule
all:			$(NAME)

# Bonus rule
bonus:			$(OBJS_BONUS)
				@make all --no-print-directory -C $(MYLIB_DIR)
				@echo -n "\n🔗 $(WHITE)Linking $(CYAN)$(NAME_BONUS)$(DEFAULT) executable\t\t"
				@$(CC) $(CFLAGS) $(OBJS_BONUS) $(MYLIB) $(READLINE) -o $(NAME_BONUS)
				$(PROGRESS_BAR)
				@echo "\n$$HEADER"

# Rule for cleaning up object files
clean:
				@make clean --no-print-directory -C $(MYLIB_DIR)
				@echo -n "\n🧹 $(RED)Cleaning up$(DEFAULT) project object files\t\t"
				@$(RM) -r $(OBJ)
				$(PROGRESS_BAR)
				@echo ""

# Full clean rule (objects files, executable and libraries)
fclean:
				@make fclean --no-print-directory -C $(MYLIB_DIR)
				@echo -n "\n🧹 $(RED)Cleaning up$(DEFAULT) $(CYAN)$(NAME)$(DEFAULT) object files\t\t"
				@$(RM) -r $(OBJ)
				$(PROGRESS_BAR)
				@echo -n "\n🗑️  $(RED)Deleting $(CYAN)$(NAME)$(DEFAULT) executable\t\t"
				@$(RM) $(NAME) $(NAME_BONUS)
				$(PROGRESS_BAR)
				@echo ""

# Rebuild rule
re:				fclean all

# Rule to compile the program with debugging flags
debug:			$(OBJS)
				@make all --no-print-directory -C $(MYLIB_DIR)
				@echo -n "\n🔗 $(CYAN)Compiling in debug mode $(DEFAULT)\t\t\t"
				@$(CC) $(CFLAGS) $(OBJS) $(MYLIB) $(READLINE) -o $(NAME) -g3 -fsanitize=address
				$(PROGRESS_BAR)
				@echo "$$HEADER"

# Rule to display help
help:
				@echo "\n$(CYAN)all$(DEFAULT)		- Build the executable $(NAME)"
				@echo "$(CYAN)bonus$(DEFAULT)		- Build the executable $(NAME_BONUS)"
				@echo "$(CYAN)clean$(DEFAULT)		- Clean up object files"
				@echo "$(CYAN)fclean$(DEFAULT)		- Clean up all object files and executables"
				@echo "$(CYAN)re$(DEFAULT)		- Rebuild the entire project"
				@echo "$(CYAN)debug$(DEFAULT)		- Run the program with debugging flags -g3"
				@echo "$(CYAN)test$(DEFAULT)		- Run the automated test suite ($(NAME))"
				@echo "$(CYAN)test_bonus$(DEFAULT)	- Run the automated test suite ($(NAME_BONUS))"
				@echo "$(CYAN)test_all$(DEFAULT)	- Run the automated test suites\n"

# Rule to run the automated tests (requires python3 and a built binary)
test:			$(NAME)
				@echo "\n🧪 $(CYAN)Running the $(WHITE)mandatory$(CYAN) test suite$(DEFAULT)\n"
				@python3 tests/smoke_test.py $(NAME)

# Rule to run the automated tests against the bonus executable
test_bonus:		bonus
				@echo "\n🧪 $(CYAN)Running the $(WHITE)bonus$(CYAN) test suite$(DEFAULT)\n"
				@python3 tests/smoke_test.py $(NAME_BONUS)

# Rule to run every test suite
test_all:		test test_bonus

# Rule to ensure that these targets are always executed as intended, even if there are files with the same name
.PHONY:			all bonus clean fclean re debug help test test_bonus test_all
