#include "hash_table.h"

#include "mem_pool.h"


#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <limits.h>

// #define __debug_hash_map__ 0

void hash_map_init(HASH_MAP* hashmap)
{
    ULL i, j;
    for (i = 0; i < HASH_TABLE_SIZE; ++i) {
        for (j = 0; j < HASH_TABLE_SIZE; ++j) {
            hashmap->node[i][j] = NULL;
        }
    }
    hashmap->uesNum = 0;
    hashmap->colisionNum = 0;
    hashmap->capacity = (ULL)HASH_TABLE_SIZE * (ULL)HASH_TABLE_SIZE;
    mem_pool_init(&hashmap->nodeMemPool);

}

void hash_map_show(HASH_MAP* hashmap)
{
    printf("----------------Hash MAP Profile--------------------------\n");
    printf("HASH_MAP:capacity\t\t\t %lld\n", hashmap->capacity);
    printf("HASH_MAP:uesNum\t\t\t\t %lld\n", hashmap->uesNum);
    printf("HASH_MAP:colisionNum\t\t\t %lld\n", hashmap->colisionNum);
    printf("----------------------------------------------------------\n");
    mem_pool_show_profile(&hashmap->nodeMemPool);
}


bool is_key_equeue(KEY key1, KEY key2)
{
    return (key1.funcAddr == key2.funcAddr) && (key1.faterAddr == key2.faterAddr);
}

// ULL hash(ULL key)
// {
//     unsigned char a = key       & 0x000000ff;
//     unsigned char b = key >> 8  & 0x000000ff;
//     unsigned char c = key >> 16 & 0x000000ff;
//     unsigned char d = key >> 24 & 0x000000ff;
//     unsigned char e = key >> 32 & 0x000000ff;
//     unsigned char f = key >> 40 & 0x000000ff;
//     unsigned char g = key >> 48 & 0x000000ff;
//     unsigned char h = key >> 56 & 0x000000ff;
//     ULL all = a * b + c + d + e + f + g + h;
    
//     // // 二次hash，效果不明显
//     // a = all       & 0x000000ff;
//     // b = all >> 8  & 0x000000ff;
//     // c = all >> 16 & 0x000000ff;
//     // d = all >> 24 & 0x000000ff;
//     // all += a * b + c + d;
//     return all % (ULL)HASH_TABLE_SIZE;
// }

// ----------------Hash MAP Profile--------------------------
// HASH_MAP:capacity                        262144
// HASH_MAP:uesNum                          76555
// HASH_MAP:colisionNum                     23445
// ----------------------------------------------------------

ULL hash(ULL key)
{
    // 平方取中
    ULL a = key & 0xffffffff;
    ULL b = key >> 32;
    ULL tmp = a * a + b * b;
    tmp = tmp & 0x0000ffffffff0000;
    tmp = tmp >> 16;
    tmp *= tmp;
    tmp = tmp & 0x000000ffff000000;
    tmp = tmp >> 24;
    return tmp % (ULL)HASH_TABLE_SIZE;
}

// ----------------Hash MAP Profile--------------------------
// HASH_MAP:capacity                        262144
// HASH_MAP:uesNum                          83224
// HASH_MAP:colisionNum                     16776
// ----------------------------------------------------------


// 这个接口的实现，需要自己考虑，是否全兼容 （// 如果存在这个节点，什么都不做）
// key存在，什么都不做;
// 需要自己调用update_node_val_by_module、set_node_val_by_module 以更新节点值
void hash_map_insert(HASH_MAP* hashmap, NODE* node)
{
    NODE_Ptr ptr = hash_map_find_key(hashmap, &node->key);
    // 找到，直接更新原来节点的值
    if (ptr != NULL) {
        // 如果存在这个节点，什么都不做
        // update_node(ptr, node); // 把node2的数据更新到node1
        #ifdef __debug_hash_map__
        printf("[hash_map_insert]: update node, no need inster\n");
        #endif
        return;
    }

    ULL row = hash(node->key.funcAddr);
    ULL col = hash(node->key.faterAddr);
    // 没找到，有两种情况，一、头节点为空; 二、发生碰撞
    #ifdef __debug_hash_map__
    printf("[hash_map_insert]: this node first inert to hash map\n");
    #endif
    if (hashmap->node[row][col] == NULL) { 
        hashmap->uesNum++;
        hashmap->node[row][col] = node;
    } else {
        NODE_Ptr ptr = hashmap->node[row][col];
        while(ptr->next != NULL) {
            ptr = ptr->next;
        }
        ptr->next = node;
        hashmap->colisionNum++;
    }
}

void hash_map_destory(HASH_MAP* hashmap)
{
    mem_poll_detory(&hashmap->nodeMemPool);
}

