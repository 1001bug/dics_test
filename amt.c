/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * native POPCNT or software almost does not matter!
 * diffirenc in several nanoseconds
 * most havy malloc and array indexing
 * __builtin_popcount normaly converts to several instructions (just like CPop fun) but not popcnt nativ cpu instcurtion
 * to fix it add -mpopcnt to compiller (or -mavx or -msse4 or -msse4.2)
 * disassembling and testing on intel and gcc6.3
 */

/* ABOUT
 * inspired by YASM HAMT
 * rewritten to be compatible with JUDY array interface - on insertion or search return poiner to user data
 * it is just experiment
 * Glib hash works faster, but it is not comformable to pass poiners to key
 * 
 * algoritm unified for 32 and 64 bit platform
 * root hash always 64 slots (on 32&64bit)
 * subTables always 32 slots (on 32&64bit) (growing from 1 to 31 as needed)
 * bitmap store only 32 bit to address subTables, but on 64 bit platform also store key
 * 
 * it is AMT (Array mapped Trie)
 * no hashing on key
 * 
 * 6 bit to jump to root hash is optimal for both 32 and 64 bit
 * 5 bit all other cuts
 * is is optimal for both 32&64bit
 
 * I compare 2 some valiants AMT32_insert and AMT64_insert on same data with
 * different root tabel and nxt cuts from 5 to 6
 * and most good result was on 32-64 slotst root and 5bit other cuts
 * so i choose 64 slots root table as differens was so littel (32slots faster, don't know why)
 * 
 * ho hashing - son no collisons
 * hashing make spread wider, 
 * linear and random sata spread good
 * but if data "not normal" for example first 5-6 bits never changes, then this put all data iin one root slot
 * 
 * 
 * no delete func. normaly i do not need such abbility
 * 
 * 
 * JudyAray is my faforit chose.
 * it is fast, little memory (using for parsing 20-30Gb logs), and it is nativly sorted
 * difference so littel  - 100-200nano
 * 
 * in some casese capacity is known. so hash table may be preallocated, but AMT/JUDY
 * smometime get stack on memory allocations (not shure, but hashtable have about MAX values just several time a test cycle)
 * 
 */

/*
 5+5+5+5+5+5+3           32bit
 6+6+6+6+6+6+6+6+6+6+4   64bit
 
 6+5+5+5+5+5+2               32bit
 6+5+5+5+5+5+5+5+5+5+5+5+3   64bit
 
 */

