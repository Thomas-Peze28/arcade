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
CXXFLAGS    =   -Wextra -Wall -std=c++20 -fno-gnu-unique -fPIC
CPPFLAGS    =   -I ./include/

# --- Core Configuration ---
NAME        =   arcade
SRC         =   src/Arcade.cpp
OBJ         =   $(SRC:.cpp=.o)

# --- Graphicals Configuration ---
LIB_DIR     =   ./lib/

NCURSES_SRC =   src/display/NcursesDisplay.cpp
NCURSES_OBJ =   $(NCURSES_SRC:.cpp=.o)
NCURSES_LIB =   $(LIB_DIR)arcade_ncurses.so

SFML_SRC    =   src/display/SFMLDisplay.cpp
SFML_OBJ    =   $(SFML_SRC:.cpp=.o)
SFML_LIB    =   $(LIB_DIR)arcade_sfml.so
SFML_FLAGS  =   -lsfml-graphics -lsfml-window -lsfml-system

GRAPHICALS  =   $(NCURSES_LIB) $(SFML_LIB)


# --- Games Configuration ---
SNAKE_SRC = src/games/SnakeGame.cpp
SNAKE_OBJ = $(SNAKE_SRC:.cpp=.o)
SNAKE_LIB = $(LIB_DIR)arcade_snake.so

GAMES     = $(SNAKE_LIB)

$(SNAKE_LIB): $(SNAKE_OBJ)
	@mkdir -p $(LIB_DIR)
	$(CXX) -shared -o $@ $^
	@printf "$(GREEN)[OK]$(RESET) Game '$@' compiled successfully.\n"

.SILENT:

all:    core games graphicals

core: $(NAME)

games: $(GAMES)

graphicals: $(GRAPHICALS)

# Compilation Core
$(NAME): $(OBJ)
	$(CXX) -o $(NAME) $(OBJ) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS)
	@printf "$(GREEN)[OK]$(RESET) Binary '$(NAME)' compiled successfully.\n"

# Compilation Ncurses
$(NCURSES_LIB): $(NCURSES_OBJ)
	@mkdir -p $(LIB_DIR)
	$(CXX) -shared -o $@ $^ -lncurses
	@printf "$(GREEN)[OK]$(RESET) Library '$@' compiled successfully.\n"

# Compilation SFML
$(SFML_LIB): $(SFML_OBJ)
	@mkdir -p $(LIB_DIR)
	$(CXX) -shared -o $@ $^ $(SFML_FLAGS)
	@printf "$(GREEN)[OK]$(RESET) Library '$@' compiled successfully.\n"

clean:
	rm -f $(OBJ) $(NCURSES_OBJ) $(SFML_OBJ) $(SNAKE_OBJ)
	@printf "$(BLUE)[INFO]$(RESET) Object files removed.\n"

fclean: clean
	rm -f $(NAME)
	rm -f $(GRAPHICALS)
	rm -f $(GAMES)
	@printf "$(BLUE)[INFO]$(RESET) Binaries and libraries removed.\n"

re: fclean all
	@printf "$(YELLOW)[INFO]$(RESET) Project rebuilt successfully.\n"

.PHONY: all core games graphicals clean fclean re