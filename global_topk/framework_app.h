#ifndef FRAMEWORK_APP_H
#define FRAMEWORK_APP_H

#include "framework.h"
#include "util.h"
using namespace std;

class Sketch_fq: public Framework{ // 20 bytes per item
public:
	Sketch_fq(double a, double c, int hNum, int bNum, int blen, int cbNum, int crNum); 
	void insert(int in);
};

Sketch_fq::Sketch_fq(double a = 0.07, double c = 1 / 1.08, int hNum = 1, int bNum = 256, int blen=8, int cbNum = 20480, int crNum = 1){
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

void Sketch_fq::insert(int in){
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



class Sketch_hg: public Framework{ // 20 bytes per item
public:
	void insert(int in);
	Sketch_hg(double a, double c, int hNum, int bNum, int blen, int cbNum, int crNum);
	double power_table[100];
};

Sketch_hg::Sketch_hg(double a = 0.07, double c = 1 / 1.08, int hNum = 1, int bNum = 256, int blen=8, int cbNum = 20480, int crNum = 1){
	light_part = new CSMSketch(cbNum, crNum);
	light_part2 = new CSMSketch(cbNum, crNum);

	cons = c;
	alpha = a;
	bucketNum = bNum;
	bucket_length = blen;
	hashNum = hNum;
	for (int i = 0; i < bucketNum; i++){
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
	for (int i = 0; i < hashNum; i++)
		hash_cnt[i].initialize(rand() % MAX_PRIME32);
	for (int i = 0; i < 15; i++){
		power_table[i] = pow(c, i);
	}
}

void Sketch_hg::insert(int in){
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

    char c_old[4] = {};
    memcpy((void*)c_old, &item[h_id][arg_min].id, 4);

    if(rand() % 100000 < alpha * 100000 * max(item[h_id][arg_min].cnt < 15? power_table[item[h_id][arg_min].cnt]: 0, 1. / (double)(item[h_id][arg_min].cnt + 1))){
		if (item[h_id][arg_min].cnt == 1){
			light_part->insert(c_old, item[h_id][arg_min].underest);
			item[h_id][arg_min].underest = 1;
			item[h_id][arg_min].store_unbiased = light_part->query(c2, 1);
			item[h_id][arg_min].store_overest = light_part->query(c2, 0);
			item[h_id][arg_min].id = id_num;
			return;
		}
		item[h_id][arg_min].cnt--;
    }
	
    light_part->insert(c2, 1);
}



class Sketch_ra: public Framework{ // 20 bytes per item
public:
	void insert(int in);
	Sketch_ra(double a, double c, int hNum, int bNum, int blen, int cbNum, int crNum);
};

Sketch_ra::Sketch_ra(double a = 0.07, double c = 1 / 1.08, int hNum = 1, int bNum = 256, int blen=8, int cbNum = 20480, int crNum = 1){
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

void Sketch_ra::insert(int in){
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

	char c_old[4];
    memcpy((void*)c_old, &item[h_id][arg_min].id, 4);

    if(rand() % 100000 < alpha * 100000 *  1. / (double)(item[h_id][arg_min].cnt + 1)){
		light_part->insert(c_old, item[h_id][arg_min].underest);
		item[h_id][arg_min].underest = 1;
		item[h_id][arg_min].store_unbiased = light_part->query(c2, 1);
		item[h_id][arg_min].store_overest = light_part->query(c2, 0);
		item[h_id][arg_min].id = id_num;
    }

    
    else{
        light_part->insert(c2, 1);
    }
}


class Sketch_ss: public Framework{ // 20 bytes per item
public:
	void insert(int in);
	Sketch_ss(double a, double c, int hNum, int bNum, int blen, int cbNum, int crNum);
};

Sketch_ss::Sketch_ss(double a = 0.07, double c = 1 / 1.08, int hNum = 1, int bNum = 256, int blen = 8, int cbNum = 20480, int crNum = 1){
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

void Sketch_ss::insert(int in){
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

    char c_old[4] = {};
    memcpy((void*)c_old, &item[h_id][arg_min].id, 4);

	item[h_id][arg_min].cnt++;
    light_part->insert(c_old, item[h_id][arg_min].underest);
	item[h_id][arg_min].underest = 1;
	item[h_id][arg_min].id = id_num;
	item[h_id][arg_min].store_unbiased = light_part->query(c2, 1);
	item[h_id][arg_min].store_overest = light_part->query(c2, 0);
}

class Sketch_naive_ra: public Framework{
public:
	void insert(int in);
	Sketch_naive_ra(double a, double c, int hNum, int bNum, int blen, int cbNum, int crNum);
};

Sketch_naive_ra::Sketch_naive_ra(double a = 0.07, double c = 1 / 1.08, int hNum = 1, int bNum = 256, int blen=8, int cbNum = 20480, int crNum = 1){
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


void Sketch_naive_ra::insert(int in){
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
        if ((item[h_id][j].id == id_num) && (item[h_id][j].cnt >= 1)){
            item[h_id][j].cnt++;
            goto naive_light;
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
            goto naive_light;
        }
    }

    if(rand() % 100000 < alpha * 100000 *  1. / (double)(item[h_id][arg_min].cnt + 1)){
		item[h_id][arg_min].id = id_num;
    }

    
    naive_light: light_part->insert(c2, 1);
}


Framework* new_sketch(int sketch_id, int heavy_bucnum, int bucket_length, int light_bucnum){
    switch (sketch_id){
        case 0:
            return new Sketch_ss(1, 1. / 1.08, 1, heavy_bucnum, bucket_length, light_bucnum);
        case 1:
            return new Sketch_fq(1, 1. / 1.08, 1, heavy_bucnum, bucket_length, light_bucnum);
        case 2:
            return new Sketch_ra(1, 1. / 1.08, 1, heavy_bucnum, bucket_length, light_bucnum);
        case 3:
            return new Sketch_hg(1, 1. / 1.08, 1, heavy_bucnum, bucket_length, light_bucnum);
		case 4:
			return new Sketch_naive_ra(1, 1. / 1.08, 1, heavy_bucnum, bucket_length, light_bucnum);

    }
}

void get_sketch_name(int sketch_id, char* ret_name){
	switch (sketch_id){
        case 0:
            sprintf(ret_name, "DB + SS");
			return;
        case 1:
			sprintf(ret_name, "DB + FQ");
            return;
        case 2:
			sprintf(ret_name, "DB + RA");
            return;
        case 3:
            sprintf(ret_name, "DB + HG");
			return;
		default:
			sprintf(ret_name, "naive + RA");
			return;
    }
}

#endif