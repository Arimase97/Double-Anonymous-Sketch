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
#include "framework_app.h"
#include "util.h"
#include <cassert>
using namespace std;


void test_topk(int bucket_length, int topk, int rep_time, int tot_flow, int heavy_bucnum, int light_bucnum, FILE* fout){	
	int i, j;
	double ARE_under, ARE_unbiased, ARE_over, ARE, AAE_unbiased, throughput;

	double avg_AAE_unbiased = 0, avg_ARE_under = 0, avg_ARE_unbiased = 0, avg_ARE_over = 0, avg_F1 = 0, avg_throughput = 0;

	Framework* s;
	for (int t = 0; t < rep_time; t++){
		printf("%d ", t);

		s = new_sketch(4, heavy_bucnum, bucket_length, light_bucnum);

		TP start, finish;
		start = now();
		for (i = 0; i < tot_flow; i++){
			s->insert(hashed_value[i]);
		}
		finish = now();
		throughput = (double) tot_flow / std::chrono::duration_cast<std::chrono::duration<double,std::ratio<1,1000000> > >(finish - start).count();
		avg_throughput += throughput / rep_time;


		// proposed sketch
		ARE_under = ARE_over = ARE_unbiased = AAE_unbiased = 0;

		s->query_init(topk);

		int hit = 0, tmp_hit;
		for (i = 0; i < topk; i++){
			int res = s->query_topk(flow[i].hash_value, tmp_hit, 1, 1);

			hit += tmp_hit;
			if (tmp_hit){
				AAE_unbiased += fabs(res - flow[i].cnt);
				ARE_unbiased += fabs(res - flow[i].cnt) / (double)(flow[i].cnt);
			}

			res = s->query_topk(flow[i].hash_value, tmp_hit, 0, 0);
			if (tmp_hit){
				ARE_under += fabs(res - flow[i].cnt) / (double)(flow[i].cnt);
			}
			res = s->query_topk(flow[i].hash_value, tmp_hit, 2, 0);
			if (tmp_hit){
				ARE_over += fabs(res - flow[i].cnt) / (double)(flow[i].cnt);
			}
		}
		avg_AAE_unbiased += AAE_unbiased / (double)hit / rep_time;
		avg_ARE_unbiased += ARE_unbiased / (double)hit / rep_time;
		avg_ARE_under += ARE_under / (double)hit / rep_time;
		avg_ARE_over += ARE_over / (double)hit / rep_time;
		avg_F1 += (double)hit / (double)topk / rep_time;

		delete(s);
	}
	fprintf(fout, "%.9f %.9f %.9f %.9f %.9f %.9f\n", avg_throughput, avg_AAE_unbiased, avg_ARE_unbiased, avg_F1, avg_ARE_under, avg_ARE_over);
	printf("\n");
}


int main(int argc, const char* argv[]){
	int memory, percent, topk_val, blen;
	int n_case = 0, dataset_id;
	int tot_flow_cnt;
	char exp_name[100];
	FILE* fin = fopen(argv[1], "r");

	fscanf(fin, "%d %d %s", &n_case, &dataset_id, exp_name);
	FILE* fout = fopen(exp_name, "a+");
	fprintf(fout, "CSM + naive + RA:\n");

	srand(time(NULL));
	printf("start loading\n");
	tot_flow_cnt = load_data(TOTAL_FLOW, dataset_id);
	printf("end loading\n");

	for (int t = 0; t < n_case; t++){
		printf("Test case #%d\n", t);
		fscanf(fin, "%d %d %d %d", &memory, &percent, &topk_val, &blen);
		
		int heavy_bucnum = memory * percent * 1024 / 1600 / blen; // 16 bytes per one cell
		int light_bucnum = memory * (100 - percent) * 1024 / 300; // 3 bytes per item
		
		test_topk(blen, topk_val, REP_TIME, min(TOTAL_FLOW, tot_flow_cnt), heavy_bucnum, light_bucnum, fout);
		printf("\n");
	}
	fclose(fin);
    fclose(fout);
	return 0;
}