/*
 * insertion linerar grow data: judy superfast, AMT goot
 * insertion linerar decrease data: about the sawm
 * insertion random (rand) data: judy superfast, AMT goot
 * insertion random (rand) hashed data (int hasher): judy slow, AMT slow. about the same
 * insertion random (rand) data shifterd <<32: judy fast, AMT goot  (>>)
 * 
 * 
 * TESTS on Intel(R) Core(TM) i7-7700 CPU @ 3.60GHz, with cmdline="isolcpus=3"
 * 
 * taskset -c 3 chrt -r 1 ./dics_test

 * random uint32 from 1 to 1`000`000

dup=3
HAMT   random insert	   NANO	 3754 	CNT: 2000
MIN:   21, P01:   21, P25:   60, P50: [[   84, ]] P75:  101, P99:   779, MAX:   2819, AVG: 96.585  

dup=3
JUDY random insert	   NANO	 3754 	CNT: 2000
MIN:   60, P01:   71, P25:   92, P50: [[  130, ]] P75:  172, P99:  1562, MAX:  12530, AVG: 175.012  

 * **************************************************************************** 

cnt 20K same result

 * ****************************************************************************
 
dup=18656
HAMT   random insert	   NANO	 3733 	CNT: 200K
MIN:   20, P01:   51, P25:   81, P50: [[  114, ]] P75:  153, P99:  1627, MAX:  44087, AVG: 201.599  

dup=18656
JUDY random insert	   NANO	 3733 	CNT: 200K
MIN:   46, P01:   64, P25:   98, P50: [[  113, ]] P75:  146, P99:   473, MAX:  23346, AVG: 138.334  

 * ****************************************************************************
dup=1135106
HAMT   random insert	   NANO	 3748 	CNT: 2M
MIN:   20, P01:   65, P25:  148, P50: [[  181, ]] P75:  213, P99:  1510, MAX: 291627, AVG: 237.100  

dup=1135106
JUDY random insert	   NANO	 3748 	CNT: 2M
MIN:   45, P01:   64, P25:  107, P50: [[  136, ]] P75:  196, P99:  1699, MAX: 393887, AVG: 224.011  

 * ****************************************************************************
 real insertion 1M, 19M duplicated
dup=19000000
HAMT   random insert	   NANO	 3730 	CNT: 20M
MIN:   21, P01:   81, P25:  172, P50: [[  185, ]] P75:  204, P99:  1491, MAX: 48566044, AVG: 246.338  

dup=19000000
JUDY random insert	   NANO	 3730 	CNT: 20M
MIN:   45, P01:   62, P25:   82, P50: [[  122, ]] P75:  135, P99:   669, MAX: 48559875, AVG: 140.401
 
 
rand range to max int: 11M dups, 8+M insetrions

dup=11353148
HAMT   random insert	   NANO	 3721 	CNT: 20M
MIN:   20, P01:  102, P25:  238, P50: [[  277, ]] P75:  323, P99:  2161, MAX: 48730526, AVG: 359.918  

dup=11353148
JUDY random insert	   NANO	 3721 	CNT: 20M
MIN:   45, P01:   93, P25:  173, P50: [[  198, ]] P75:  241, P99:   773, MAX: 48715162, AVG: 254.554
 
 
 * *****************************************************************************
same range, same characteristic, BUT int value shifterd <<32 (64bit ineger insertion) (>>)

dup=11352940
HAMT   random insert	   NANO	 3719 	CNT: 20000000
MIN:   63, P01:  131, P25:  282, P50: [[  304, ]] P75:  333, P99:  2244, MAX: 48738646, AVG: 397.221  

dup=11352940
JUDY random insert	   NANO	 3719 	CNT: 20000000
MIN:   41, P01:   98, P25:  180, P50: [[  205, ]] P75:  254, P99:  1571, MAX: 48737834, AVG: 284.678  
 
 
 *  
 *  * 
 *  */
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include "amt.h"



#ifndef NOOPTY
#define UNLIKELY __glibc_unlikely
#define   LIKELY __glibc_likely
#else
#define UNLIKELY  
#define   LIKELY 
#endif

// +/- 1 nanosecond. emu slowly or same
//for __builtin_popcount add -mpopcnt to compiller (or -mavx or -msse4 or -msse4.2)
#ifdef CTPOP_EMULATION
#define CTPOP(v) CTPop( (uint32_t)v  )
#else
#define CTPOP(v) __builtin_popcount( (uint32_t)v  )
#endif


//SETTINGS
#define shift      5
#define shift_root 6
    
#define mask      ~((~0UL)<<5)
#define mask_root ~((~0UL)<<6)
    
#define table_root  1<<6
#define table_sub   1<<5
    
//platform dependant!!
#define keypartbits_limit  ( (sizeof(uintptr_t)*8) -1 )
    
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

inline static uint32_t CTPop(uint32_t Map) __attribute__ ((always_inline));
static int AMT_print_traverse_down(AMTNode *node, int offset);

#define memory_alignment_lastbit(ptr)    \
  ( (   UNLIKELY(   (   (uintptr_t)ptr & 0x1   )==0x1  )   )                        \
   ? __assert_fail ("Variable " __STRING(ptr)  " address  misaligned", __FILE__, __LINE__, __ASSERT_FUNCTION)  \
    : __ASSERT_VOID_CAST (0)      )

#define IsSubTrie(n)            (((n)->Base_or_Value & 0x1) == 0x1)
#define IsNotSubTrie(n)            (((n)->Base_or_Value & 0x1) == 0x0)

#define SetAsBase(node_base, subTreePtr)        (node_base) = (uintptr_t)(subTreePtr) | 0x1;
#define SetAsValue(node_value,    entryPtr)     (node_value) = (uintptr_t)(entryPtr);
#define GetBase(ptr)           (AMTNode *)( ( (ptr)->Base_or_Value ) ^ 1)

#ifndef assert_msg
#define assert_msg(expr,msg)    \
  ((   LIKELY(expr)   )                        \
   ? __ASSERT_VOID_CAST (0)      \
   : __assert_fail (__STRING(msg) ": "    __STRING(expr), __FILE__, __LINE__, __ASSERT_FUNCTION))
#endif


