CC = g++ 
CFLAGS = -Wall -std=c++17 -O3

all: local_cut global_cut

local_cut: local_cut.cpp
	$(CC) $(CFLAGS) -o local_cut local_cut.cpp

global_cut: global_cut.cpp
	$(CC) $(CFLAGS) -o global_cut global_cut.cpp

clean:
	rm -f local_cut global_cut