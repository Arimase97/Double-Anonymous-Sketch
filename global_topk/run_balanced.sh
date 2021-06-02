make clean
make

cd data_proc/
g++ -O3 -o data_proc data_proc.cpp -std=c++11
g++ -O3 -o data_proc2 data_proc2.cpp -std=c++11
cd ..

for test_case in 0.1
do
    cd data_proc/
    ./data_proc2 $test_case
    cd ..
    ./global_topk_USS.out config.txt
    ./global_topk_SS.out config.txt
    ./global_topk_waving.out config.txt
    ./global_topk_framework.out config.txt
    ./global_topk_frequent.out config.txt
    done
done