CXX = g++
CFLAGS = -Wall -Wextra -pedantic -Werror
LDFLAGS = -lm -lcurl -lpugixml

NAME = towers
SRC_PREFIX = ./src/
O_PREFIX = $(SRC_PREFIX).o_files/

SRC = main.cpp extractor.cpp requests.cpp parser.cpp preprocess.cpp
HEADERS = main.hpp
OBJ = $(addprefix $(O_PREFIX), $(SRC:.cpp=.o))

.PHONY: all debug profiling clean

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(OBJ) -o $(NAME) $(LDFLAGS)

$(O_PREFIX)%.o: $(SRC_PREFIX)%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

debug: CFLAGS += -g
debug: all

profiling: CFLAGS += -pg
profiling: LDFLAGS += -pg
profiling: all

clean:
	rm -f $(OBJ) $(NAME)