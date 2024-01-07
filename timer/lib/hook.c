#include "hash_table.h"
#include "mem_pool.h"   // 操操操操操操操操，艹艹艹艹艹艹艹艹， 不包含这个文件，在静态库的场景会crash
#include "perf_counter.h"
#include <stdio.h>
#include <limits.h>

#include "stack.h"
#include "perf_counter.h"
#include "get_time_pid.h"


const char* modules[] = {  "MODULE_CPL", "MODULE_LND", "MODULE_ROF", "MODULE_ICE", "MODULE_ATM", "MODULE_OCN",
						    "MODULE_GLC", "MODULE_WAV", "MODULE_CPLLND", "MODULE_CPLROF", "MODULE_CPLICE",
						    "MODULE_CPLATM", "MODULE_CPLOCN", "MODULE_CPLGLC", "MODULE_CPLWAV", "MODULE_UNDEFINE"
						};
int module_num = sizeof(modules) / sizeof(const char*);

HASH_MAP hashmap;
bool trace_flag = false;
ULL depth = 0;
int depth_is_zero_num = 0;
ULL max_call_depth = 0;

Stack time_stk;                         // 记录时间栈
Stack func_addr_stk;                    // 记录函数地址栈
Stack module_stk;                       // 记录模块栈

// 这个定义实在动态库
// 然后在 静态库里面extern，就可以改变这个值了？？？、好奇怪（按道理应该是不行的啊，但是效果是可以的）
// 难道确实是有这个优化嘛
int __profile__rank = INT_MAX;        //记录当前处在哪个rank中（多进程）
int __profile_common_size = INT_MAX;  // record  mpi size

ULL call_start_probe_times = 0;
ULL call_end_probe_times = 0;

void get_basic_info()
{
    if (depth == 0) {
        depth_is_zero_num++;
    }
    if (depth > max_call_depth) {
        max_call_depth = depth;
    }
}

void push_moduleid_(int *moduId);
void pop_moduleid_();
    
void __attribute__((constructor)) traceBegin(void); // 这东西不太适合对C++插桩
// 因为，这个函数并不是限制性的，C++有可能是限制属性alloc，这个也是会被插桩的
// 对于C的目标文件是先执行的

// 所以为了统一起见，还是不要用这个东西了
// 可以使用全局变量，depth
// depth == 0
// 在__cyg_profile_func_enter()中表示刚开始
// 在__cyg_profile_func_exit() 表示结束

void __attribute__((destructor)) traceEnd(void);

void __cyg_profile_func_enter(void *func, void *caller);

void __cyg_profile_func_exit(void *func, void *caller);


void push_moduleid_(int *module_id)
{
    stack_push(&module_stk, *module_id);
    if (__profile__rank == INT_MAX || __profile__rank == 0) {
        printf("[%s]][in mpi rank %lld][in module %s]\n", times_stamp(), __profile__rank, modules[get_stack_top_data(&module_stk)]);
        // printf("[in mpi rank %lld][push module %d]\n", __profile__rank, *module_id);
        // printf("[in mpi rank %lld][module stack top value is %d]\n", __profile__rank, get_stack_top_data(&module_stk));
    }
}
void pop_moduleid_()
{
    if (__profile__rank == INT_MAX || __profile__rank == 0) {
        // printf("[in mpi rank %lld][pop module]\n", __profile__rank);
    }
    stack_pop(&module_stk);
}
 
// 这样的操作，就是只对constructor destructor之间的代码采样
void __attribute__((constructor, no_instrument_function)) traceBegin(void) {
    trace_flag = true;
    printf("__traceBegin__\n");
    hash_map_init(&hashmap);
    stack_init(&time_stk);
    stack_init(&func_addr_stk);
    stack_init(&module_stk);
    stack_push(&module_stk, module_num - 1); // 预先加入一个module(MODULE_UNDEFINE)，避免moduleStack为0，访问top()的时候，会crash
}

const char* outfile_path = "./out/";
const char* outfile_postfix = "factrace.txt";
char** outfilenames = NULL;

