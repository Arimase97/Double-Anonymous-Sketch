#ifndef _WAVINGSKETCH_H
#define _WAVINGSKETCH_H

#include "abstract.h"
#include "../data.h"
#include <vector>

#define factor 1

static int seed;

template<uint32_t slot_num>
class WavingSketch : public Abstract{
public:
    struct Bucket{
        data_type items[slot_num];
        count_type counters[slot_num];
        count_type incast;

        void Insert(const data_type item){
            uint32_t choice = hash_(item, seed + 1) & 1;
            count_type min_num = INT_MAX;
            uint32_t min_pos = -1;
            
            for (uint32_t i = 0;i < slot_num;++i){
                if(counters[i] == 0){
                    items[i] = item;
                    counters[i] = -1;
                    return;
                }
                else if(items[i] == item){
                    if(counters[i] < 0)
                        counters[i]--;
                    else{
                        counters[i]++;
                        incast += COUNT[choice];
                    }
                    return;
                }

                count_type counter_val = std::abs(counters[i]);
                if(counter_val < min_num){
                    min_num = counter_val;
                    min_pos = i;
                }
            }

            if(incast * COUNT[choice] >= int(min_num * factor)){
                if(counters[min_pos] < 0){
                    uint32_t min_choice = hash_(items[min_pos], seed + 1) & 1;
                    incast -= COUNT[min_choice] * counters[min_pos];
                }
                items[min_pos] = item;
                counters[min_pos] = min_num + 1;
            }
            incast += COUNT[choice];
        }

        count_type Query(const data_type item){
            uint32_t choice = hash_(item, seed + 1) & 1;

            for(uint32_t i = 0;i < slot_num;++i){
                if(items[i] == item){
                    return std::abs(counters[i]);
                }
            }

            return 0;//incast * COUNT[choice];
        }

        count_type Unbiased_Query(const data_type item){
            uint32_t choice = hash_(item, seed + 1) & 1;
            for (uint32_t i = 0; i < slot_num; i++){
                if ((items[i] == item) && (counters[i] < 0)){
                    return -counters[i];
                }
            }

            return incast * COUNT[choice];
        }
    };

    WavingSketch(uint32_t _BUCKET_NUM):Abstract((char *)"WavingSketch"), BUCKET_NUM(_BUCKET_NUM){
        buckets = new Bucket[BUCKET_NUM];
        memset(buckets, 0, BUCKET_NUM * sizeof(Bucket));
        seed = rand() % (MAX_PRIME - 1);
    }
    ~WavingSketch(){
        delete [] buckets;
    }

    void Insert(const data_type item){
        uint32_t bucket_pos = hash_(item, seed) % BUCKET_NUM;
        buckets[bucket_pos].Insert(item);
    }

    count_type Query(const data_type item){
        uint32_t bucket_pos = hash_(item, seed) % BUCKET_NUM;
        return buckets[bucket_pos].Query(item);
    }

    count_type Unbiased_Query(const data_type item){
        uint32_t bucket_pos = hash_(item, seed) % BUCKET_NUM;
        return buckets[bucket_pos].Unbiased_Query(item);
    }

    void Query_Topk(unordered_map<int, int>& result, int topk){
        vector<KV_pair>* ranks = new vector<KV_pair>;
        for (int i = 0; i < BUCKET_NUM; i++){
            for (int j = 0; j < slot_num; j++){
                if (buckets[i].counters[j] != 0){
                    //ranks->push_back(KV_pair(buckets[i].items[j], Unbiased_Query(buckets[i].items[j])));
                    ranks->push_back(KV_pair(buckets[i].items[j], Unbiased_Query(buckets[i].items[j])));
                }
            }
        }
        sort(ranks->begin(), ranks->end());

        int tmp_n = ranks->size();
        result.clear();
        for (int i = 0; i < min(topk, tmp_n); i++){
            result[(*ranks)[i].key] = (*ranks)[i].value;
        }
        delete(ranks);
    }

    void Query_Topk_Biased(unordered_map<int, int>& result, int topk){
        vector<KV_pair>* ranks = new vector<KV_pair>;
        for (int i = 0; i < BUCKET_NUM; i++){
            for (int j = 0; j < slot_num; j++){
                if (buckets[i].counters[j] != 0){
                    ranks->push_back(KV_pair(buckets[i].items[j], Query(buckets[i].items[j])));
                }
            }
        }
        sort(ranks->begin(), ranks->end());

        int tmp_n = ranks->size();
        result.clear();
        for (int i = 0; i < min(topk, tmp_n); i++){
            result[(*ranks)[i].key] = (*ranks)[i].value;
        }
        delete(ranks);
    }

    Bucket* buckets;
    const uint32_t BUCKET_NUM;

};

#endif