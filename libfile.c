/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "LibFile.h"
#include "entry.h"
#include "fcb.h"
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
enum file_mod_e {
    MOD_R = 1,
    MOD_W = 2,
    MOD_B = 4,
    MOD_P = 8,
    MOD_X = 16,
    MOD_A = 32,
    MOD_UNKNOWN = 0
};

/**
 *	依据传入的字符串类型的文件打开模式，返回一个int类型的值对应模式
 *	0000 0010 w
 *	0000 0100 b
 *	0000 1000 +
 *	0001 0000 x
 *	0010 0000 a
 */
static int convert_mod(char* mod) {
    int rmod = 0;

    for (int i = 0; i < (int) strlen(mod); i++) {
        if (mod[i] == 'r') {
            rmod |= MOD_R;
        } else if (mod[i] == 'w') {

            rmod |= MOD_W;
        } else if (mod[i] == 'b') {
            rmod |= MOD_B;
        } else if (mod[i] == '+') {
            rmod |= MOD_P;
        } else if (mod[i] == 'x') {
            rmod |= MOD_X;
        } else if (mod[i] == 'a') {

            rmod |= MOD_A;
        } else {
            fprintf(stderr, "can't understand this mod\n");
            return -1;
        }//if
    }//for	

    //处理不合法的文件打开模式
    if (((rmod & MOD_R)&&(rmod & MOD_A)) || ((rmod & MOD_W)&&(MOD_A & rmod))) {
        return -1; //ra wa是不合法的
    }
    return rmod;
}//convert_mod();

/**
 *	功能：判断文件是否有读权限
 *	1.从fcb_array中读出对应文件指针
 *	2.根据指针的权限记录
 *	3.如果存在MOD_R或者MOD_P权限，表明可以写，返回1
 *	4.否则返回0
 *
 */
static int has_r_mod(int mod) {
    /**
     * 如果权限位MOD_R或者MOD_P存在，则认为可读。
     */
    if (mod & MOD_R || mod & MOD_P) {
        return 1;
    }
    return 0;
}

int has_w_mod(int mod) {

    /**
     * 如果权限位MOD_R或者MOD_P存在，则认为可xie。
     */
    if (mod & MOD_W || mod & MOD_P) {
        return 1;
    }
    return 0;
}

/**
 * 判断是否可以 以当前权限访问文件
 * 1 可以
 * -1 不可以
 * @param path
 * @param mod
 * @return 
 */
static int xaccess(char*path, int mod) {
    /**
     * 判断是否有权限访问文件。
     * @param path
     * @param mod
     * @return 
     */

    int fcbs_num = get_fcbs_num();
    //遍历打开文件列表，如果文件已经打开，判断权限是否冲突，如果没有冲突返回1
    for (int i = 0; i < fcbs_num; i++) {
        if (strcmp(get_fcb_by_index(i)->path, path) == 0) {//该文件被打开过
            if (get_fcb_by_index(i)->isclose == 0) {//当前文件，处于开的状态
                if (has_w_mod(mod) || has_w_mod(get_fcb_by_index(i)->mod)) {
                    printf("xaccess():conflict!\n");
                    return -1;
                }
            }
        }
    }
    return 1;
}

int xfopen(char* path, char* mod) {
    int fmod = convert_mod(mod);
    if (fmod < 0) {
        fprintf(stderr, "the mod(%s) of file:%s is not true.\n", mod, path);
        exit(-1);
    }
    if (access(path, 0) == -1) {//not exist
        if (fmod & MOD_W || fmod & MOD_A) {//新建
            printf("file not exist, create new one\n");
            FILE* file_temp = fopen(path, "w");
            fclose(file_temp);
        } else {
            printf("file not found\n");
            return -1;
        }
    }
    if (xaccess(path, fmod) == -1) {
        printf("can't access the file\n");
        return -1;
    }
    FILE* fp = NULL;

    struct fcb_t* fcb_block = new_fcb_t();
    int loc = fcbs_insert(fcb_block);
    //如果模式含有w，同时不含有x，就需要截断文件
    //将文件所有内容全部读出，以TRUNC方式写入entry
    //然后删除文件，重建一个新的文件代替原文件
    if ((fmod & MOD_W)&&!(fmod & MOD_X)) {
        printf("w模式打开文件，需要截断文件\n");
        fp = fopen(path, "rb");
        if (!fp) {//当文件被占用，或者不存在的时候包错。
            printf("file not exist!\n");
            fcbs_remove(loc); //将插入的fcb删除。

            return -1; //文件打开失败
        }
        fseek(fp, 0L, SEEK_END);
        if (ftell(fp) == 0) {//文件为空，不需要截断操作
            printf("empty file\n");
        } else {
            char*buff = (char*) malloc(sizeof (char)*25);
            struct op_entry_t* entry = new_op_entry_node();
            entry->type = TRUNC;
            entry->start = ftell(fp);
            int r = fread(buff, sizeof (buff), 1, fp);
            while (r) {
                op_entry_cpy(entry, 2, buff);
                r = fread(buff, sizeof (buff), 1, fp);
            }//while
            insert_entry(loc, entry);
            fclose(fp);
            fp = NULL;
            free(buff);
            printf("del file\n");
            remove(path); //删除文件
            fp = fopen(path, "w"); //新建文件。
            fclose(fp);
            fp = NULL;
        }
    }//if(w)
    //resolve mod problem
    if ((fmod & MOD_B)) {//如果文件是二进制方式打开的话，使用rb+可读可写的二进制处理模式
        fp = fopen(path, "rb+");

    } else {
        fp = fopen(path, "r+");
    }

    if (!fp) {
        fprintf(stderr, "can't open file:%s\n", path);
        fcbs_remove(loc);
        return -1;
    }


    fcb_block->mod = fmod;
    fcb_block->file_pointer = fp;
    strcpy(fcb_block->path, path);
    fcb_block->isopen = 1;
    fcb_block->isclose = 0;
    int fcbs_num=get_fcbs_num();
    int is_open_flag=0;
    for(int i=0;i<fcbs_num;i++){
        if(strcmp(get_fcb_by_index(i)->path,fcb_block->path)==0){
            fcb_block->entrys_index=get_fcb_by_index(i)->entrys_index;
            is_open_flag=1;
            break;
        }
    }
    if(is_open_flag==0){//新打开的文件
        fcb_block->entrys_index=new_entry_link();    
    }
    
    printf("xfopen():done\n");
    return loc;
    return 0;
}