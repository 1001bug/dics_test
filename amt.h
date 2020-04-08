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
    
//c99 uintptr_t
#include <stdint.h>

    //Hide internals
    typedef struct AMT AMT;

    void* AMT_insert(AMT *hamt, uintptr_t new_key);
    void* AMT_get(AMT *hamt, uintptr_t r_key);
    AMT * AMT_init();
    
    void AMT_print(AMT *hamt);





#ifdef __cplusplus
}
#endif

#endif /* AMT_H */

