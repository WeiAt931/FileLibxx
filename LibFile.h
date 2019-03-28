/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   LibFile.h
 * Author: suang
 *
 * Created on March 26, 2019, 3:56 PM
 */

#ifndef LIBFILE_H
#define LIBFILE_H

#ifdef __cplusplus
extern "C" {
#endif
#include<stdio.h>
#include<stdlib.h>
    
    
    int  xfopen(char* path,char* mod);
    int  xclose(int index);
    int  xfputs(char* buff,int index);

    int  lib_init();
    int  lib_finish();
#ifdef __cplusplus
}
#endif

#endif /* LIBFILE_H */

