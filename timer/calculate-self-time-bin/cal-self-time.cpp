
#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>




int main(int argc, char** argv)
{
    if (argc != 3) {
        std::cout << "ERROR: args must be 3" << std::endl;
        exit(1);
    }

    // std::cout << argc << std::endl;
    std::string srcFile {argv[1]};
    std::string caledSelfTime {argv[2]};


    std::fstream fin;
    fin.open(srcFile, std::ios::in);
    if (!fin.is_open()) {
        std::cerr << "open file " << srcFile << "Error" << std::endl;
        exit(1);
    }

    std::string s1, s2;
    int times;
    using LL = unsigned long long;
    LL acctime, shelltime;
    std::unordered_map<std::string, LL> self_func_runtime, son_func_runtime;

    // 文件头不处理
    int fileHeadItemNum = 5;
    while (fileHeadItemNum--) {
        fin >> s1;
    }
    // 聚合 自身运行时间，聚合 孩子运行时间
    while(fin >> s1 >>  s2 >>  times >>  acctime >>  shelltime) {
        if (self_func_runtime.find(s1) == self_func_runtime.end()) {
            self_func_runtime.emplace(s1, acctime);
        } else {
            self_func_runtime[s1] += acctime;
        }

        if (son_func_runtime.find(s2) == son_func_runtime.end()) {
            son_func_runtime.emplace(s2, shelltime);
        } else {
            son_func_runtime[s2] += shelltime;
        }
    }
    fin.close();

    // 函数运行时间 = 自身运行时间 - 孩子运行时间
    for (const auto& items : self_func_runtime) {
        if (son_func_runtime.find(items.first) != son_func_runtime.end()) { // 如果这个函数有 子函数
            self_func_runtime[items.first] -= son_func_runtime[items.first];
        }
    }


    // 处理结果写入文件
    std::fstream fout;
    fout.open(caledSelfTime, std::ios::out | std::ios::trunc);
    if (!fout.is_open()) {
        std::cerr << "open file " << caledSelfTime << "Error" << std::endl;
        exit(1);
    }

    LL all_time = 0;
    for (const auto& items : self_func_runtime) {
        all_time += items.second;
        fout << items.first << " " << items.second << std::endl;
    }

    fout << "This Module Run Total time is :  "  << all_time << std::endl;

    fout.close();

    return 0;
}