// find return ptr
// not find, return NULL
NODE_Ptr hash_map_find_node(HASH_MAP* hashmap, const NODE* node)
{
    ULL row = hash(node->key.funcAddr);
    ULL col = hash(node->key.faterAddr);
    if (hashmap->node[row][col] == NULL) {
        #ifdef __debug_hash_map__
        printf("[hash_map_find_node]: not Find this node\n");
        #endif
        return NULL;
    } else {
        NODE* p = hashmap->node[row][col];
        while(p != NULL) {
            if (!is_key_equeue(p->key, node->key)) {
                p = p->next;
                continue;
            } else {
                #ifdef __debug_hash_map__
                printf("[hash_map_find_node]: Find key\n");
                #endif
                break;
            }
        }
        return p;
    }
}

// find return ptr
// not find, return NULL
NODE_Ptr hash_map_find_key(HASH_MAP* hashmap, const KEY* key)
{
    ULL row = hash(key->funcAddr);
    ULL col = hash(key->faterAddr);
    if (hashmap->node[row][col] == NULL) {
        #ifdef __debug_hash_map__
        printf("[hash_map_find_key]: not Find this node\n");
        #endif
        return NULL;
    } else {
        NODE* p = hashmap->node[row][col];
        while(p != NULL) {
            if (!is_key_equeue(p->key, *key)) {
                p = p->next;
                continue;
            } else {
                #ifdef __debug_hash_map__
                printf("[hash_map_find_key]: Find key\n");
                #endif
                break;
            }
        }
        return p;
    }
}

// 因为适配区分模块采样，数据结构改变，导致以下接口发生改变，不可用
// void print_hash_map_node(HASH_MAP* hashmap)
// {
//     int i, j;
//     for (i = 0; i < HASH_TABLE_SIZE; ++i) {
//         for(j = 0; j < HASH_TABLE_SIZE; ++j) {
//             if (hashmap->node[i][j] == NULL) {
//                 continue;
//             }
//             NODE_Ptr first_node = hashmap->node[i][j];
//             while(first_node != NULL) {
//                 printf("funcAddr:%p \t", (void*)first_node->key.funcAddr);
//                 printf("faterAddr:%p \t", (void*)first_node->key.faterAddr);
//                 printf("times:%d \t", first_node->val.times);
//                 printf("acctime:%lld \t", (ULL)first_node->val.acctime);
//                 printf("sheltime:%lld \t", (ULL)first_node->val.sheltime);
//                 printf("\n");
//                 first_node = first_node->next;
//             }
//         }
//     }
// }

void show_hash_map_node(HASH_MAP* hashmap)
{
    int i, j;
    NODE_Ptr first_node;
    for (i = 0; i < HASH_TABLE_SIZE; ++i) {
        for(j = 0; j < HASH_TABLE_SIZE; ++j) {
            if (hashmap->node[i][j] == NULL) {
                continue;
            }
            first_node = hashmap->node[i][j];
            while(first_node != NULL) {
                show_node(first_node);
                first_node = first_node->next;
            }
        }
    }
}

// 因为适配区分模块采样，数据结构改变，导致以下接口发生改变，不可用
// void save_hash_map_node_to_file(HASH_MAP* hashmap, const char* filename)
// {
//     FILE *fw = NULL;
//     fw = fopen(filename, "w");
//     if (fw == NULL) {
//         printf("[ERROR]: open file %s error, when save hash map node to file\n", filename);
//         return;
//     }
//     fprintf(fw, "funcAddr\tfaterAddr\ttimes\tacctime\tsheltime\n");
//     int i, j;
//     for (i = 0; i < HASH_TABLE_SIZE; ++i) {
//         for(j = 0; j < HASH_TABLE_SIZE; ++j) {
//             if (hashmap->node[i][j] == NULL) {
//                 continue;
//             }
//             NODE_Ptr first_node = hashmap->node[i][j];
//             while(first_node != NULL) {
//                 fprintf(fw, "%p \t", (void*)first_node->key.funcAddr);
//                 fprintf(fw, "%p \t", (void*)first_node->key.faterAddr);
//                 fprintf(fw, "%d \t\t", first_node->val.times);
//                 fprintf(fw, "%lld \t\t", (ULL)first_node->val.acctime);
//                 fprintf(fw, "%lld \t", (ULL)first_node->val.sheltime);
//                 fprintf(fw, "\n");
//                 first_node = first_node->next;
//             }
//         }
//     }
//     fclose(fw);
// }


// to do 文件指针数组， 使用module区分文件名
extern char** modules;
extern int module_num;
void save_hash_map_node_to_file(HASH_MAP* hashmap, char** filename)
{
    int i, j, k;
    FILE *fw[module_num];
    memset(fw, 0, sizeof(fw));
    for (i = 0; i < module_num; ++i) {
        fw[i] = fopen(filename[i], "w");
        if (fw[i] == NULL) {
            printf("[ERROR]: open file %s error, when save hash map node to file\n", filename[i]);
            return;
        }
        fprintf(fw[i], "funcAddr\tfaterAddr\ttimes\tacctime\tsheltime\n");
    }

    for (i = 0; i < HASH_TABLE_SIZE; ++i) {
        for(j = 0; j < HASH_TABLE_SIZE; ++j) {
            if (hashmap->node[i][j] == NULL) {
                continue;
            }
            NODE_Ptr first_node = hashmap->node[i][j];
            while(first_node != NULL) {
                for (k = 0; k < module_num; ++k) {
                    if (first_node->val.acctime[k] == 0) { // 是否需要调用次数是0的数据
                        continue;
                    }
                    fprintf(fw[k], "%p \t", (void*)first_node->key.funcAddr);
                    fprintf(fw[k], "%p \t", (void*)first_node->key.faterAddr);
                    fprintf(fw[k], "%d \t\t", first_node->val.times[k]);
                    fprintf(fw[k], "%lld \t\t", first_node->val.acctime[k]);
                    fprintf(fw[k], "%lld \t", first_node->val.sheltime[k]);
                    fprintf(fw[k], "\n");
                }
                first_node = first_node->next;
            }
        }
    }
    for (k = 0; k < module_num; ++k) {
        fclose(fw[k]);
    }
}


