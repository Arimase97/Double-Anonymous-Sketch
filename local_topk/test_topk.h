#ifndef TEST_TOPK_H
#define TEST_TOPK_H

#include "framework_app.h"
#include <cstdlib>
#include <unistd.h>
using namespace std;

void test_topk(int bucket_length, int topk, int rep_time, int tot_flow, int heavy_bucnum, int light_bucnum, int sketch_id, FILE* fout){	
	int i, j;
	double ARE_under, ARE_unbiased, ARE_over, ARE, AAE_unbiased, throughput;

	double avg_AAE_unbiased = 0, avg_ARE_under = 0, avg_ARE_unbiased = 0, avg_ARE_over = 0, avg_F1 = 0, avg_throughput = 0;

	Framework* s;
	for (int t = 0; t < rep_time; t++){
		printf("%d ", t);

		s = new_sketch(sketch_id, heavy_bucnum, bucket_length, light_bucnum);

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
			int res = s->query_topk(flow[i].hash_value, tmp_hit, 1, 0); // unbiased + early freezing

			hit += tmp_hit;
			if (tmp_hit){
				AAE_unbiased += fabs(res - flow[i].cnt);
				ARE_unbiased += fabs(res - flow[i].cnt) / (double)(flow[i].cnt);
			}

			res = s->query_topk(flow[i].hash_value, tmp_hit, 0, 0); // underestimation + early freezing
			if (tmp_hit){
				ARE_under += fabs(res - flow[i].cnt) / (double)(flow[i].cnt);
			}
			res = s->query_topk(flow[i].hash_value, tmp_hit, 2, 0); // overestimation + early freezing
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

#endif