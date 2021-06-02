#include <iostream>
#include <algorithm>
#include <fstream> 
#include <stdio.h>
#include <string.h>
#include <cmath> 
#include <string>
#include <cstdlib>
#include <unordered_map>
#include "BOBHash32.h"
#include "data.h"
#include "frequent_app.h"
#include "util.h"
#include <cassert>
using namespace std;


FILE* fout;

BOBHash32* hash_id = new BOBHash32(rand() % MAX_PRIME32);

void test_topk(int topk, int rep_time, int tot_flow, int memory){
    Frequent_App* s;
	int i, j;
	double ARE, AAE;

	double avg_AAE = 0, avg_ARE = 0, avg_F1 = 0;

	for (int t = 0; t < rep_time; t++){
		printf("%d ", t);

		s = new Frequent_App(1, 1. / 1.08, 1, 1, (double)memory / 76.0);

		for (i = 0; i < tot_flow; i++){
			s->insert(hashed_value[i]);
        }
		

		// Count + Heap
		ARE = AAE = 0;

		s->query_init(topk);

		int hit = 0, tmp_hit;
		for (i = 0; i < topk; i++){
			int res = s->query_topk(flow[i].hash_value, tmp_hit);

			hit += tmp_hit;
			if (tmp_hit){
				AAE += fabs(res - flow[i].cnt);
                ARE += fabs(res - flow[i].cnt) / (double)(flow[i].cnt);
			}
		}

		avg_AAE += AAE / (double)hit / rep_time;
		avg_ARE += ARE / (double)hit / rep_time;
		avg_F1 += (double)hit / (double)topk / rep_time;
		delete(s);
	}
	fprintf(fout, "%f %f %f\n", avg_AAE, avg_ARE, avg_F1);
	printf("\n");
}

int main(int argc, const char* argv[]){
	int memory, percent, topk_val, blen;
	int n_case = 0, dataset_id;
	int tot_flow_cnt;
	char exp_name[100];
	FILE* fin = fopen(argv[1], "r");

	fscanf(fin, "%d %d %s", &n_case, &dataset_id, exp_name);
	fout = fopen(exp_name, "a+");
	fprintf(fout, "Frequent:\n");

	srand(time(NULL));
	printf("start loading\n");
	tot_flow_cnt = load_data(TOTAL_FLOW, dataset_id);
	printf("end loading\n");

	for (int t = 0; t < n_case; t++){
		printf("Test case #%d\n", t);
		fscanf(fin, "%d %d %d %d", &memory, &percent, &topk_val, &blen);
		
		test_topk(topk_val, REP_TIME, min(TOTAL_FLOW, tot_flow_cnt), memory * 1024);
		printf("\n");
	}
	fclose(fin);
    fclose(fout);
	return 0;
} 
