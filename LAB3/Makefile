CC = g++
FLAGS = -Wall -g

PROJECT = AES
MAIN = main.cc
HEADERS = aes.h common.h mode.h crypto.h
SRC = aes.cc mode.cc crypto.cc
OBJECTS = ${SRC:.cc=.o}

all: $(MAIN) $(HEADERS) $(OBJECTS)
	$(CC) $(FLAGS) $(OBJECTS) $(MAIN) -o $(PROJECT)

$(OBJECTS): $(SRC) $(HEADERS)

clean:
	rm -f $(OBJECTS) $(PROJECT)
	