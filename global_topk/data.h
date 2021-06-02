#ifndef DATA_H
#define DATA_H

#include <cstdlib>
#include <string>
#include <cstdio>
#include <cstring>
#include "BOBHash32.h"
using namespace std;

const int MAXNUM = 5 * 1e7;
unordered_map<string, int> realAns;
unordered_map<int, int> realAns_hashed;
char inp_stream[30001000][15];
int hashed_value[30001000];

static BOBHash32* hash_func = new BOBHash32(rand() % MAX_PRIME32);

int flow_cnt = 0;
struct flow_item{
	int cnt;
	string id;
    int hash_value;
    int sketch_id; // for global topk
	bool operator < (const flow_item& other) const {
		return cnt > other.cnt;
	}
}flow[5001000];

struct KV_pair{
    int key;
    int value;

    KV_pair () {}
    KV_pair (int _key, int _value){
        key = _key;
        value = _value;
    }
    bool operator < (const KV_pair& other) const {
        return value > other.value;
    }
};

int load_data_CAIDA(int max_item, char* file_name){
    FILE* fin = fopen(file_name, "r");
    char buf[100] = {0};
    int i, j;
    realAns.clear();
    for (i = 0; i < max_item; i++)
    {
        if (fread(buf, 1, 13, fin) < 13){
            break;
        }
        char temp1[15];
        strncpy(temp1, buf, 13);
		double time_stamp = *(double*)(buf + 13);
		temp1[13] = 0;
		for (int j = 0; j < 13; j++){
			temp1[j] = buf[j];
			if (temp1[j] == 0){
				temp1[j] = 'a';
			}
			inp_stream[i][j] = temp1[j];
		}
		inp_stream[i][13] = inp_stream[i][14] = 0;
        string s1 = temp1;
		realAns[s1] += 1;
    }
    printf("i: %d\n", i);
    fclose(fin);
    return i;
}

int load_data(int max_item, char* file_name){
    int ret;
    ret = load_data_CAIDA(max_item, file_name);

    unordered_map <string, int>::iterator it;
	flow_cnt = 0;
    realAns_hashed.clear();
	for (it = realAns.begin(); it != realAns.end(); it++){
		char temp1[15];
		strcpy(temp1, it->first.c_str());
        flow[flow_cnt].cnt = it->second;
		flow[flow_cnt].id = it->first;
        flow[flow_cnt].hash_value = hash_func->run(temp1, 14);
        realAns_hashed[flow[flow_cnt].hash_value] = it->second;
		flow_cnt++;
	}
	sort(flow, flow + flow_cnt);

    for (int i = 0; i < ret; i++){
        hashed_value[i] = hash_func->run(inp_stream[i], 14);
    }
    return ret;
}

#endif