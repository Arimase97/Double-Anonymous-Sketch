make clean
make

cd data_proc/
g++ -O3 -o data_proc data_proc.cpp -std=c++11
g++ -O3 -o data_proc2 data_proc2.cpp -std=c++11
cd ..

for test_case in 0.5 0.4 0.3 0.2 0.1 0.05 0.03 0.02 0.01
do
    cd data_proc/
    ./data_proc $test_case
    cd ..
    ./global_topk_USS.out config_uss.txt
    ./global_topk_SS.out config_ss.txt
    ./global_topk_waving.out config_waving.txt
    ./global_topk_framework.out config_ours.txt
    ./global_topk_frequent.out config_freq.txt
    done
done