AMT * AMT_init(){
    
    AMT *hamt = calloc(1,sizeof(AMT));
    
    hamt->root = calloc(table_root,sizeof(AMTNode));

    return hamt;
}

void* AMT_insert(AMT *hamt, uintptr_t new_key){
   
    
    AMTNode *node     __attribute__ ((aligned(sizeof(uintptr_t))));
    AMTNode *newnodes __attribute__ ((aligned(sizeof(uintptr_t))));
    AMTEntry *entry   __attribute__ ((aligned(sizeof(uintptr_t))));
    
    uintptr_t key     __attribute__ ((aligned(sizeof(uintptr_t))));
    uint32_t keypart;
    uint32_t Map;
    int32_t keypartbits = 1;
    
    //int level = 0;
    
    key = new_key;
    
    keypart = key & mask_root;
    
    node = &(hamt->root[keypart]);

    if (node->Base_or_Value == 0) {
        
        entry = malloc(sizeof(AMTEntry));
        memory_alignment_lastbit(entry);
        
        entry->user_key  = new_key;
        entry->user_data = 0;
        
        node->BitMap_or_Key = key;
        SetAsValue(node->Base_or_Value,entry);
        
        //return addr to store user data
        return &(entry->user_data);
    }

    for (;;) {

        if (IsNotSubTrie(node)) {
            
            if (node->BitMap_or_Key == key
                && ((AMTEntry *)(node->Base_or_Value))->user_key == new_key
                    ) {

                //return addr whith existing user data
                return &( ((AMTEntry *)(node->Base_or_Value))->user_data );
            } 
            
            else {
                //build tree downward until keys differ
                
                //existing key
                uintptr_t key2 = node->BitMap_or_Key;
                
                for (;;) {
                    uint32_t keypart2;

                    /* replace node with subtrie */
                    keypartbits += shift;
                    if (UNLIKELY(keypartbits > keypartbits_limit)) {
                        assert(1==0);
                        
                        /*key = hamt->ReHashKey(r_key, level);
                        key2 = hamt->ReHashKey(
                            ((HAMTEntry *)(node->Base_or_Value))->key, level);
                        keypartbits = 0;*/
                        
                    }
                    
                    keypart = (key >> keypartbits) & mask;
                    keypart2 = (key2 >> keypartbits) & mask;

                    if (keypart == keypart2) {
                        /* Still equal, build one-node subtrie and continue
                         * downward.
                         */
                        newnodes = malloc(sizeof(AMTNode));
                        memory_alignment_lastbit(newnodes);
                        
                        newnodes[0].BitMap_or_Key = key2;
                        newnodes[0].Base_or_Value = node->Base_or_Value;
                        
                        node->BitMap_or_Key = 1UL<<keypart;
                        SetAsBase(node->Base_or_Value,newnodes);
                        
                        node = &newnodes[0];
                        
                        //level++;
                    } else {
                        /* partitioned: allocate two-node subtrie */
                        newnodes = malloc(2*sizeof(AMTNode));
                        memory_alignment_lastbit(newnodes);

                        entry = malloc(sizeof(AMTEntry));
                        memory_alignment_lastbit(entry);
                                
                        entry->user_key = new_key;
                        entry->user_data = 0;
                        
                        /* Copy nodes into subtrie based on order */
                        if (keypart2 < keypart) {
                            newnodes[0].BitMap_or_Key = key2;
                            newnodes[0].Base_or_Value = node->Base_or_Value;
                            newnodes[1].BitMap_or_Key = key;
                            SetAsValue((&newnodes[1])->Base_or_Value, entry);
                        } else {
                            newnodes[0].BitMap_or_Key = key;
                            SetAsValue((&newnodes[0])->Base_or_Value, entry);
                            newnodes[1].BitMap_or_Key = key2;
                            newnodes[1].Base_or_Value = node->Base_or_Value;
                        }

                        /* Set bits in bitmap corresponding to keys */
                        node->BitMap_or_Key = (1UL<<keypart) | (1UL<<keypart2);
                        SetAsBase(node->Base_or_Value, newnodes);
                        
                        return &(  entry->user_data  );
                    }
                }
            }
        }

        /* Here we get if current node is Subtrie: look up in bitmap */
        
        
        keypartbits += shift;
        if ( UNLIKELY(keypartbits >= keypartbits_limit) ) {
            assert(1==0);
            
            /*key = hamt->ReHashKey(r_key, level);
            keypartbits = 0;*/
        }
        keypart = (key >> keypartbits) & mask;
        
        //if NOT in bitmap - add node
        uint32_t bit = 1UL<<keypart;
        if (!(node->BitMap_or_Key & (bit))) {
            
            

            /* set bit to 1 */
            node->BitMap_or_Key |= bit;

            /* Count total number of bits in bitmap to determine new size */
            uint32_t Size= CTPOP(node->BitMap_or_Key);
            assert_msg(Size <= table_sub,"Size valid range 1..32: __builtin_popcount(BitMap)>table_sub");
            
            newnodes = malloc(Size*sizeof(AMTNode));
            memory_alignment_lastbit(newnodes);

            /* Count bits below to find where to insert new node at */
            //ACHTUNG!!! count only 32bit of 64. it is because BitMap use only 32 bit and only Value use 64bit on 64bit paltform
            Map = CTPOP(node->BitMap_or_Key & ~((~0ULL)<<keypart));
            assert_msg(Map < table_sub,"Map valid range 0..31: __builtin_popcount(keypart)>table_sub");
            
            
            /* Copy existing nodes leaving gap for new node */
            memcpy(newnodes, GetBase(node), Map*sizeof(AMTNode));
            memcpy(&newnodes[Map+1], &(GetBase(node))[Map],
                   (Size-Map-1)*sizeof(AMTNode));
            /* Delete old subtrie */
            free(GetBase(node));
            /* Set up new node */
            newnodes[Map].BitMap_or_Key = key;
            entry = malloc(sizeof(AMTEntry));
            memory_alignment_lastbit(entry);
            
            entry->user_key = new_key;
            entry->user_data = 0;
            
            SetAsValue((&newnodes[Map])->Base_or_Value, entry);
            SetAsBase(node->Base_or_Value, newnodes);

            return &( entry->user_data );
        }
        
        //keypart bit present in bitmap - jump to next node

        Map = CTPOP(node->BitMap_or_Key & ~((~0ULL)<<keypart));
        
        assert_msg(Map < table_sub,"Map valid range 0..31: __builtin_popcount(keypart)>table_sub");

        //level++;
        node = &(GetBase(node))[Map];
    }
}


