/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   fcb.h
 * Author: suang
 *
 * Created on March 26, 2019, 7:04 PM
 */

#ifndef FCB_H
#define FCB_H

#ifdef __cplusplus
extern "C" {
#endif
#define FCBS_INIT_SIZE 256
#define FCBS_INCREAMENT 10
#include<stdlib.h>
#include<stdio.h>



struct fcb_t{
	FILE* file_pointer;
	int mod;
        char path[256];
        int isopen;
        int isclose;
        int entrys_index;       
};
int get_fcbs_num();
FILE*  get_file_ptr_by_index(int index); 
struct fcb_t* get_fcb_by_index(int index);
/**
 * 创建节点，返回指针。
 * @return 
 */
struct fcb_t* new_fcb_t();
/**
 * 初始化fcb指针数组
 * @return 
 */
int fcbs_array_init();
/**
 * 释放fcb指针数组，
 * @return 
 */
int fcbs_array_free();
/**
 * 将fcb插入数组，同时返回下标
 * @param inserted_fcb
 * @return 
 */
int fcbs_insert(struct fcb_t* inserted_fcb);
/**
 * 移除index处的fcb
 * @param index
 * @return 
 */
int fcbs_remove(int index);

#ifdef __cplusplus
}
#endif

#endif /* FCB_H */

