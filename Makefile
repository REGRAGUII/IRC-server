CXX = c++

CFLAGS = -Wall -Wextra -Werror  -std=c++98 -g3 -fsanitize=address

SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)

NAME = ircserv


all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all
