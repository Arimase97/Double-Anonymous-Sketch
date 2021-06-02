#include "global_topk.h"
#include "data.h"
#include "archive/WavingSketch.h"
#include "util.h"
#include <map>
using namespace std;

map<int, int>mp;

void test_global_topk(int memory, int distrib_cnt, int topk, int heavy_percent, int blen, char* inp_path, FILE* fout){
    char sketch_name[100];
    int i, j;
    unordered_map<int, int>::iterator it;
    
    sprintf(sketch_name, "Waving_sketch");
    fprintf(fout, "%s:\n", sketch_name);
    printf("%s:\n", sketch_name);

    double avg_ARE = 0, avg_F1 = 0, avg_AAE = 0;

    int cdf_distribution[1001];
    memset(cdf_distribution, 0, sizeof(cdf_distribution));

    for (int tt = 0; tt < REP_TIME; tt++){
        Global_Topk_Set* store_predicted[200];

        Global_Topk_Set* ground_truth = new Global_Topk_Set(topk);
        Global_Topk_Set* predicted = new Global_Topk_Set(topk);

        Global_Topk_Set* ground_truth_predicted = new Global_Topk_Set(topk);

        int hit_distribution[200];
        memset(hit_distribution, 0, sizeof(hit_distribution));

        for (i = 0; i < distrib_cnt; i++){
            if (i == 99){
                i++;
                i--;
            }
        // for (i = 0; i < 10; i++){
            printf("%d ", i);
            char file_name[100];
            sprintf(file_name, "%s%02d%02d.dat", inp_path, distrib_cnt - 1, i);
            
            
            int tot_item = load_data(INF, file_name);
            printf("%d %d\n", tot_item, flow_cnt);

            WavingSketch<8>* sketch = new WavingSketch<8>(memory / 68);

            for (j = 0; j < tot_item; j++){
                sketch->Insert(hashed_value[j]);
            }

            Global_Topk_Set* cur_truth = new Global_Topk_Set(flow, min(topk, flow_cnt));
            ground_truth->merge(*cur_truth);

            unordered_map<int, int>* result_topk_map = new unordered_map<int, int>;
            result_topk_map->clear();
            sketch->Query_Topk_Biased(*result_topk_map, topk);
            flow_item* result_topk = new flow_item[5000];
            
            for (j = 0; j < topk; j++){
                result_topk[j].cnt = 0;
                result_topk[j].hash_value = j + 1;
            }

            for (it = result_topk_map->begin(), j = 0; it != result_topk_map->end(); it++, j++){
                result_topk[j].cnt = it->second;
                result_topk[j].hash_value = it->first;
                result_topk[j].sketch_id = i;
                mp[it->first] = sketch->Unbiased_Query(it->first);
            }


            Global_Topk_Set* cur_predict = new Global_Topk_Set(result_topk, topk);

            for (it = result_topk_map->begin(), j = 0; it != result_topk_map->end(); it++, j++){
                result_topk[j].cnt = realAns_hashed[it->first];
                result_topk[j].hash_value = it->first;
                result_topk[j].sketch_id = i;
            }

            Global_Topk_Set* cur_ground_truth_predicted = new Global_Topk_Set(result_topk, topk);

            //for (j = 0; j < cur_predict->topk_pairs.size(); j++){
            //    if (cur_predict->topk_pairs[j].cnt){
                    //fprintf(fout, "%d %d\n", cur_predict->topk_pairs[j].cnt, cur_predict->topk_pairs[j].hash_value);
            //    }
            //}

            predicted->merge(*cur_predict);
            ground_truth_predicted->merge(*cur_ground_truth_predicted);

            store_predicted[i] = new Global_Topk_Set(topk);
            *(store_predicted[i]) = *cur_predict;

            double deviation = 0;
            for (j = 0; j < topk; j++){
                if ((*result_topk_map)[flow[j].hash_value] != 0){
                    deviation += (*result_topk_map)[flow[j].hash_value] - flow[j].cnt;
                }
            }
            printf("%f\n", deviation);

            delete[] result_topk;
            delete(cur_truth);
            delete(cur_predict);
            delete(sketch);
            delete(result_topk_map);
            delete(cur_ground_truth_predicted);
            printf("%d\n", ground_truth->topk_pairs.size());
        }

        int hit = 0;
        
        unordered_map<int, int>* ground_truth_map = new unordered_map<int, int>;
        ground_truth_map->clear();
        unordered_map<int, int>* predicted_map = new unordered_map<int, int>;
        predicted_map->clear();
        unordered_map<int, int>* ground_truth_predicted_map = new unordered_map<int, int>;

        ground_truth->query(*ground_truth_map);
        predicted->query(*predicted_map);
        ground_truth_predicted->query(*ground_truth_predicted_map);

        printf("%d\n", ground_truth->topk_pairs.size());
        
        int tot = 0;
        double ARE = 0, AAE = 0;
        for (it = ground_truth_map->begin(); it != ground_truth_map->end(); it++){
            int freq = mp[it->first];
            int sketch_id = (*predicted_map)[it->first] >> 20;

            hit += ((*predicted_map)[it->first] != 0);
            tot++;
            if (((*predicted_map)[it->first] != 0)){
                ARE += fabs(it->second - freq) / (double)it->second;
                AAE += fabs(it->second - freq);
                hit_distribution[sketch_id]++;
            }
        }
        //fprintf(fout, "F1 Score: %.9f, ARE: %.9f, %d %d\n", (double)hit / (double)tot, ARE / (double)hit, hit, tot);


        avg_F1 += (double)hit / (double)tot / (double)REP_TIME;
        avg_ARE += ARE / (double)hit / (double)REP_TIME;
        avg_AAE += AAE / (double)hit / (double)REP_TIME;

        
        for (i = 0; i < distrib_cnt; i++){
            fprintf(fout, "%d ", hit_distribution[i]);
        }
        fprintf(fout, "\n");

        

        for (i = 0; i < distrib_cnt; i++){
            predicted_map->clear();
            store_predicted[i]->query(*predicted_map);
            hit = 0;
            for (it = ground_truth_map->begin(); it != ground_truth_map->end(); it++){
                hit += ((*predicted_map)[it->first] != 0);
            }
            fprintf(fout, "%d ", hit);
        }
        fprintf(fout, "\n");
        
        
        

        bool flag = 0;
        for (i = 0; i < topk; i++){
            if (predicted->topk_pairs[i].hash_value == ground_truth->topk_pairs[0].hash_value){
                cdf_distribution[i]++;
                flag = 1;
                break;
            }
        }
        if (!flag){
            cdf_distribution[topk]++;
        }

        delete(ground_truth);
        delete(ground_truth_map);
        delete(predicted);
        delete(predicted_map);

        fflush(fout);
    }
    
    fprintf(fout, "F1 Score: %.9f, ARE: %.9f, AAE: %.9f\n", avg_F1, avg_ARE, avg_AAE);
    for (i = 0; i <= topk; i++){
        fprintf(fout, "%d ", cdf_distribution[i]);
    }
    fprintf(fout, "\n");
}

int main(int argc, char* argv[]){
    int n_case;
    char inp_path[100], output_file[100];
    srand(1);

    FILE* fin = fopen(argv[1], "r");
    fscanf(fin, "%d%s%s", &n_case, inp_path, output_file);

    FILE* fout = fopen(output_file, "a+");

    for (int t_case = 0; t_case < n_case; t_case++){
        //printf("Test cases: %d\n", t_case);
        int memory, distrib_cnt, topk, heavy_percent, blen;
        fscanf(fin, "%d%d%d%d%d", &memory, &distrib_cnt, &topk, &heavy_percent, &blen);

        test_global_topk(memory, distrib_cnt, topk, heavy_percent, blen, inp_path, fout);
        fflush(fout);
    }
    fclose(fin);
    fclose(fout);

    return 0;
}
