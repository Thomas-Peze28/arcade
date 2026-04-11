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

# --- Ncurses Configuration ---
NCURSES_SRC =   src/display/Ncurses/NcursesDisplay.cpp
NCURSES_OBJ =   $(NCURSES_SRC:.cpp=.o)
NCURSES_LIB =   $(LIB_DIR)arcade_ncurses.so

# --- SFML Configuration ---
SFML_SRC    =   src/display/SFML/SFMLDisplay.cpp
SFML_OBJ    =   $(SFML_SRC:.cpp=.o)
SFML_LIB    =   $(LIB_DIR)arcade_sfml.so
SFML_FLAGS  =   -lsfml-graphics -lsfml-window -lsfml-system

# --- SDL2 Configuration ---
SDL2_SRC    =   src/display/SDL2/SDL2Display.cpp
SDL2_OBJ    =   $(SDL2_SRC:.cpp=.o)
SDL2_LIB    =   $(LIB_DIR)arcade_sdl2.so
SDL2_FLAGS  =   -lSDL2 -lSDL2_ttf -lSDL2_image

# --- Allegro5 Configuration ---
ALLEGRO5_SRC    =   src/display/Allegro5/Allegro5Display.cpp
ALLEGRO5_OBJ    =   $(ALLEGRO5_SRC:.cpp=.o)
ALLEGRO5_LIB    =   $(LIB_DIR)arcade_allegro5.so
ALLEGRO5_FLAGS  =   -lallegro -lallegro_primitives -lallegro_font -lallegro_ttf

GRAPHICALS  =   $(NCURSES_LIB) $(SFML_LIB) $(SDL2_LIB) $(ALLEGRO5_LIB)

# --- Games Configuration ---

# --- Snake Game Configuration ---
SNAKE_SRC   =   src/games/Snake/Snake.cpp
SNAKE_OBJ   =   $(SNAKE_SRC:.cpp=.o)
SNAKE_LIB   =   $(LIB_DIR)arcade_snake.so

# --- Minesweeper Game Configuration ---
MINESWEEPER_SRC =   src/games/Minesweeper/Minesweeper.cpp
MINESWEEPER_OBJ =   $(MINESWEEPER_SRC:.cpp=.o)
MINESWEEPER_LIB =   $(LIB_DIR)arcade_minesweeper.so

GAMES       =   $(SNAKE_LIB) $(MINESWEEPER_LIB)

.SILENT:

all:    core games graphicals

core: $(NAME)

games: $(GAMES)

graphicals: $(GRAPHICALS)

# Compilation Core
$(NAME): $(OBJ)
	$(CXX) -o $(NAME) $(OBJ) $(CXXFLAGS) $(CPPFLAGS)
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

# Compilation SDL2
$(SDL2_LIB): $(SDL2_OBJ)
	@mkdir -p $(LIB_DIR)
	$(CXX) -shared -o $@ $^ $(SDL2_FLAGS)
	@printf "$(GREEN)[OK]$(RESET) Library '$@' compiled successfully.\n"

# Compilation Allegro5
$(ALLEGRO5_LIB): $(ALLEGRO5_OBJ)
	@mkdir -p $(LIB_DIR)
	$(CXX) -shared -o $@ $^ $(ALLEGRO5_FLAGS)
	@printf "$(GREEN)[OK]$(RESET) Library '$@' compiled successfully.\n"
$(SNAKE_LIB): $(SNAKE_OBJ)
	@mkdir -p $(LIB_DIR)
	$(CXX) -shared -o $@ $^
	@printf "$(GREEN)[OK]$(RESET) Game '$@' compiled successfully.\n"

$(MINESWEEPER_LIB): $(MINESWEEPER_OBJ)
	@mkdir -p $(LIB_DIR)
	$(CXX) -shared -o $@ $^
	@printf "$(GREEN)[OK]$(RESET) Game '$@' compiled successfully.\n"

clean:
	rm -f $(OBJ) $(NCURSES_OBJ) $(SFML_OBJ) $(SDL2_OBJ) $(ALLEGRO5_OBJ) $(SNAKE_OBJ) $(MINESWEEPER_OBJ)
	@printf "$(BLUE)[INFO]$(RESET) Object files removed.\n"

fclean: clean
	rm -f $(NAME)
	rm -f $(GRAPHICALS)
	rm -f $(GAMES)
	@printf "$(BLUE)[INFO]$(RESET) Binaries and libraries removed.\n"

re: fclean all
	@printf "$(YELLOW)[INFO]$(RESET) Project rebuilt successfully.\n"

.PHONY: all core games graphicals clean fclean re