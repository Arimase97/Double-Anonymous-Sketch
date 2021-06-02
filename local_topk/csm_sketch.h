#include "BOBHash32.h"
#include "cassert"
#ifndef CSM_SKETCH_H
#define CSM_SKETCH_H
using namespace std;

class CSMSketch{ // 4 bytes per item
public:
	int bucketNum;
	int dNum;
	static const int MAX_rawNum = 6;
	static const int MAX_bucketNum = 1005000; 

	CSMSketch(int bNum, int rNum);
	void insert(char* s, int num);
	int query(char* s, int opt);
	unsigned int cnt[MAX_bucketNum];
    int total_counter;
	BOBHash32* hash;

	uint32_t hash_seed;
};

CSMSketch::CSMSketch(int _bNum=204800, int _dNum=1){
	bucketNum = _bNum;
	dNum= _dNum;
	hash = new BOBHash32[dNum + 1];
    total_counter = 0;
	
    for (int j = 0; j < bucketNum; j++){
        cnt[j] = 0;
    }
	
	hash_seed = rand() % MAX_PRIME32;
	for (int i = 0; i < dNum; i++){
		hash[i].initialize(hash_seed);
	}
	hash[dNum].initialize(hash_seed);
}

void CSMSketch::insert(char* in, int inc_cnt){
	//unsigned int inNum[dNum + 10];

	//int idx = rand() % dNum;
	int idx = 0;
    int buc_id = BOBHash_run(*(uint32_t*)in, hash_seed) % bucketNum;
    cnt[buc_id] += (short)inc_cnt;


    total_counter += inc_cnt;
}

/*
 * opt: whether we query the unbiased value or over estimated value
 *      0: over estimated value
 *      1: unbiased value
 * 
 */
int CSMSketch::query(char* in, int opt){
	unsigned int inNum[dNum + 10];
	for(int i = 0; i < dNum; i++){
		inNum[i] = hash[i].run(in, 4) % bucketNum;
	}
	
	int res = 0;
    for (int i = 0; i < dNum; i++){
        res += (unsigned int)cnt[inNum[i]];
    }
	
    assert(opt == 0 || opt == 1);
    if (opt == 0){
        return res;
    }
    else if (opt == 1){
        return res - dNum * (total_counter - res) / (bucketNum - 1);
    }
}

#endif