void * AMT_get(AMT *hamt, uintptr_t r_key){

    AMTNode *node     __attribute__ ((aligned(sizeof(uintptr_t))));
    AMTNode *newnodes __attribute__ ((aligned(sizeof(uintptr_t))));
    AMTEntry *entry   __attribute__ ((aligned(sizeof(uintptr_t))));
    
    uintptr_t key     __attribute__ ((aligned(sizeof(uintptr_t))));
    uint32_t keypart;
    uint32_t Map;
    int32_t keypartbits = 1;

    key = r_key;

    
    keypart = key & mask_root;
    node = &hamt->root[keypart];

    //root slot empty
    if (node->Base_or_Value == 0)
        return NULL;

    
    for (;;) {
        if (IsNotSubTrie(node)) {
            if (node->BitMap_or_Key == key
                && ((AMTEntry *)(node->Base_or_Value))->user_key == r_key
                    
                    )
                return & (   ((AMTEntry *)(node->Base_or_Value))->user_data);
            else
                return NULL;
        }

        /* Subtree: look up in bitmap */
        keypartbits += shift;
        if ( UNLIKELY(keypartbits >= keypartbits_limit) ) {
            assert(1==0);
            
            /*key = hamt->ReHashKey(r_key, level);
            keypartbits = 0;*/
        }
        keypart = (key >> keypartbits) & mask;

        uint32_t bit = 1UL<<keypart;

        if (!(node->BitMap_or_Key & (bit))) 
            return NULL;
        
        
        Map = CTPOP(node->BitMap_or_Key & ~((~0ULL)<<keypart));
        
        assert_msg(Map < table_sub,"Map valid range 0..31: __builtin_popcount(keypart)>table_sub");

        //level++;
        node = &(GetBase(node))[Map];

    }
}

/******************************************************************************/
//emulate popcount. from Ideal Hash Trees (Phil Bagwell)
#define S_K5  0x55555555U
#define S_K3  0x33333333U
#define S_KF0 0xF0F0F0FU
#define S_KFF 0xFF00FFU

