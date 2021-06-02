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
#include "archive/USS.h"
#include "util.h"
#include <cassert>
using namespace std;


FILE* fout;

BOBHash32* hash_id = new BOBHash32(rand() % MAX_PRIME32);

void test_topk(int topk, int rep_time, int tot_flow, int memory){
    USS* s;
	int i, j;
	double  ARE, AAE, throughput;

	double avg_throughput = 0, avg_AAE = 0, avg_ARE = 0, avg_F1 = 0;

	for (int t = 0; t < rep_time; t++){
		printf("%d ", t);

		s = new USS((memory) / 76);

		TP start, finish;
		start = now();
		for (i = 0; i < tot_flow; i++){
			s->Insert(hashed_value[i]);
        }
		finish = now();
		throughput = (double) tot_flow / std::chrono::duration_cast<std::chrono::duration<double,std::ratio<1,1000000> > >(finish - start).count();
		avg_throughput += throughput / rep_time;
		

		// Count + Heap
		ARE = AAE = 0;

		s->query_init(flow, topk, flow_cnt);

		int hit = 0, tmp_hit;
		for (i = 0; i < topk; i++){
			int res = s->query_topk(flow[i].hash_value);
            tmp_hit = res > 0;

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
	fprintf(fout, "%f %f %f %f\n", avg_throughput, avg_AAE, avg_ARE, avg_F1);
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
	fprintf(fout, "USS:\n");

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
