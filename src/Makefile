CC = g++
CFLAGS = -Wall -std=c++17 -O3
COMMON_HEADER = common.hpp dinic.hpp kvcc.hpp

all: local_cut global_cut kvcc_baseline kvcc_sweep

local_cut: local_cut.cpp $(COMMON_HEADER)
	$(CC) $(CFLAGS) -o local_cut local_cut.cpp

global_cut: global_cut.cpp $(COMMON_HEADER)
	$(CC) $(CFLAGS) -o global_cut global_cut.cpp

kvcc_baseline: kvcc_baseline.cpp $(COMMON_HEADER)
	$(CC) $(CFLAGS) -o kvcc_baseline kvcc_baseline.cpp

kvcc_sweep: kvcc_sweep.cpp $(COMMON_HEADER)
	$(CC) $(CFLAGS) -o kvcc_sweep kvcc_sweep.cpp

clean:
	rm -f local_cut global_cut kvcc_baseline kvcc_sweep