//ACHTUNG!!! __builtin_popcount converted to CTPop by GCC
//to use native INTEL popcnt add -mpopcnt to compiller (or -mavx or -msse4 or -msse4.2)
//10 nano __builtin_popcount
//13 this inline func
static inline uint32_t CTPop(uint32_t Map)
{
Map-=((Map>>1)&S_K5);
Map=(Map&S_K3)+((Map>>2)&S_K3);
Map=(Map&S_KF0)+((Map>>4)&S_KF0);
Map+=Map>>8;
return (Map+(Map>>16))&0x3F;
}

/***  STRING HASH *************************************************************/

static uintptr_t compute_string_hash(const char *s) {
    const int p = 31;
    const int m = 1e9 + 9;
    uintptr_t hash_value = 0;
    uintptr_t p_pow = 1;
    while(*s) {
        hash_value = (hash_value + (*s - 'a' + 1) * p_pow) % m;
        p_pow = (p_pow * p) % m;
        s+=1;
    }
    return hash_value;
}

static uintptr_t hashCode(const char *s){
  uintptr_t hash = 0;
  while(*s) {
    hash = hash * 31 + *s;
    s+=1;
  }
  return hash;
}

static uintptr_t yasm_HashKey(const char *key)
{
    uintptr_t a=31415, b=27183, vHash;
    for (vHash=0; *key; key++, a*=b)
        vHash = a*vHash + *key;
    return vHash;
}

static uintptr_t yasm_ReHashKey(const char *key, int Level)
{
    uintptr_t a=31415, b=27183, vHash;
    for (vHash=0; *key; key++, a*=b)
        vHash = a*vHash*(uintptr_t)Level + *key;
    return vHash;
}

/******* INT HASH ************************************************************/

static uint32_t integerHash32(uint32_t h)
{
	h ^= h >> 16;
	h *= 0x85ebca6b;
	h ^= h >> 13;
	h *= 0xc2b2ae35;
	h ^= h >> 16;
	return h;
}

// from code.google.com/p/smhasher/wiki/MurmurHash3
static uint64_t integerHash64(uint64_t k){
	k ^= k >> 33;
	k *= 0xff51afd7ed558ccdLLU;
	k ^= k >> 33;
	k *= 0xc4ceb9fe1a85ec53LLU;
	k ^= k >> 33;
return k;
}

