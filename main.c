/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: alex
 *
 * Created on 4 апреля 2020 г., 1:34
 */
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include<stdarg.h>
#include<time.h>
#include<unistd.h>
//#include <pthread.h>
#include <Judy.h>
//#include <glib.h>
//-I/usr/include/glib-2.0/ -I/usr/lib/x86_64-linux-gnu/glib-2.0/include
//-lglib-2.0


#include "amt.h"

/*
 * 
 */


#define RDTSCP(V) __asm__ __volatile__("rdtscp;shl $32, %%rdx;or %%rdx, %%rax":"=a"(V):: "%rcx", "%rdx")

uint64_t cycles_in_one_usec=0;

void PRINT_STAT(const char *name, uint64_t *time_S, uint64_t *time_E, uint64_t *time_Dif, uint64_t cnt);
//void printKeyValue( gpointer key, gpointer value, gpointer userData );
#define NN 2000000





uint32_t integerHash32(uint32_t h);
uint64_t integerHash64(uint64_t h);



int main(int argc, char** argv) {

    long cnt=0;
    
    uint64_t start;
    uint64_t *rdtscp_S;
    uint64_t *rdtscp_E;
    uint64_t *rdtscp_dif;
    uint32_t *data_random;
    uint32_t *data_linear;
    
    Pvoid_t  PJLArray1 = (Pvoid_t) NULL;
    Pvoid_t  PJLArray2 = (Pvoid_t) NULL;
    Word_t * PValue; // pointer to array element value
    int      Rc_int;
    
    
    /*GHashTable *GHT1 = g_hash_table_new (g_int64_hash, g_int64_equal);
    GHashTable *GHT2 = g_hash_table_new (g_int64_hash, g_int64_equal);*/

    AMT *H_random = AMT_init(sizeof(uint32_t));
    AMT *H_linear = AMT_init(sizeof(uint32_t));
    
    
    
    
    uint64_t c1,c2;
    RDTSCP(c1);
    usleep(1000);
    RDTSCP(c2);
    cycles_in_one_usec = (c2-c1)/1000;
   
    rdtscp_S = calloc(NN, sizeof (uint64_t));
    rdtscp_E = calloc(NN, sizeof (uint64_t));
    rdtscp_dif = calloc(NN, sizeof (uint64_t));
   
    srand (time(NULL));
    
    data_random = calloc(NN, sizeof (uint64_t));
    data_linear = calloc(NN, sizeof (uint64_t));
    
    
    for(Word_t i=0;i<NN;i++){
        //data_random[i]=integerHash32(NN-i+1);//rand()%1000000+1;
        //data_random[i]=(i+1);//rand()%1000000+1;
        //data_random[i]=rand()%10000000+1;
        data_random[i]=  i+1;
    }
  
    
    
//HAMT_traverse(H_w);


    
    
    
    
    
    
    
    AMT *H_00 = AMT_init(sizeof(uint64_t));    
    cnt=0;
    for(Word_t i=0;i<NN;i++){
        Word_t d = data_random[i];
        //d|=d<<32;
        //d=integerHash64(d);
        
    RDTSCP(start);
  
    PValue = (Word_t*)AMT_insert(H_00,d);

    if(*PValue == d){ //dup
        cnt+=1;
    }else{
    
    *PValue = d;
    }
    
    RDTSCP(rdtscp_E[i]);rdtscp_S[i]=start;
    
    }
    printf("dup=%li\n",cnt);
    PRINT_STAT("HAMT   random insert", rdtscp_S, rdtscp_E, rdtscp_dif, NN);
    sleep(1); 
    
    
    
    
    cnt=0;
    for(Word_t i=0;i<NN;i++){
        Word_t d = data_random[i];
         //d|=d<<32;
        //d=integerHash64(d);        
        
    RDTSCP(start);
    JLI(PValue, PJLArray1, d);
        
    if(*PValue == d){
        cnt+=1;
    }else{
        *PValue = d;
    }
    RDTSCP(rdtscp_E[i]);rdtscp_S[i]=start;
    
    }
    printf("dup=%li\n",cnt);
    PRINT_STAT("JUDY random insert", rdtscp_S, rdtscp_E, rdtscp_dif, NN);
    sleep(1);
    
    
    
    
    
    
    
    
    //AMT_print(H_00);
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    exit(0);
    
    
    
     ///////////////////////////////////////////////////////////////////////////
    cnt=0;
    for(Word_t i=0;i<NN/2;i++){
        Word_t d = data_random[i];
    RDTSCP(start);
  
    PValue = (Word_t*)AMT_insert(H_random,d);

    if(*PValue == d){ //dup
        cnt+=1;
    }else{
    
    *PValue = d;
    }
    
    RDTSCP(rdtscp_E[i]);rdtscp_S[i]=start;
    
    }
    printf("cnt=%li\n",cnt);
    PRINT_STAT("HAMT random insert", rdtscp_S, rdtscp_E, rdtscp_dif, NN/2);
    sleep(1);
    
    
    
    cnt=0;
    for(Word_t i=0;i<NN/2;i++){
        Word_t d = data_random[i];
    RDTSCP(start);
    JLI(PValue, PJLArray1, d);

    if(*PValue == d){
        cnt+=1;
    }else{
        *PValue = d;
    }
    RDTSCP(rdtscp_E[i]);rdtscp_S[i]=start;
    
    }
    printf("cnt=%li\n",cnt);
    PRINT_STAT("JUDY random insert", rdtscp_S, rdtscp_E, rdtscp_dif, NN/2);
    sleep(1);
    
    
    
    
    
     ///////////////////////////////////////////////////////////////////////////
    cnt=0;
    for(Word_t i=0;i<NN;i++){
        Word_t d = data_linear[i];
    RDTSCP(start);
  
    PValue = (Word_t*)AMT_insert(H_linear,d);
    
    

    if(*PValue == d){

        cnt+=1;
    }else{
    
    *PValue = d;
    }
    
    RDTSCP(rdtscp_E[i]);rdtscp_S[i]=start;
    
    }
    printf("cnt=%li\n",cnt);
    PRINT_STAT("HAMT linear insert", rdtscp_S, rdtscp_E, rdtscp_dif, NN);
    sleep(1);
    
    
      cnt=0;
    for(Word_t i=0;i<NN;i++){
        Word_t d = data_linear[i];
    RDTSCP(start);
    JLI(PValue, PJLArray2, d);
    
    if(*PValue == d){
        
        cnt+=1;
    }else{
        *PValue = d;
    }
        
    
    RDTSCP(rdtscp_E[i]);rdtscp_S[i]=start;
    
    }
    printf("cnt=%li\n",cnt);
    PRINT_STAT("JUDY linear insert", rdtscp_S, rdtscp_E, rdtscp_dif, NN);
    sleep(1);
    
    
    
    
    
    
    
     ///////////////////////////////////////////////////////////////////////////
    cnt=0;
    for(Word_t i=0;i<NN;i++){
        Word_t d = data_random[i];
    RDTSCP(start);
    
    //PValue = HAMT_insert(H_random,d);
    PValue = (Word_t*)AMT_get(H_random,d);
    

    if(PValue && *PValue == d){
        
        cnt+=1;
    }
        
    
    RDTSCP(rdtscp_E[i]);rdtscp_S[i]=start;
    
    }
    printf("cnt=%li\n",cnt);
    PRINT_STAT("HAMT random check", rdtscp_S, rdtscp_E, rdtscp_dif, NN);
    sleep(1);
    
    
    cnt=0;
    for(Word_t i=0;i<NN;i++){
        Word_t d = data_random[i];
    RDTSCP(start);
    JLG(PValue, PJLArray1, d);

    if(PValue && *PValue == d) 
        cnt+=1;
    RDTSCP(rdtscp_E[i]);rdtscp_S[i]=start;
    
    }
    printf("cnt=%li\n",cnt);
    PRINT_STAT("JUDY random check", rdtscp_S, rdtscp_E, rdtscp_dif, NN);
    sleep(1);
    
    
    
     ///////////////////////////////////////////////////////////////////////////
    cnt=0;
    for(Word_t i=0;i<NN;i++){
        Word_t d = data_linear[i];
    RDTSCP(start);
    
    //PValue = HAMT_insert(H_linear,d);
    PValue = (Word_t*)AMT_get(H_linear,d);
    

    if(PValue && *PValue == d){
        
        cnt+=1;
    }
        
    
    RDTSCP(rdtscp_E[i]);rdtscp_S[i]=start;
    
    }
    printf("cnt=%li\n",cnt);
    PRINT_STAT("HAMT linear check", rdtscp_S, rdtscp_E, rdtscp_dif, NN);
    sleep(1);
    
    cnt =0;
    for(Word_t i=0;i<NN;i++){
        Word_t d = data_linear[i];
    RDTSCP(start);
    JLG(PValue, PJLArray2, d);

    if(PValue && *PValue == d) 
        cnt+=1;
    
    RDTSCP(rdtscp_E[i]);rdtscp_S[i]=start;
    
    }
    printf("cnt=%li\n",cnt);
    PRINT_STAT("JUDY linear chaeck", rdtscp_S, rdtscp_E, rdtscp_dif, NN);
    sleep(1);
    
    exit(0);
    
    
     ///////////////////////////////////////////////////////////////////////////
  
    
    ///////////////////////////////////////////////////////////////////////////
    
    
       /*
    ///////////////////////////////////////////////////////////////////////////
    cnt=0;
    for(Word_t i=0;i<NN;i++){
    RDTSCP(start);
    
    if(g_hash_table_insert( GHT1, &(data_linear[i]), &(data_linear[i]) )){
    cnt+=1;
    }
    
    RDTSCP(rdtscp_E[i]);rdtscp_S[i]=start;
    
    }
    printf("cnt=%li\n",cnt);
    PRINT_STAT("GLIB linear", rdtscp_S, rdtscp_E, rdtscp_dif, NN);
    sleep(1);
    
    cnt=0;
    for(Word_t i=0;i<NN;i++){
    RDTSCP(start);
    
    if(g_hash_table_insert( GHT2, &(data_random[i]), &(data_random[i]) )){
    cnt+=1;
    }
    RDTSCP(rdtscp_E[i]);rdtscp_S[i]=start;
    
    }
    printf("cnt=%li\n",cnt);
    PRINT_STAT("GLIB random", rdtscp_S, rdtscp_E, rdtscp_dif, NN);
    sleep(1);
    

    
     ///////////////////////////////////////////////////////////////////////////
    
    
    ///////////////////////////////////////////////////////////////////////////
    
    
    
    //////////////////////////////////////////////////////////////////////////
    cnt=0;
    for(Word_t i=0;i<NN;i++){
    RDTSCP(start);
    
    if(g_hash_table_lookup( GHT1, &(data_linear[i]))){
        cnt+=1;
    }
    
    RDTSCP(rdtscp_E[i]);rdtscp_S[i]=start;
    
    }
    printf("cnt=%li\n",cnt);
    PRINT_STAT("GLIB lookup linear", rdtscp_S, rdtscp_E, rdtscp_dif, NN);
    sleep(1);
    
    
    cnt=0;
    for(Word_t i=0;i<NN;i++){
    RDTSCP(start);
    
    if(g_hash_table_lookup( GHT2, &(data_random[i]))){
        cnt+=1;
    } 
    
    RDTSCP(rdtscp_E[i]);rdtscp_S[i]=start;
    
    }
    printf("cnt=%li\n",cnt);
    PRINT_STAT("GLIB lookup random", rdtscp_S, rdtscp_E, rdtscp_dif, NN);
    sleep(1);
    

    
    
    //g_hash_table_foreach( GHT, printKeyValue, NULL );

   // Destroy the map.
   g_hash_table_destroy( GHT1 );
   g_hash_table_destroy( GHT2 );
   */
    
    
    
    return (EXIT_SUCCESS);
}

