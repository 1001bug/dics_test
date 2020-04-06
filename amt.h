/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   amt.h
 * Author: alex
 *
 * Created on 6 апреля 2020 г., 23:15
 */

#ifndef AMT_H
#define AMT_H

#ifdef __cplusplus
extern "C" {
#endif
    
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <stdint.h>
    

    typedef struct AMTEntry {
        uintptr_t user_key;
        uintptr_t user_data;
    } AMTEntry;

    typedef struct AMTNode {
        uintptr_t BitMap_or_Key; /* 32 bits, bitmap or 32-64bit key */
        uintptr_t Base_or_Value; /* piner to AMTNode list or AMTEntry */
    } AMTNode;

    typedef struct AMT {
        AMTNode *root;
    } AMT;


    uintptr_t AMT_insert(AMT *hamt, uintptr_t new_key);
    uintptr_t AMT_get(AMT *hamt, uintptr_t r_key);
    AMT * AMT_create();
    
    void AMT_print(AMT *hamt);





#ifdef __cplusplus
}
#endif

#endif /* AMT_H */

