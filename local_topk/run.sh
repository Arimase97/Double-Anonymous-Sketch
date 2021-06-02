#!/bin/bash/

make clean
make

./waving_topk.out config.txt
./csm_ra_topk.out config.txt
./csm_ra_naive.out config.txt
./uss.out config.txt
./SS.out config.txt
./frequent_test.out config.txt