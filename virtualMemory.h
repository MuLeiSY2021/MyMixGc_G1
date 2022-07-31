//
// Created by muleisy on 22-7-29.
//
#ifndef YGC_VIRTUAL_MEMORY_H
#define YGC_VIRTUAL_MEMORY_H

#include <stdlib.h>
#include <stdio.h>
#include <uthash.h>
#include "tool.h"
#include "lib/myLinkedList.h"
#endif //YGC_VIRTUAL_MEMORY_H


//----------------------CONST-------------------------//
const int CELL_SIZE = 32;

const int CELL_COUNT = 20;

enum CELL_TYPE {
    FREE = 0,
    EDEN = 1,
    SURVIVE = 2,
    OLD = 3,
    HUGE = 4,
};

//------------------ANNOUNCE---------------//

//=========STRUCT=========//
typedef struct VirtualMemory *VirtualMemory_p;
typedef struct Cell *Cell_p;
typedef struct MemoryMap *MemoryMap_p;
//=======FUNCTION===========//
MemoryMap_p newMemoryMap();
Cell_p newCell(int id, int type);
//-----------------------Cell--------------------------//

struct Cell {
    int type_flg;
    int id;
    int protectFlg;
    long usedIndex;
};

void setFlg(int *flgTgr, int type) {
    *flgTgr = type;
}

Cell_p newCell(int id, int type) {
    Cell_p tmp = malloc(sizeof(struct Cell));
    tmp->type_flg = type;
    tmp->id = id;
    tmp->usedIndex = 0;
    return tmp;
}
//---------------------VirtualMemory----------------------//

//---------------------------------------------------//
struct VirtualMemory {
    int size;
    int freeCellId;
    void *memory;
    MemoryMap_p memoryMap;
    Cell_p cells[];
};

Cell_p getCell(int id,VirtualMemory_p virtualMemory) {
    if(id > CELL_COUNT) {
        return NULL;
    }
    return virtualMemory->cells[id];
}

VirtualMemory_p newVirtualMemory() {
    VirtualMemory_p tmp = malloc(sizeof(struct VirtualMemory));
    tmp->size = CELL_SIZE * CELL_COUNT;
    tmp->freeCellId = 0;
    tmp->memory = malloc(tmp->size);
    tmp->memoryMap = newMemoryMap();
    for (int i = 0; i < CELL_COUNT; ++i) {
        tmp->cells[i] = newCell(i, FREE);
    }
    return tmp;
}

//------------------DISPLAY--------------------------------//
void virtualMemoryDis(VirtualMemory_p virtualMemory) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 5; ++j) {
            Cell_p cell = virtualMemory->cells[(j + i * 5)];
            char * types[5] = {"FREE","EDEN","SURVIVE","OLD","HUGE"};
            char *tgr = types[cell->type_flg];
            printf(" |ID:%2d S:%-7s M:%2ld/%db| ",cell->id,tgr,cell->usedIndex,CELL_SIZE);
        }
        putchar('\n');
    }
}

//--------------------MemoryMap-------------------//

struct MemoryMap {
    struct MemoryMapNode *mapHead;
};

MemoryMap_p newMemoryMap() {
    MemoryMap_p tmp = malloc(sizeof(struct MemoryMap));
    tmp->mapHead = NULL;
    return tmp;
}

//--------------------MemoryMapNode-------------------//

typedef struct MemoryMapNode {
    Cell_p cell;
    long byte;
    long realSite_v;
    void *mappingSite_k;
    UT_hash_handle hh;
} *MemoryMapNode_p;

MemoryMapNode_p newMemoryMapNode(long realSite,long byte,Cell_p cell) {
    MemoryMapNode_p tmp = (MemoryMapNode_p) malloc(sizeof(struct MemoryMapNode));
    tmp->realSite_v = realSite;
    tmp->byte = byte;
    tmp->cell = cell;
    char *trs = (char *) malloc(sizeof(getLen(realSite, 10)));
    sprintf(trs, "%ld", realSite);
    tmp->mappingSite_k = (void *) BKDRHash(trs);
    return tmp;
}

