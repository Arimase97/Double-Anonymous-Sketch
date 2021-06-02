#ifndef SS_USS_H
#define SS_USS_H

#include "StreamSummary.h"
#include <algorithm>
using namespace std;

class USS : public Abstract{
public:

    USS(uint32_t _SIZE):Abstract((char *)"USS"){
        summary = new StreamSummary(_SIZE);
    }
    ~USS(){
        delete summary;
    }

    void Insert(const data_type item){
        if(!summary->Add_Data(item)){
            if(summary->isFull()){
                if(rd() % (summary->Min_Num() + 1) == 0){
                    summary->Add_Counter(item, false);
                }
                else{
                    summary->Add_Min();
                }
            }
            else{
                summary->Add_Counter(item, true);
            }
        }
    }

    count_type Query(const data_type item){
        return summary->Query(item);
    }

    void query_init(flow_item* item, int topk, int total){
        int* bin = new int[3000100];
        int tot_bin = 0;
        for (int i = 0; i < total; i++){
            if (Query(item[i].hash_value)){
                bin[tot_bin++] = Query(item[i].hash_value);
            }
        }
        topk = min(topk, tot_bin);
        sort(bin, bin + tot_bin);
        reverse(bin, bin + tot_bin);
        threshold = bin[topk - 1];

        int rem = 0;
        for (int i = 0; i < topk; i++){
            if (bin[i] == threshold){
                rem++;
            }
        }
        static int tmp_idx[5000100];
        for (int i = 0; i < total; i++){
            tmp_idx[i] = i;
        }
        random_shuffle(tmp_idx, tmp_idx + total);
        for (int i = 0; i < total && rem; i++){
            if (Query(item[tmp_idx[i]].hash_value) == threshold){
                Insert(item[tmp_idx[i]].hash_value);
                rem--;
            }
        }
        
        delete(bin);
    }

    void report_topk(flow_item* item, int topk, int total, unordered_map<int, int>& result){
        result.clear();

        int* bin = new int[3000100];
        int tot_bin = 0;
        for (int i = 0; i < total; i++){
            if (Query(item[i].hash_value)){
                bin[tot_bin++] = Query(item[i].hash_value);
            }
        }
        sort(bin, bin + tot_bin);
        reverse(bin, bin + tot_bin);
        topk = min(topk, tot_bin);

        threshold = bin[topk - 1];

        int rem = 0;
        for (int i = 0; i < topk; i++){
            if (bin[i] == threshold){
                rem++;
            }
        }
        static int tmp_idx[5000100];
        for (int i = 0; i < total; i++){
            tmp_idx[i] = i;
        }
        random_shuffle(tmp_idx, tmp_idx + total);
        for (int i = 0; i < total && rem; i++){
            if (Query(item[tmp_idx[i]].hash_value) == threshold){
                Insert(item[tmp_idx[i]].hash_value);
                rem--;
            }
        }

        delete(bin);

        for (int i = 0; i < total; i++){
            if (Query(item[i].hash_value) > threshold){
                result[item[i].hash_value] = Query(item[i].hash_value);
            }
        }
    }

    int query_topk(const data_type item){
        int res = summary->Query(item);
        if (res > threshold){
            return res;
        }
        return 0;
    }

private:
    StreamSummary* summary;
    int threshold;
};

#endif //SS_USS_H
