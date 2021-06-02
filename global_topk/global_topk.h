#ifndef GLOBAL_TOPK_H
#define GLOBAL_TOPK_H

#include <unordered_map>
#include <algorithm>
#include <vector>
#include "data.h"
using namespace std;

class Global_Topk_Set{
public:
    int topk_cnt;
    vector<flow_item>topk_pairs;
    Global_Topk_Set () {}

    Global_Topk_Set (int _topk_cnt){
        topk_pairs.clear();
        topk_cnt = _topk_cnt;
    }

    Global_Topk_Set& operator = (const Global_Topk_Set& other){
        topk_cnt = other.topk_cnt;
        topk_pairs.clear();
        for (int i = 0; i < topk_cnt; i++){
            topk_pairs.push_back(other.topk_pairs[i]);
        }
        return *this;
    }

    Global_Topk_Set (flow_item* init_pairs, int _topk_cnt){
        topk_cnt = _topk_cnt;
        topk_pairs.clear();
        for (int i = 0; i < topk_cnt; i++){
            topk_pairs.push_back(init_pairs[i]);
        }
        sort(topk_pairs.begin(), topk_pairs.end());
    }

    void merge(Global_Topk_Set& other){
        for (int i = 0; i < other.topk_pairs.size(); i++){
            topk_pairs.push_back(other.topk_pairs[i]);
        }
        sort(topk_pairs.begin(), topk_pairs.end());
        if (topk_pairs.size() > topk_cnt){
            int del_cnt = topk_pairs.size() - topk_cnt;
            for (int i = 0; i < del_cnt; i++){
                topk_pairs.pop_back();
            }
        }
    }

    void query(unordered_map<int, int>& result){
        result.clear();
        for (int i = 0; i < topk_pairs.size(); i++){
            if (result[topk_pairs[i].hash_value]){
                printf("no %d\n", topk_pairs[i].hash_value);
            }
            result[topk_pairs[i].hash_value] = topk_pairs[i].cnt | ((1 << 20) * topk_pairs[i].sketch_id);
        }
    }
};
#endif