// 因为适配区分模块采样，数据结构改变，导致以下接口发生改变，不可用
// void make_node(NODE_Ptr nodeptr, ULL funcAddr, ULL faterAddr, int times, ULL acctime, ULL sheltime)
// {
//     if (nodeptr == NULL) {
//         return;
//     }
//     nodeptr->key.funcAddr = funcAddr;
//     nodeptr->key.faterAddr = faterAddr;
//     nodeptr->val.times = times;
//     nodeptr->val.acctime = acctime;
//     nodeptr->val.sheltime = sheltime;
//     nodeptr->next = NULL;
// }

// 因为适配区分模块采样，数据结构改变，导致以下接口发生改变，不可用
// void show_node(NODE_Ptr nodeptr)
// {
//     if (nodeptr == NULL) {
//         return;
//     }
//     printf("----------------Node Profile------------------------------\n");
//     printf("NODE:Addr\t\t\t\t %p\n", (void*)nodeptr);
//     printf("NODE:key.funcAddr\t\t\t %lld\n", nodeptr->key.funcAddr);
//     printf("NODE:key.faterAddr\t\t\t %lld\n", nodeptr->key.faterAddr);
//     printf("NODE:val.times\t\t\t\t %d\n", nodeptr->val.times);
//     printf("NODE:val.acctime\t\t\t %lld\n", nodeptr->val.acctime);
//     printf("NODE:val.sheltime\t\t\t %lld\n", nodeptr->val.sheltime);
//     printf("----------------------------------------------------------\n");
// }

// 因为适配区分模块采样，数据结构改变，导致以下接口发生改变，不可用
// void update_node(NODE_Ptr node1, const NODE_Ptr node2)
// {
//     if (node1 == NULL || node2 == NULL) {
//         printf("update_node false, cause at last one ptr is NULL\n");
//         return;
//     }
//     node1->val.times += node2->val.times;
//     node1->val.acctime += node2->val.acctime;
//     node1->val.sheltime += node2->val.sheltime;
// }

// extern char** modules;
// extern int module_num;
void show_node(NODE_Ptr nodeptr)
{
    if (nodeptr == NULL) {
        return;
    }
    printf("----------------Node Profile------------------------------\n");
    printf("NODE:Addr\t\t\t\t %p\n", (void*)nodeptr);
    // printf("modules num\t\t\t\t %d\n", module_num);

    printf("NODE:key.funcAddr\t\t\t");
    printf("NODE:key.faterAddr\t\t\t");
    printf("NODE:val.times\t\t\t\t");
    printf("NODE:val.acctime\t\t\t");
    printf("NODE:val.sheltime\t\t\t\n");

    int i;
    for(i = 0; i < default_module_num; ++i) {
        printf("\t\t\t %lld", nodeptr->key.funcAddr);
        printf("\t\t\t %lld", nodeptr->key.faterAddr);
        printf("\t\t\t\t %d", nodeptr->val.times[i]);
        printf("\t\t\t %lld", nodeptr->val.acctime[i]);
        printf("\t\t\t %lld\n", nodeptr->val.sheltime[i]);

    }
    printf("----------------------------------------------------------\n");
}

void set_node_key(NODE_Ptr nodeptr, ULL funcAddr, ULL faterAddr)
{
    if (nodeptr == NULL) {
        printf("set_node_key false, cause the nodeptr is NULL\n");
        return;
    }
    nodeptr->key.funcAddr = funcAddr;
    nodeptr->key.faterAddr = faterAddr;
    nodeptr->next = NULL;
}

void set_node_val_by_module(NODE_Ptr nodeptr, int times, ULL acctime, ULL sheltime, int module)
{
    nodeptr->val.times[module]    = times;
    nodeptr->val.acctime[module]  = acctime;
    nodeptr->val.sheltime[module] = sheltime;
}
void update_node_val_by_module(NODE_Ptr nodeptr, int times, ULL acctime, ULL sheltime, int module)
{
    nodeptr->val.times[module]    += times;
    nodeptr->val.acctime[module]  += acctime;
    nodeptr->val.sheltime[module] += sheltime;
}

//      原先的代码
        // nodeptr->val.times[current_module]++;
        // nodeptr->val.acctime[current_module] += self_end_time - self_start_time;
        // nodeptr->val.sheltime[current_module] += perf_counter() - shell_start_time;