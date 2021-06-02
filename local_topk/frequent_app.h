#ifndef FREQUENT_APP_H
#define FREQUENT_APP_H

#include "frequent.h"
#include "util.h"
using namespace std;

class Frequent_App: public Frequent{ // 20 bytes per item
public:
	Frequent_App(double a, double c, int hNum, int bNum, int blen, int cbNum, int crNum); 
	void insert(int in);
};

Frequent_App::Frequent_App(double a = 0.07, double c = 1 / 1.08, int hNum = 1, int bNum = 256, int blen=8, int cbNum = 20480, int crNum = 1){
	light_part = new CSMSketch(cbNum, crNum);
	light_part2 = new CSMSketch(cbNum, crNum);

	cons = c;
	alpha = a;
	bucketNum = bNum;
	bucket_length = blen;
	hashNum = hNum;
	for(int i = 0; i < bucketNum; i++){
		for (int j = 0; j < bucket_length; j++){
            item[i][j].id = 0;
		    item[i][j].cnt = 0;
			item[i][j].underest = 0;
			item[i][j].store_unbiased = 0;
			item[i][j].store_overest = 0;
        }
	}
	hash_id = new BOBHash32(rand() % MAX_PRIME32);
	hash_cnt = new BOBHash32[hashNum];
	for(int i = 0; i < hashNum; i++)
		hash_cnt[i].initialize(rand() % MAX_PRIME32);
}

void Frequent_App::insert(int in){
	unsigned int id_num = in;
	char c2[4] = {};
	memcpy((void*)c2, &in, 4);
	#ifdef DEBUG_MODE
		light_part2->insert(c2, 1);
	#endif
	unsigned int h_id = hash_cnt[0].run(c2, 4) % bucketNum;  

	int i, j;
    int min_cnt = INF, arg_min = -1;

	for (j = 0; j < bucket_length; j++){
        if ((item[h_id][j].id == id_num) && (item[h_id][j].underest >= 1)){
            item[h_id][j].cnt++;
			item[h_id][j].underest++;
            return;
        }
    }

    for (j = 0; j < bucket_length; j++){
        if (min_cnt > item[h_id][j].cnt){
            min_cnt = item[h_id][j].cnt;
            arg_min = j;
        }
    }

    for (j = 0; j < bucket_length; j++){
        if (item[h_id][j].cnt == 0){
            item[h_id][j].id = id_num;
            item[h_id][j].cnt = 1;
			item[h_id][j].underest = 1;
			item[h_id][j].store_unbiased = light_part->query(c2, 1);
			item[h_id][j].store_overest = light_part->query(c2, 0);
            return;
        }
    }

    light_part->insert(c2, 1);

    for (i = 0; i < bucket_length; i++){
        item[h_id][i].cnt--;
        if (item[h_id][i].cnt == 0){
            char c_old[4] = {};
            memcpy(c_old, &item[h_id][i].id, 4);
            item[h_id][i].id = 0;
            light_part->insert(c_old, item[h_id][i].underest);
            item[h_id][i].underest = 0;
        }
    }
}


#endif