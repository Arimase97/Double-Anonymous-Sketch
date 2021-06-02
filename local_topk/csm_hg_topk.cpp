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
#include "test_topk.h"
#include "util.h"
#include <cassert>
using namespace std;



int main(int argc, const char* argv[]){
	int memory, percent, topk_val, blen;
	int n_case = 0, dataset_id;
	int tot_flow_cnt;
	char exp_name[100];
	FILE* fin = fopen(argv[1], "r");

	fscanf(fin, "%d %d %s", &n_case, &dataset_id, exp_name);
	FILE* fout = fopen(exp_name, "a+");
	fprintf(fout, "CSM + HeavyGuardian:\n");

	srand(time(NULL));
	printf("start loading\n");
	tot_flow_cnt = load_data(TOTAL_FLOW, dataset_id);
	printf("end loading\n");

	for (int t = 0; t < n_case; t++){
		printf("Test case #%d\n", t);
		fscanf(fin, "%d %d %d %d", &memory, &percent, &topk_val, &blen);
		
		int heavy_bucnum = memory * percent * 1024 / 1600 / blen; // 16 bytes per item
		int light_bucnum = memory * (100 - percent) * 1024 / 200; // 2 bytes per item
		
		test_topk(blen, topk_val, REP_TIME, min(TOTAL_FLOW, tot_flow_cnt), heavy_bucnum, light_bucnum, 3, fout);
		printf("\n");
	}
	fclose(fin);
    fclose(fout);
	return 0;
} 
