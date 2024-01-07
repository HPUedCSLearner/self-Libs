#ifndef __hash_table__
#define __hash_table__


#include "common.h"
#include <stdlib.h>
#include <stdbool.h>

#define HASH_TABLE_SIZE  (1 << 9)


typedef struct HASH_MAP
{
    NODE_Ptr node[HASH_TABLE_SIZE][HASH_TABLE_SIZE]; // 就像二维矩阵头节点
    ULL uesNum;
    ULL colisionNum;
    ULL capacity;
    MEM_POOL nodeMemPool;
}HASH_MAP;


void hash_map_init(HASH_MAP* hashmap);
void hash_map_show(HASH_MAP* hashmap);
NODE_Ptr hash_map_find_node(HASH_MAP* hashmap, const NODE* node);
NODE_Ptr hash_map_find_key(HASH_MAP* hashmap, const KEY* key);
void hash_map_insert(HASH_MAP* hashmap, NODE* node);
bool is_key_equeue(KEY key1, KEY key2);
ULL hash(ULL key);
void hash_map_destory(HASH_MAP* hashmap);
void show_hash_map_node(HASH_MAP* hashmap);
void save_hash_map_node_to_file(HASH_MAP* hashmap, char** filenames);


// 因为适配区分模块采样，数据结构改变，导致以下接口发生改变，不可用
// void make_node(NODE_Ptr, ULL funcAddr, ULL faterAddr, int times, ULL acctime, ULL sheltime);
// void show_node(NODE_Ptr);
// void update_node(NODE_Ptr node1, const NODE_Ptr node2); // 把node2的数据更新到node1


void set_node_key(NODE_Ptr, ULL funcAddr, ULL faterAddr);
void set_node_val_by_module(NODE_Ptr, int times, ULL acctime, ULL sheltime, int module);
void update_node_val_by_module(NODE_Ptr, int times, ULL acctime, ULL sheltime, int module);
void show_node(NODE_Ptr);




#endif