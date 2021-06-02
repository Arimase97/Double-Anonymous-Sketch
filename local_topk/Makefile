CC = g++
CFLAGS = -O2 -std=gnu++14 -w

OBJS = csm_fq_topk.out csm_hg_topk.out csm_ra_topk.out csm_ss_topk.out uss.out waving_topk.out csm_ra_naive.out SS.out frequent_test.out

all: $(OBJS)

csm_hg_topk.out: csm_hg_topk.cpp
	$(CC) $(CFLAGS) -o csm_hg_topk.out csm_hg_topk.cpp

csm_ss_topk.out: csm_ss_topk.cpp
	$(CC) $(CFLAGS) -o csm_ss_topk.out csm_ss_topk.cpp

csm_ra_topk.out: csm_ra_topk.cpp
	$(CC) $(CFLAGS) -o csm_ra_topk.out csm_ra_topk.cpp

csm_fq_topk.out: csm_fq_topk.cpp
	$(CC) $(CFLAGS) -o csm_fq_topk.out csm_fq_topk.cpp

uss.out: uss.cpp
	$(CC) $(CFLAGS) -o uss.out uss.cpp

waving_topk.out: waving_topk.cpp
	$(CC) $(CFLAGS) -o waving_topk.out waving_topk.cpp

csm_ra_naive.out: csm_ra_naive.cpp
	$(CC) $(CFLAGS) -o csm_ra_naive.out csm_ra_naive.cpp

SS.out: SS.cpp
	$(CC) $(CFLAGS) -o SS.out SS.cpp

frequent_test.out: frequent_test.cpp
	$(CC) $(CFLAGS) -o frequent_test.out frequent_test.cpp

clean:
	rm -f $(OBJS)