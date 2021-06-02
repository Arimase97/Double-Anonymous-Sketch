#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#include "data.h"
#include "csm_sketch.h"
#include <cstring>
#include <algorithm>
using namespace std;

class Framework{
public:
    int bucketNum;
    int bucket_length;
	int hashNum;
	static const int MAX_bucketNum = 102400;
	double cons;
	double alpha;

	class Item{
	public:
		int id;
		int cnt;
		int underest;
		int store_unbiased;
		int store_overest;
	};
	Item item[MAX_bucketNum][64];

	CSMSketch* light_part;
	CSMSketch* light_part2;
	BOBHash32* hash_cnt;
	BOBHash32* hash_id;

	bool is_topk[MAX_bucketNum][64]; // data structures convenient for query
	class Query_Item: public Item{
	public:
		int buc_id;
		int pos;

		bool operator < (const Query_Item& other) const{
			return cnt > other.cnt;
		}
	};

	virtual void insert(int) = 0;

	void report_topk(unordered_map<int, int>& result, int topk, int opt = 0){
		Query_Item* query_item = new Query_Item[MAX_bucketNum * 32];

		for (int i = 0; i < bucketNum; i++){
			for (int j = 0; j < bucket_length; j++){
				query_item[i * bucket_length + j].buc_id = i;
				query_item[i * bucket_length + j].pos = j;
				query_item[i * bucket_length + j].cnt = item[i][j].cnt;
			}
		}

		sort(query_item, query_item + bucketNum * bucket_length);
		
		result.clear();
		for (int i = 0; i < min(topk, bucketNum * bucket_length); i++){
			int tmp;
			is_topk[query_item[i].buc_id][query_item[i].pos] = 1;
			result[item[query_item[i].buc_id][query_item[i].pos].id] = query_topk(item[query_item[i].buc_id][query_item[i].pos].id, tmp, 1, opt);
		}
		for (int i = bucketNum * bucket_length; i < topk; i++){
			result[rand()] = -1;
		}
		
		delete(query_item);
	}

	void report_topk_under(unordered_map<int, int>& result, int topk){
		Query_Item* query_item = new Query_Item[MAX_bucketNum * 32];

		for (int i = 0; i < bucketNum; i++){
			for (int j = 0; j < bucket_length; j++){
				query_item[i * bucket_length + j].buc_id = i;
				query_item[i * bucket_length + j].pos = j;
				query_item[i * bucket_length + j].cnt = item[i][j].cnt;
			}
		}

		sort(query_item, query_item + bucketNum * bucket_length);
		
		result.clear();
		for (int i = 0; i < topk; i++){
			int tmp;
			is_topk[query_item[i].buc_id][query_item[i].pos] = 1;
			result[item[query_item[i].buc_id][query_item[i].pos].id] = query_item[i].cnt;
		}
		
		delete(query_item);
	}

	int query_init(int topk){
		memset(is_topk, 0, sizeof(is_topk));
		Query_Item* query_item = new Query_Item[MAX_bucketNum * 32];

		for (int i = 0; i < bucketNum; i++){
			for (int j = 0; j < bucket_length; j++){
				query_item[i * bucket_length + j].buc_id = i;
				query_item[i * bucket_length + j].pos = j;
				query_item[i * bucket_length + j].cnt = item[i][j].cnt;
			}
		}

		sort(query_item, query_item + bucketNum * bucket_length);
		
		for (int i = 0; i < topk; i++){
			is_topk[query_item[i].buc_id][query_item[i].pos] = 1;
		}
		
		delete(query_item);
		
	}

	int query_topk(int in, int& hit, int opt, int opt2){
		assert (opt >= 0 && opt <= 2);
		
		char temp1[4];
		memcpy((void*)temp1, &in, 4);
		unsigned int id_num = in;
		unsigned int h_id = hash_cnt[0].run(temp1, 4) % bucketNum; 

		for (int i = 0; i < bucket_length; i++){
			if ((item[h_id][i].id == id_num) && (is_topk[h_id][i] == 1)){
				hit = 1;
				if (opt == 0){
					return item[h_id][i].underest;
				}
				else if (opt == 1){
					if (opt2 == 0){
						return item[h_id][i].underest + item[h_id][i].store_unbiased;
					}
					else{
						return item[h_id][i].underest + light_part->query(temp1, 1);
					}
				}
				else if (opt == 2){
					if (opt2 == 0){
						return item[h_id][i].underest + item[h_id][i].store_overest;
					}
					else{
						return item[h_id][i].underest + light_part->query(temp1, 0);
					}
				}
			}
		}
		hit = 0;
		return 0;
	}

	int query_thres(int in, int& hit, int opt, int opt2, int threshold=0){
		assert (opt >= 0 && opt <= 2);
		
		char temp1[4];
		memcpy((void*)temp1, &in, 4);
		unsigned int id_num = in;
		unsigned int h_id = hash_cnt[0].run(temp1, 4) % bucketNum; 

		for (int i = 0; i < bucket_length; i++){
			if ((item[h_id][i].id == id_num) && (item[h_id][i].cnt >= threshold)){
				hit = 1;
				if (opt == 0){
					return item[h_id][i].underest;
				}
				else if (opt == 1){
					if (opt2 == 0){
						return item[h_id][i].underest + item[h_id][i].store_unbiased;
					}
					else{
						return item[h_id][i].underest + light_part->query(temp1, 1);
					}
				}
				else if (opt == 2){
					if (opt2 == 0){
						return item[h_id][i].underest + item[h_id][i].store_overest;
					}
					else{
						return item[h_id][i].underest + light_part->query(temp1, 0);
					}
				}
			}
		}
		hit = 0;
		return 0;
	}

	int query_cs(int in){
		char temp1[4] = {};
		memcpy((void*)temp1, &in, 4);

		return light_part2->query(temp1, 1);
	}

	double query_precision(flow_item* flows, int topk){
		int threshold = flows[topk - 1].cnt;
		int all = 0;
		for (int i = 0; i < bucketNum; i++){
			for (int j = 0; j < bucket_length; j++){
				if (item[i][j].cnt >= threshold){
					all++;
				}
			}
		}

		int hit = 0, tmp_hit;
		for (int i = 0; i < topk; i++){
			query_thres(flows[i].hash_value, tmp_hit, 0, 0, threshold);
			hit += tmp_hit;
		}
		return (double)hit / (double)all;
	}
};


#endif