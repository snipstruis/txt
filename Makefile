C = clang++ -march=native --std=c++1z -Wall -Wextra -ggdb -O3 -fno-rtti -fno-exceptions -DNDEBUG

all: main.cc *.h
	$C -o txt main.cc -lSDL2 -lGL

