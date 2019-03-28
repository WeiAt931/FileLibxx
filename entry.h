/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   entry.h
 * Author: suang
 *
 * Created on March 26, 2019, 7:05 PM
 */

#ifndef ENTRY_H
#define ENTRY_H

#ifdef __cplusplus
extern "C" {
#endif




//用于操作函数的名称作为操作的类型
enum entry_type_e{XFPUTS,FWRITE,TRUNC,NONE};

struct  op_entry_t{
	enum entry_type_e type;
	long int start;
	long int new_len;
	char* new_buff;
        char* old_buff;
        long int old_len;
	struct op_entry_t* next;
};

void op_entry_cpy(struct op_entry_t*entry_node,int i,char*buff);
/**
 * 初始化文件操作条目的头指针空间。
 * 
 * @return 
 */
int op_entrys_head_init();
/**
 * 释放头节点空间，如果头节点后有其他节点，全部释放
 * @return 
 */
int op_entrys_head_array_free();
/**
 * 创建新的节点并返回
 * @return 
 */
struct op_entry_t* new_op_entry_node();
/**
 * 返回对应下标的entry列表头节点
 * @param index
 * @return null
 */
struct op_entry_t* get_op_entry_by_index(int index);
/**
 * 返回当前entrys头节点个数
 * @return 
 */
int get_entrys_head_num();
/**
 * 插入第index个链表一个entry
 * @param index
 * @param inserted_entry
 * @return 
 */
int insert_entry(int index,struct op_entry_t* inserted_entry);
/**
 * 当打开一个新文件时，需要为该文件准备一个op entry link
 * 创建，并返回index
 */
int new_entry_link();


#ifdef __cplusplus
}
#endif

#endif /* ENTRY_H */

