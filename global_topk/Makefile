CC = g++
CFLAGS = -g -std=gnu++11 -w

OBJS = global_topk_framework.out global_topk_USS.out global_topk_waving.out global_topk_frequent.out global_topk_SS.out global_topk_naive.out

all: $(OBJS)

global_topk_framework.out: global_topk_framework.cpp
	$(CC) $(CFLAGS) -o global_topk_framework.out global_topk_framework.cpp

global_topk_USS.out: global_topk_USS.cpp
	$(CC) $(CFLAGS) -o global_topk_USS.out global_topk_USS.cpp

global_topk_waving.out: global_topk_waving.cpp
	$(CC) $(CFLAGS) -o global_topk_waving.out global_topk_waving.cpp

global_topk_frequent.out: global_topk_frequent.cpp
	$(CC) $(CFLAGS) -o global_topk_frequent.out global_topk_frequent.cpp

global_topk_SS.out: global_topk_SS.cpp
	$(CC) $(CFLAGS) -o global_topk_SS.out global_topk_SS.cpp

global_topk_naive.out: global_topk_naive.cpp
	$(CC) $(CFLAGS) -o global_topk_naive.out global_topk_naive.cpp


clean:
	rm -f $(OBJS)