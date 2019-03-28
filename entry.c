/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "entry.h"
#include "flags.h"
#include "flags.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define INIT_ENTRYS_HEAD_SIZE 20
#define INC_ENTRYS_HEAD_SIZE 10

#define INIT_BUFF_SIZE 1024 

struct op_entry_t** entrys_head_array;
int entrys_head_array_size=0;
int entrys_head_num=0;

/**
 * 
 * struct  op_entry_t{
	enum entry_type_e type;
	long int start;
	long int new_len;
	char* new_buff;
        int len1;
        char* old_buff;
        int len2;
	struct log_entry_t* next;
};
 */

/**
 * 将buff中的内容拷贝到entry缓冲区中，当i=1,拷贝到new ，i=2,拷贝到old
 * @param entry_node
 * @param i 1 / 2
 * @param buff
 */
void op_entry_cpy(struct op_entry_t*entry_node,int i,char*buff){
    int len=strlen(buff);
    
    if(i==1){
        if(sizeof(entry_node->new_buff)-entry_node->new_len<=len){//剩余空间是否比需要空间大1（用于写'\0'）
            entry_node->new_buff=(char*)realloc(entry_node->new_buff,
                    sizeof(entry_node->new_buff)+sizeof(char)*len);
            //根据要拷贝进来的buff大小分配空间。
        }
        strcpy((entry_node->new_buff)+entry_node->new_len,buff);
        entry_node->new_len+=len;
        
    }else if(i==2){
        if(sizeof(entry_node->old_buff)-entry_node->old_len<=len){//剩余空间是否比需要空间大1（用于写'\0'）
            entry_node->old_buff=(char*)realloc(entry_node->old_buff,
                    sizeof(entry_node->old_buff)+sizeof(char)*len);
            
        }
        strcpy((entry_node->old_buff)+entry_node->old_len,buff);
        entry_node->old_len+=len;
    }


}
/**
 * 释放一个entry 节点
 * @param entry_node
 */
static void op_entry_node_free(struct op_entry_t* entry_node){
    if(!entry_node){
        return ;
    }
    if(entry_node->new_buff){
        free(entry_node->new_buff);
    }
    if(entry_node->old_buff){
        free(entry_node->old_buff);
    }
    free(entry_node);
    return ;
}


/**
 * 释放一个entry链表
 * @param entrys_link_head
 */
static void op_entrys_link_free(struct op_entry_t* entrys_link_head){
    if(!entrys_link_head){
        return ;
    }
    struct op_entry_t* second_entry_node=entrys_link_head->next;
    while(second_entry_node){
        op_entry_node_free(second_entry_node);
        second_entry_node=second_entry_node->next;
    }
    op_entry_node_free(entrys_link_head);
    return;
}

/**
 * 释放所有entry,所有空间
 * @return 
 */
int op_entrys_head_array_free(){
    if(!entrys_head_array){//为空不用释放
        return success;
    }
    for(int i=0;i<entrys_head_num;i++){
        op_entrys_link_free(entrys_head_array[i]);
    }
    free(entrys_head_array);
    entrys_head_array=NULL;
    return success;
}

int op_entrys_head_array_init(){
	//printf("log_entry_init():do\n");
	if(!entrys_head_array){
            entrys_head_array=(struct op_entry_t**)malloc(sizeof(struct op_entry_t*)*INIT_ENTRYS_HEAD_SIZE);
        }
        if(!entrys_head_array){
            fprintf(stderr,"can't malloc enough space for process!\n");
            exit(-1);
        }
        entrys_head_array_size=INIT_ENTRYS_HEAD_SIZE;
        entrys_head_num=0;
        
	//printf("log_entry_init():done\n");
	return success;
}

static int op_entrys_head_inc(){
    entrys_head_array=(struct op_entry_t**)realloc(entrys_head_array,
            sizeof(entrys_head_array)+sizeof(struct op_entry_t*)*INC_ENTRYS_HEAD_SIZE);
    if(!entrys_head_array){
        fprintf(stderr,"op_entrys_head_inc():realloc err\n");
        exit(-1);
    }
    entrys_head_array_size+=INC_ENTRYS_HEAD_SIZE;
    return success;
}


/**
 * 创建一个新的entry节点并返回指针。
 * @return 
 */
struct op_entry_t* new_op_entry_node(){
    struct op_entry_t* new_entry_node;
    new_entry_node=(struct op_entry_t*)malloc(sizeof(struct op_entry_t));
    if(!new_entry_node)exit(-1);
    new_entry_node->new_buff=(char*)malloc(sizeof(char)*INIT_BUFF_SIZE);
    if(!new_entry_node->new_buff)exit(-1);
    new_entry_node->old_buff=(char*)malloc(sizeof(char)*INIT_BUFF_SIZE);
    if(!new_entry_node->old_buff)exit(-1);
    new_entry_node->new_len=0;
    new_entry_node->old_len=0;
    new_entry_node->start=0;
    new_entry_node->next=NULL;
    new_entry_node->type=NONE;
    return new_entry_node;
}

struct op_entry_t* get_op_entry_by_index(int index){
    if(index<0||index>=entrys_head_num){
        fprintf(stderr,"get_op_entry_by_index():out of range!\n");
        return NULL;
    }
    return entrys_head_array[index];
}


/**
 * 
 * 返回当前总共有多少entrys 头节点
 * @return 
 */
int get_entrys_head_num(){
    return entrys_head_num;
}

/**
 * 插入第index个链表一个entry
 * @param index
 * @param inserted_entry
 * @return 
 */
int insert_entry(int index,struct op_entry_t* inserted_entry){
    if(index<0||index>=entrys_head_num){
        fprintf(stderr,"insert_entry():out of range!\n");
        return 0;
    }
    if(!entrys_head_array[index]){//如果当前列表头节点为空，创建节点后插入
        entrys_head_array[index]=new_op_entry_node();
    }
    inserted_entry->next=entrys_head_array[index]->next;
    entrys_head_array[index]->next=inserted_entry;
    return success;
}

int new_entry_link(){
    if(entrys_head_num>=entrys_head_array_size){
        op_entrys_head_inc();
    }
    entrys_head_array[entrys_head_num++]=new_op_entry_node();
    return entrys_head_num-1;

}