void __attribute__((destructor, no_instrument_function)) traceEnd(void) {
    trace_flag = false;

    if (__profile__rank == INT_MAX || __profile__rank == 0) {
        init_diff_module_filenames(&outfilenames, module_num, BUF_SIZE);
        get_file_name(outfilenames, outfile_path, outfile_postfix, modules);
        // get_file_name(outfilenames, outfile_path, outfile_postfix);
        // show_diff_module_filenames(outfilenames, module_num);
        save_hash_map_node_to_file(&hashmap, outfilenames);
        // show_diff_modules(modules, 4);
    
        printf("*************************TraceEnd  Basic Info*************************\n");
        hash_map_show(&hashmap);
        // show_hash_map_node(&hashmap);
        printf("[basic info]: depth_is_zero_num: \t%d\n", depth_is_zero_num);
        printf("[basic info]: max_call_depth: \t\t%lld\n", max_call_depth);
        printf("[basic info]: call_start_probe_times: \t%lld\n", call_start_probe_times);
        printf("[basic info]: call_end_probe_times: \t%lld\n", call_end_probe_times);
        printf("[basic info]: mpi rank: \t\t%d\n", __profile__rank);
        printf("[basic info]: mpi common size: \t\t%d\n", __profile_common_size);
        // printf("[basic info]: sampling out file: \t%s\n", outfilename);
        printf("[basic info]: __traceEnd__\n");
        printf("***********************************************************************\n");

        printf("[basic info]: out file path:\n ");
        show_diff_module_filenames(outfilenames, module_num);
    }
    
    // free_diff_module_filename_mems(outfilenames, module_num); // 这个行代码，在模式中，会crash; 自测没关系？？？
    hash_map_destory(&hashmap);
}

// 在探针里面的变量 ，用全局变量，降低访存开销， __cyg_profile_func_enter
NODE_Ptr nodeptr = NULL; // 使用全局变量，降低访存开销
ULL print_flag = 1;
NODE tmp;

ULL shell_start_time = 0;
ULL shell_end_time = 0;
ULL self_start_time = 0;
ULL self_end_time = 0;

// 自身地址肯定是准的
// caller 是 指令内部，无法解析
bool first_call_cyg_enter = true;
ULL god_addr = 0;
ULL self_addr = 0;
ULL fater_addr = 0;


// 这里面的实现，不要有局部变量（即使有一个，每次都要访存，这个探针可能是上亿次调用，累加起来，就很大）
void __cyg_profile_func_enter(void *func, void *caller) {
    // if (trace_flag == false) return;

    stack_push(&func_addr_stk, (ULL)func);


    // insert time prob
    shell_start_time = perf_counter();
    stack_push(&time_stk, shell_start_time);

    get_basic_info();

    depth++;
    call_start_probe_times++;
    // if (print_flag % 100000 == 0) hash_map_show(&hashmap);
    // print_flag++;

    
    // insert time prob
    self_start_time = perf_counter();
    stack_push(&time_stk, self_start_time);

}

ULL current_module = 0;

void __cyg_profile_func_exit(void *func, void *caller) {
    // if (trace_flag == false) return;

    self_end_time = perf_counter();

    // 取时间栈里面的时间
    self_start_time  = get_stack_top_data(&time_stk);
    stack_pop(&time_stk);
    shell_start_time = get_stack_top_data(&time_stk);
    stack_pop(&time_stk);

    // 取函数地址栈 （self_addr, fater_addr）
    self_addr = get_stack_top_data(&func_addr_stk);     // 取自己的函数地址；（栈顶 地址，就是自身地址）
    stack_pop(&func_addr_stk);
    if(is_stack_empty(&func_addr_stk) == true) {        // 取父函数地址；（如果此时函数栈空，说明函数栈里面没有他父亲函数的地址，我们使用caller作为父函数地址）
        fater_addr = (ULL)caller;
    } else {
        fater_addr = get_stack_top_data(&func_addr_stk);
    }
    // 取模块数栈
    current_module = get_stack_top_data(&module_stk);

    // 构造父子调用关系节点，查找是否已经插入hashmap中，如果已经插入，更新节点的值，如果没有插入，插入新节点
    // make_node(&tmp, self_addr, fater_addr, 1, 666666, 666666);
    set_node_key(&tmp, self_addr, fater_addr);
    nodeptr = hash_map_find_key(&hashmap, &tmp.key);
    if(nodeptr != NULL) {
        // update old node
        // nodeptr->val.times[current_module]++;
        // nodeptr->val.acctime[current_module] += self_end_time - self_start_time;
        // nodeptr->val.sheltime[current_module] += perf_counter() - shell_start_time;
        update_node_val_by_module(nodeptr, 1, self_end_time - self_start_time, perf_counter() - shell_start_time, current_module);
    } else {
        // inter new node
        nodeptr = get_node(&hashmap.nodeMemPool);
        // make_node(nodeptr, self_addr, fater_addr, 1, self_end_time - self_start_time, perf_counter() - shell_start_time);
        // nodeptr->val.times[current_module]++;
        // nodeptr->val.acctime[current_module] += self_end_time - self_start_time;
        // nodeptr->val.sheltime[current_module] += perf_counter() - shell_start_time;
        set_node_key(nodeptr, self_addr, fater_addr);
        set_node_val_by_module(nodeptr, 1, self_end_time - self_start_time, perf_counter() - shell_start_time, current_module);
        hash_map_insert(&hashmap, nodeptr);
    }

    depth--;
    call_end_probe_times++;

}