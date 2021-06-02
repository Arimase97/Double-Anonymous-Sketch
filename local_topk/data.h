#ifndef DATA_H
#define DATA_H

#include <cstdlib>
#include <cstring>
#include <string>
#include <cstdio>
#include "BOBHash32.h"
using namespace std;

const int MAXNUM = 5 * 1e7;
unordered_map<string, int> realAns;
char inp_stream[20001000][15];
int hashed_value[20001000];

static BOBHash32 hash_func = BOBHash32(rand() % MAX_PRIME32);

int flow_cnt = 0;
struct flow_item{
	int cnt;
	string id;
    int hash_value;
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

int load_data_CAIDA(int max_item){
    FILE* fin = fopen("../data/caida.dat", "r");
    char buf[100] = {0};
    int i, j;
    for (i = 0; i < max_item; i++)
    {
        if (fread(buf, 1, 21, fin) < 21){
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

int load_data_WEB(int max_item){
    FILE* fin = fopen("../data/webdocs_form00.dat", "r");
    char buf[100] = {0};
    int i, j;
    for (i = 0; i < max_item; i++)
    {
        if (fread(buf, 1, 4, fin) < 4){
            break;
        }
        char temp1[15];
        strncpy(temp1, buf, 4);
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

int load_data_NET(int max_item){
    FILE* fin = fopen("../data/net.dat", "r");
    char buf[100] = {0};
    int i, j;
    for (i = 0; i < max_item; i++)
    {
        if (fread(buf, 1, 4, fin) < 4){
            break;
        }
        char temp1[15];
        strncpy(temp1, buf, 4);
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

int load_data_synthesis(int max_item){
    FILE* fin = fopen("../data/009.dat", "r");
    char buf[100] = {0};
    int i, j;
    for (i = 0; i < max_item; i++)
    {
        if (fread(buf, 1, 4, fin) < 4){
            break;
        }
        char temp1[15];
        strncpy(temp1, buf, 4);
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

int load_data_syn(int max_item, int dataset_id){
    char file_name[100];
    sprintf(file_name, "../data/%03d.dat", dataset_id);
    FILE* fin = fopen(file_name, "r");
    char buf[100] = {0};
    int i, j;
    for (i = 0; i < max_item; i++)
    {
        if (fread(buf, 1, 4, fin) < 4){
            break;
        }
        char temp1[15];
        strncpy(temp1, buf, 4);
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

int load_data(int max_item, int dataset_id){
    int ret;
    if (dataset_id == 0){
        ret = load_data_CAIDA(max_item);
    }
    else if (dataset_id == 1){
        ret = load_data_WEB(max_item);
    }
    else if (dataset_id == 2){
        ret = load_data_NET(max_item);
    }
    else if (dataset_id == 3){
        ret = load_data_synthesis(max_item);
    }
    else{
        ret = load_data_syn(max_item, (dataset_id - 3) * 3);
    }

    unordered_map <string, int>::iterator it;
	flow_cnt = 0;
	for (it = realAns.begin(); it != realAns.end(); it++){
		char temp1[15];
		strcpy(temp1, it->first.c_str());
        flow[flow_cnt].cnt = it->second;
		flow[flow_cnt].id = it->first;
        flow[flow_cnt].hash_value = hash_func.run2(temp1, 14);
		flow_cnt++;
	}
	sort(flow, flow + flow_cnt);

    for (int i = 0; i < ret; i++){
        hashed_value[i] = hash_func.run2(inp_stream[i], 14);
    }

    return ret;
}

#endif