/*
 void printKeyValue( gpointer key, gpointer value, gpointer userData ) {
     static int cnt=0;

   printf( "%5i\t%li => %li\n",cnt++, *( (Word_t*)key ), *( (Word_t*)value ) );
   return;
}
 */
 
 
 
#define RDTSC
 
void PRINT_STAT(const char *name, uint64_t *time_S, uint64_t *time_E, uint64_t *time_Dif, uint64_t count) {

#ifdef RDTSC
//    uint64_t cycles_in_one_usec = count_cpu_cycles_in_one_usec();
#endif 

    int cmpare(const void *a, const void *b) {

        const uint64_t *ia = (const uint64_t *) a;

        const uint64_t *ib = (const uint64_t *) b;

        return *ia - *ib;

    }

    uint64_t sum=0;


    uint64_t cnt = 0;

    for (uint64_t i = 0; i < count; i++) {

        uint64_t tmp = time_E[i] - time_S[i];

        if (tmp > 0) {

#ifdef RDTSC
            tmp = (tmp * 1000L) / cycles_in_one_usec;
#endif 

            time_Dif[cnt] = tmp;
            
            sum+=tmp;

            cnt += 1;

        }

    }





    qsort(time_Dif, cnt, sizeof (uint64_t), cmpare);





    fprintf(stderr,

            "%s\t   NANO\t %lu \tCNT: %i\n"

            "MIN: %4lu, "

            "P01: %4lu, "

            "P25: %4lu, "

            "P50: [[ %4lu, ]] "

            "P75: %4lu, "

            "P99: %5lu, "

            "MAX: %6lu, "
            
            "AVG: %4.3f  "



            "\n\n"

            , name
            
            , cycles_in_one_usec

            , cnt

            , time_Dif[0]

            , time_Dif[cnt / 100]

            , time_Dif[(cnt * 25) / 100]

            , time_Dif[(cnt * 50) / 100]

            , time_Dif[(cnt * 75) / 100]

            , time_Dif[(cnt * 99) / 100]

            , time_Dif[cnt - 1]

            ,sum*1.0/cnt

            );







}










uint64_t integerHash64(uint64_t k){
	k ^= k >> 33;
	k *= 0xff51afd7ed558ccdLLU;
	k ^= k >> 33;
	k *= 0xc4ceb9fe1a85ec53LLU;
	k ^= k >> 33;
return k;
}


uint32_t integerHash32(uint32_t h)
{
	h ^= h >> 16;
	h *= 0x85ebca6b;
	h ^= h >> 13;
	h *= 0xc2b2ae35;
	h ^= h >> 16;
	return h;
}