uint32_t crc32cHardware32(uint32_t crc, const void* data, size_t length) {
    //function source from https://github.com/htot/crc32c/blob/master/crc32c/crc32c.cc
    //ACHTUNG   CRC32C is not CRC32  !!!
    
    //ARM C Language Extensions PDF
    //arm __crc32cw(crc64bit, *(uint32_t*) p_buf);
    //#include<arm_acle.h>
    //if defined __ARM_FEATURE_CRC32 then all right
    //gcc -o crc -std=c99 -march=armv8-a+crc -mfpu=crypto-neon-fp-armv8 crc.c
    //для crc32 важна march
    //и это crc32{C} - она отличается от того что называют crc32
    //__crc32cb  char8
    //__crc32ch  short16
    //__crc32cw  int32
    //__crc32cd  int64
    
    //врамках проекта zLib есть функция crc32()
    //работает не сильно медленнее (раза в два)
    //можно делать фоллбэк

    //INEL ONLY 
    //gcc -mcrc32
    //__builtin_ia32_crc32di int64
    //__builtin_ia32_crc32si int32
    //__builtin_ia32_crc32qi char8
    //__builtin_ia32_crc32hi short16
    
    
    const char* p_buf = (const char*) data;
    // alignment doesn't seem to help?
    for (size_t i = 0; i < length / sizeof(uint32_t); i++) {
        //crc = __builtin_ia32_crc32si(crc, *(uint32_t*) p_buf);
        p_buf += sizeof(uint32_t);
    }

    // This ugly switch is slightly faster for short strings than the straightforward loop
    length &= sizeof(uint32_t) - 1;
    /*
    while (length > 0) {
        crc32bit = __builtin_ia32_crc32qi(crc32bit, *p_buf++);
        length--;
    }
    */
    switch (length) {
        case 3:
            //crc = __builtin_ia32_crc32qi(crc, *p_buf++);
        case 2:
            //crc = __builtin_ia32_crc32hi(crc, *(uint16_t*) p_buf);
            break;
        case 1:
            //crc = __builtin_ia32_crc32qi(crc, *p_buf);
            break;
        case 0:
            break;
        default:
            assert(0);
    }

    return crc;
}
uint32_t crc32cHardware64(uint32_t crc, const char *p_buf,size_t length) {

    //math done platform indep by uintptr_t
    //MACRO for intrinsics choose?
    //but __builtin_ia32_crc32di for 64bit
    //and __builtin_ia32_crc32si from 32bit
    
    //size_t length = strlen(p_buf);
    
    // alignment doesn't seem to help?
    uintptr_t crc64bit = crc;
    for (size_t i = 0; i < length / sizeof(uintptr_t); i++) {
        //crc64bit = __builtin_ia32_crc32di(crc64bit, *(uintptr_t*) p_buf);
        p_buf += sizeof(uintptr_t);
    }

    // This ugly switch is slightly faster for short strings than the straightforward loop
    uint32_t crc32bit = (uint32_t) crc64bit;
    length &= sizeof(uintptr_t) - 1;
    /*
    while (length > 0) {
        crc32bit = __builtin_ia32_crc32qi(crc32bit, *p_buf++);
        length--;
    }
    */
    switch (length) {
        case 7:
            //crc32bit = __builtin_ia32_crc32qi(crc32bit, *p_buf++);
        case 6:
            //crc32bit = __builtin_ia32_crc32hi(crc32bit, *(uint16_t*) p_buf);
            p_buf += 2;
        // case 5 is below: 4 + 1
        case 4:
            //crc32bit = __builtin_ia32_crc32si(crc32bit, *(uint32_t*) p_buf);
            break;
        case 3:
            //crc32bit = __builtin_ia32_crc32qi(crc32bit, *p_buf++);
        case 2:
            //crc32bit = __builtin_ia32_crc32hi(crc32bit, *(uint16_t*) p_buf);
            break;
        case 5:
            //crc32bit = __builtin_ia32_crc32si(crc32bit, *(uint32_t*) p_buf);
            p_buf += 4;
        case 1:
            //crc32bit = __builtin_ia32_crc32qi(crc32bit, *p_buf);
            break;
        case 0:
            break;
        default:
            // This should never happen; enable in debug code
            assert(0);
    }

    return crc32bit;

}
/**** PRINT ******************************************************************/


static void p(int v) {
    for (int i = 0; i < v; i++)
        printf("  ");
}
static void h(int v) {
    for (int i = 0; i < v; i++)
        printf("#");
}

static int AMT_print_traverse_down(AMTNode *node, int offset){
    int count=0;
    
    if (IsSubTrie(node)) {
        unsigned long i, Size;

        /* Count total number of bits in bitmap to determine size */
        //ACHTUNG only 32 bit of Btimap!!
        Size = __builtin_popcount(node->BitMap_or_Key);

        p(offset);
            printf("Trie. Size %lu. BitMap: ",Size);
            for(int i=31; i >= 0; i-=4) {
            printf("%i%i%i%i "
                    , (node->BitMap_or_Key & (1 << (i-0))) > 0
                    , (node->BitMap_or_Key & (1 << (i-1))) > 0
                    , (node->BitMap_or_Key & (1 << (i-2))) > 0
                    , (node->BitMap_or_Key & (1 << (i-3))) > 0
                    );
            }
            printf("\n");
            
                    
                    
                    
                    
                    
        for (i=0; i<Size; i++){
            
            count+=AMT_print_traverse_down(&(GetBase(node))[i],offset+1);
        }
            return count;
        
    }else if(node->BitMap_or_Key){
        p(offset);
        printf("Key: %lu\n", node->BitMap_or_Key);
        p(offset);
        printf("user_key:  %lu\n", ((AMTEntry *)(node->Base_or_Value))->user_key);
        p(offset);
        printf("user_data: 0x%lx\n\n", (void*)(((AMTEntry *)(node->Base_or_Value))->user_data));
        
        return 1;
    
    }
}
void AMT_print(AMT *hamt)
{
    int i=0,c=0,s=0;

    
    
    for (i=0 ; i < table_root; i++){
        printf("Root %02i\n",i);
        c=AMT_print_traverse_down(&hamt->root[i], 1);
        printf("Root %02i COUNT ",i);
        h(c);
        printf(" %i\n",c);
        s+=c;
    }
    
    printf("FINISH: sum COUNT %i\n",s);
}

