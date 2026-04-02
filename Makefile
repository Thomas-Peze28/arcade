##
## EPITECH PROJECT, 2026
## Makefile
## File description:
## arcade
##

# --- Couleurs ---
GREEN       =   \033[0;32m
BLUE        =   \033[0;34m
YELLOW      =   \033[1;33m
RESET       =   \033[0m

CXX         =   g++
CXXFLAGS    =   -Wextra -Wall -std=c++20 -fno-gnu-unique

# --- Configuration ---
NAME =   arcade

SRC  =   src/main.cpp \
		 src/Arcade.cpp \

OBJ  =   $(SRC:.cpp=.o)
INC  =   -I ./include

.SILENT:

all:    $(NAME)

# Compilation
$(NAME): $(OBJ)
	$(CXX) -o $(NAME) $(OBJ) $(CXXFLAGS) $(INC)
	@printf "$(GREEN)[OK]$(RESET) Binary '$(NAME)' compiled successfully.\n"

clean:
	rm -f $(OBJ)
	@printf "$(BLUE)[INFO]$(RESET) Object files removed.\n"

fclean: clean
	rm -f $(NAME)
	@printf "$(BLUE)[INFO]$(RESET) Binaries removed.\n"

re: fclean all
	@printf "$(YELLOW)[INFO]$(RESET) Project rebuilt successfully.\n"