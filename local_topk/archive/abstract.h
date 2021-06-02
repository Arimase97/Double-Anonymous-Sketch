#ifndef ABSTRACT_H
#define ABSTRACT_H

#include "hash.h"
#include "../data.h"
#include <algorithm>
#include <string>
#include <random>
#include <unordered_map>
#include <climits>
#include <cstring>
#include <iostream>

#define MAX(a, b) (a > b? a:b)
#define MIN(a, b) (a < b? a:b)

#define hash_ hash32

typedef uint32_t data_type; //note: according to your dataset
typedef int32_t count_type; //note: according to your dataset
typedef std::unordered_map<data_type, count_type> HashMap;

inline uint64_t hash64(data_type item, uint32_t seed = 0){
    return Hash::BOBHash64((uint8_t*)&item, sizeof(data_type), seed);
}

inline uint32_t hash32(data_type item, uint32_t seed = 0){
    return Hash::BOBHash64((uint8_t*)&item, sizeof(data_type), seed);
}

static std::random_device rd;
static const count_type COUNT[2] = {1, -1};


count_type Get_Median(count_type result[], uint32_t length){
    std::sort(result, result + length);
    return (length & 1) ? result[length >> 1] :
           (result[length >> 1] + result[(length >> 1) - 1]) / 2;
}

class Abstract{
public:
    char* name;

    Abstract(char* _name):name(_name){};
    virtual ~Abstract(){};

    virtual void Insert(const data_type item) = 0;
    virtual count_type Query(const data_type item) = 0;

    double Check(flow_item* item, int topk, int total){
        int HIT = item[topk - 1].cnt;

        count_type value = 0, all = 0, hit = 0, size = 0;
        double aae = 0, are = 0, cr = 0, pr = 0, f1 = 0;
        for(int i = 0; i < total; i++){
            value = Query(item[i].hash_value);
            if(item[i].cnt > HIT){
                all++;
                if(value > HIT){
                    hit += 1;
                    aae += abs(item[i].cnt - value);
                    are += abs(item[i].cnt - value) / (double)item[i].cnt;
                }
            }
            if(value > HIT)
                size += 1;
        }

        aae /= hit; are /= hit; cr = hit / (double)all;
        pr = hit / (double)size;
        f1 = 2 * pr * cr / (pr + cr);

        return pr;
    }

    void Check(HashMap mp, count_type HIT){
        HashMap::iterator it;
        count_type value = 0, all = 0, hit = 0, size = 0;
        double aae = 0, are = 0, cr = 0, pr = 0, f1 = 0;
        for(it = mp.begin();it != mp.end();++it){
            value = Query(it->first);
            if(it->second > HIT){
                all++;
                if(value > HIT){
                    hit += 1;
                    aae += abs(it->second - value);
                    are += abs(it->second - value) / (double)it->second;
                }
            }
            if(value > HIT)
                size += 1;
        }

        aae /= hit; are /= hit; cr = hit / (double)all;
        pr = hit / (double)size;
        f1 = 2 * pr * cr / (pr + cr);

        printf("%s: %d\nAAE: %lf\nARE: %lf\nCR: %lf\nPR: %lf\nF1: %lf\n", name, all, aae, are, cr, pr, f1);
    }
};

#endif // ABSTRACT_H
