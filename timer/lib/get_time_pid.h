#ifndef __get_pid_time_h__
#define __get_pid_time_h__

#include <string.h>
#include <unistd.h>
#include <time.h>

#define BUF_SIZE (1024)


const char* get_pid();
const char* times_stamp();
void get_self_file_name(char* buf);

char __timestamp__[BUF_SIZE] = {0};
char __pid__[BUF_SIZE] = {0};


const char* get_pid() {
	pid_t current_id;
	current_id = getpid();
    memset(__pid__, 0, sizeof(__pid__));
	sprintf(__pid__, "%d", current_id);
	return __pid__;
}

const char* times_stamp()
{
    time_t ticks = time(NULL);
    struct tm* ptm = localtime(&ticks);
    memset(__timestamp__, 0, sizeof(__timestamp__));
    strftime(__timestamp__, sizeof(__timestamp__), "%Y-%m-%d-%H-%M-%S", ptm);
    return __timestamp__;
}

// 因为适配区分模块采样，数据结构改变，导致以下接口发生改变，不可用
// void get_file_name(char** buf, const char*file_path, const char* file_postfix)
// {
//     memset(buf, 0, sizeof(buf));
//     strcat(buf, file_path);
//     strcat(buf, times_stamp());
//     strcat(buf, "_"); 
//     strcat(buf, get_pid());
//     strcat(buf, "_");
//     strcat(buf, file_postfix);
// }

// 这个参数传进来modules
// int module_num = sizeof(modules) / sizeof(const char*);
// module 就是1了？？？，奇怪
// 搞不懂，先采用extern的方式使用
// extern char** modules;
extern int module_num;

// const char* modules[] = {  "MODULE_CPL", "MODULE_LND", "MODULE_ROF", "MODULE_ICE", "MODULE_ATM", "MODULE_OCN",
// 						    "MODULE_GLC", "MODULE_WAV", "MODULE_CPLLND", "MODULE_CPLROF", "MODULE_CPLICE",
// 						    "MODULE_CPLATM", "MODULE_CPLOCN", "MODULE_CPLGLC", "MODULE_CPLWAV", "MODULE_UNDEFINE"
// 						};
// int module_num = sizeof(modules) / sizeof(const char*);

// 根据文件路径， 文件后缀， 不同模块，产生多个文件名
void get_file_name(char** buf, const char*file_path, const char* file_postfix, const char** modules)
// void get_file_name(char** buf, const char*file_path, const char* file_postfix)
{
    int i;
    // int module_num = sizeof(*modules) / sizeof(const char*);
    printf("module_num %d\n", module_num);
    for (i = 0; i < module_num; ++i) {
        memset(buf[i], 0, sizeof(buf[i]));
        strcat(buf[i], file_path);
        strcat(buf[i], times_stamp());
        strcat(buf[i], "_"); 
        strcat(buf[i], get_pid());
        strcat(buf[i], "_");
        strcat(buf[i], file_postfix);
        strcat(buf[i], ".");
        strcat(buf[i], modules[i]);
        // strcat(buf[i], ")");
    }
}

void init_diff_module_filenames(char*** filenames, int nums, int filename_buf_size)
{
    *filenames = (char**)malloc(sizeof(char*) * nums);
    int i;
    for (i = 0; i < nums; ++i) {
        (*filenames)[i] = (char*)malloc(sizeof(char) * filename_buf_size);
        memset((*filenames)[i], 0, sizeof((*filenames)[i]));
    }
}
void free_diff_module_filename_mems(char** filenames, int nums)
{
    int i;
    for (i = 0; i < nums; ++i) {
        if (filenames[i] != NULL) {
            free(filenames[i]);
        }
    }
    if (filenames != NULL) {
        free(filenames);
    }
}

void show_diff_module_filenames(char** filenames, int nums)
{
    int i;
    for (i = 0; i < nums; ++i) {
        printf("filename %d: %s\n", i, filenames[i]);
    }
}
void show_diff_modules(const char** modules, int nums)
{
    int i;
    for (i = 0; i < nums; ++i) {
        printf("modules %d: %s\n", i, modules[i]);
    }
}

#endif