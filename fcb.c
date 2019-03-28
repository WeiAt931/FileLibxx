/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdio.h>
#include <stdlib.h>

#include "fcb.h"
#include "flags.h"

#define FCBS_INIT_SIZE 256
#define FCBS_INCREAMENT 10

struct fcb_t** fcbs_array;
int fcbs_array_size=0;
int fcbs_num=0;

static int fcbs_block_free(struct fcb_t* fcb_block){
    if(fcb_block){
        if(fcb_block->file_pointer){
            fclose(fcb_block->file_pointer);
            fcb_block->file_pointer=NULL;
        }
        free(fcb_block);
    }
    return 1;
}

static int fcbs_array_inc(){
    fcbs_array=(struct fcb_t**)realloc(fcbs_array,sizeof(fcbs_array)+sizeof(struct fcb_t*)*FCBS_INCREAMENT);
    if(!fcbs_array){
        fprintf(stderr,"fcbs_array_inc():realloc err\n");
        exit(-1);
    }
    fcbs_array_size+=FCBS_INCREAMENT;
    return success;
}

int get_fcbs_num(){
    return fcbs_num;
}

FILE*  get_file_ptr_by_index(int index){
   if(index<0||index>=fcbs_num){
        fprintf(stderr,"get_file_ptr_by_index():out of range\n");
        return NULL;
    }
   return fcbs_array[index]->file_pointer;
}

struct fcb_t* get_fcb_by_index(int index){
    if(index<0||index>=fcbs_num){
        fprintf(stderr,"get_fcb_by_index():out of range\n");
        return NULL;
    }
    return fcbs_array[index];
}

struct fcb_t* new_fcb_t(){
    struct fcb_t* new_fcb=(struct fcb_t*)malloc(sizeof(struct fcb_t));
    if(!new_fcb){
        fprintf(stderr,"new_fcb_t():malloc err\n");
        exit(-1);
    }
    new_fcb->entrys_index=-1;
    new_fcb->file_pointer=NULL;
    new_fcb->isclose=0;
    new_fcb->isopen=0;
    new_fcb->mod=-1;
    new_fcb->path[0]='\0';
    return new_fcb;
}

int fcbs_remove(int index){
    if(index<0||index>=fcbs_num){
        fprintf(stderr,"fcbs_remove():out of range\n");
        return -1;
    }
    
    fcbs_block_free(fcbs_array[index]);
    fcbs_array[index]=NULL;
    fcbs_num--;
    return success;    
}

/**
 * 将fcb插入数组，同时返回下标
 * @param inserted_fcb
 * @return 
 */
int fcbs_insert(struct fcb_t* inserted_fcb){
    if(fcbs_num>=fcbs_array_size){
        fcbs_array_inc();
    }
    fcbs_array[fcbs_num++]=inserted_fcb;
    return fcbs_num-1;
}

/**
 * 初始化指针数组空间
 * @return 
 */
int fcbs_array_init(){

    fcbs_array=(struct fcb_t**)malloc(sizeof(struct fcb_t*)*FCBS_INIT_SIZE);
    if(!fcbs_array){
        fprintf(stderr,"fcbs_array_init():malloc err\n");
        exit(-1);
    }
    fcbs_array_size=FCBS_INIT_SIZE;
    fcbs_num=0;
    return success;
}


int fcbs_array_free(){
    for(int i=0;i<fcbs_num;i++){
        fcbs_block_free(fcbs_array[i]);
    }
    free(fcbs_array);
    fcbs_array=NULL;
    return success;
}