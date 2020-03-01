CC = g++
FLAGS = -std=c++11 -pedantic -O2 -Wall -Wextra -Werror -Wshadow -g -Wsign-conversion

all: gitint.o gitint-shell

gitint.o: gitint.h gitint.cpp
	$(CC) $(FLAGS) gitint.h gitint.cpp -c 

gitint-shell: gitint.o gitint-shell.cpp
	$(CC) $(FLAGS) gitint.o gitint-shell.cpp -o gitint-shell
	
clean:
	rm -rf *o
	rm gitint-shell