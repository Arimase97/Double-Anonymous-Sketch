#include <bits/stdc++.h>
#include "../BOBHash32.h"
using namespace std;

string inp_stream[30001000];
unordered_map<string, int>realAns;
BOBHash32* hash_fun;

struct Flow_Item{
    string id;
    int freq;
    int real_freq;

    bool operator < (const Flow_Item& other) const {
        return freq > other.freq;
    }
}flow_item[10000100];

int load_data_CAIDA(int max_item){
    FILE* fin = fopen("../../data/caida.dat", "r");
    char buf[100] = {0};
    int i, j;
    for (i = 0; i < max_item; i++)
    {
        if (fread(buf, 1, 21, fin) < 21){
            break;
        }
        char temp1[15];
        strncpy(temp1, buf, 13);
		double time_stamp = *(double*)(buf + 13);
		temp1[13] = 0;
        inp_stream[i] = "";
		for (int j = 0; j < 13; j++){
			temp1[j] = buf[j];
			if (temp1[j] == 0){
				temp1[j] = 'a';
			}
			inp_stream[i] += temp1[j];
		}
        string s1 = temp1;
		realAns[s1] += 1;
    }
    printf("i: %d\n", i);
    fclose(fin);
    return i;
}

int load_data_syn(int max_item, int dataset_id){
    char file_name[100];
    sprintf(file_name, "../../data/%03d.dat", dataset_id);
    FILE* fin = fopen(file_name, "r");
    char buf[100] = {0};
    int i, j;
    for (i = 0; i < max_item; i++)
    {
        if (fread(buf, 1, 4, fin) < 4){
            break;
        }
        char temp1[15];
        strncpy(temp1, buf, 4);
		temp1[13] = 0;
		for (int j = 0; j < 13; j++){
			temp1[j] = buf[j];
			if (temp1[j] == 0){
				temp1[j] = 'a';
			}
			inp_stream[i][j] = temp1[j];
		}
		inp_stream[i][13] = inp_stream[i][14] = 0;
        string s1 = temp1;
		realAns[s1] += 1;
    }
    printf("i: %d\n", i);
    fclose(fin);
    return i;
}

int load_data_webpage(int max_item){
    FILE* fin = fopen("../../data/webdocs_form00.dat", "r");
    char buf[100] = {0};
    int i, j;
    for (i = 0; i < max_item; i++)
    {
        if (fread(buf, 1, 4, fin) < 4){
            break;
        }
        if (i == 10001000){
            i++;
            i--;
        }
        char temp1[15];
        memset(temp1, 0, sizeof(temp1));
        strncpy(temp1, buf, 4);
		temp1[13] = 0;
        if ((buf[0] == 0) && (buf[1] == 0) && (buf[2] == 0) && (buf[3] == 0)){
            i--;
            continue;
        }
		for (int j = 0; j < 13; j++){
			temp1[j] = buf[j];
			if (temp1[j] == 0){
				temp1[j] = 'a';
			}
			inp_stream[i][j] = temp1[j];
		}
		inp_stream[i][13] = inp_stream[i][14] = 0;
        string s1 = temp1;
		realAns[s1] += 1;
    }
    printf("i: %d\n", i);
    fclose(fin);
    return i;
}

int load_data_NET(int max_item){
    FILE* fin = fopen("../../data/net.dat", "r");
    char buf[100] = {0};
    int i, j;
    for (i = 0; i < max_item; i++)
    {
        if (fread(buf, 1, 4, fin) < 4){
            break;
        }
        char temp1[15];
        memset(temp1, 0, sizeof(temp1));
        strncpy(temp1, buf, 4);
		temp1[13] = 0;
		for (int j = 0; j < 13; j++){
			temp1[j] = buf[j];
			if (temp1[j] == 0){
				temp1[j] = 'a';
			}
			inp_stream[i][j] = temp1[j];
		}
		inp_stream[i][13] = inp_stream[i][14] = 0;
        string s1 = temp1;
		realAns[s1] += 1;
    }
    printf("i: %d\n", i);
    fclose(fin);
    return i;
}


// CAIDA
unordered_map<string, int> cur_ok;
unordered_map<string, int> out_file_no_map;
int main(int argc, char* argv[]){
    double heavy_ratio = stod(string(argv[1]));
    srand(time(NULL));
    hash_fun = new BOBHash32(1);

    for (int file_no = 1; file_no < 2; file_no++){
        printf("%d\n", file_no);
        char file_name[110];

        realAns.clear();
        int tot_obj = 0;
        tot_obj = load_data_syn(20000000, 9);
        
        int tot_item = 0;
        int tot_obj2 = 0;
        unordered_map<string, int>::iterator it;

        for (it = realAns.begin(); it != realAns.end(); it++){
            flow_item[tot_item].id = it->first;
            flow_item[tot_item].freq = it->second;
            flow_item[tot_item].real_freq = it->second;
            tot_item++;
            tot_obj2 += flow_item[tot_item - 1].real_freq;
        }
        printf("%d\n", tot_obj2);
        sort(flow_item, flow_item + tot_item);
        reverse(flow_item, flow_item + tot_item);

        random_shuffle(flow_item, flow_item + tot_item - 1);

        printf("%d\n", tot_item);

        FILE* fout[100];
        for (int i = 0; i < 100; i++){
            char fout_name[100];
            //sprintf(fout_name, "../../data/%02d.dat", i + file_no * 10);
            sprintf(fout_name, "../../data_distributed/99%02d.dat", i);
            fout[i] = fopen(fout_name, "w");
        }
        out_file_no_map.clear();

        int cur_pos = 0;
        for (int i = 0; i < 100; i++){
            int cur_size;
            if (i == 0){
                cur_size = tot_obj * heavy_ratio;
            }
            else{
                cur_size = tot_obj * (1 - heavy_ratio) / 99.0;
            }

            printf("%d %lf\n", i, (double)cur_size);
            int cur_cnt = 0;
            cur_ok.clear();
            while ((cur_cnt <= cur_size) && (cur_pos < tot_item)){
                cur_ok[flow_item[cur_pos].id] = 1;
                cur_cnt += flow_item[cur_pos].real_freq;
                out_file_no_map[flow_item[cur_pos].id] = i;
                cur_pos++;
            }
            printf("%d\n", cur_cnt);
        }
            
            

        char tmp[20];
        for (int j = 0; j < tot_obj; j++){
            string cur_string = "";
            for (int k = 0; k < 13; k++){
                tmp[k] = char(inp_stream[j][k] + k + 5 * file_no);
                cur_string += inp_stream[j][k];
            }
            fwrite(tmp, 1, 13, fout[out_file_no_map[cur_string]]);
        }
    }

    return 0;
}
