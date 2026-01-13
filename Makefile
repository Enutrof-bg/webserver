CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98

HEADER = includes/webserv.hpp

FILE = main.cpp Config.cpp Server.cpp Response.cpp Utils.cpp ErrorCode.cpp

NAME = webserv

FILE_PATH = srcs/
OBJ_DIR = obj/

SRC = $(addprefix $(FILE_PATH), $(FILE))
OBJ = $(patsubst $(FILE_PATH)%.cpp, $(OBJ_DIR)%.o, $(SRC))

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $(NAME)

$(OBJ_DIR)%.o: $(FILE_PATH)%.cpp $(HEADER)
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean: 
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re



