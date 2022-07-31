//
// Created by muleisy on 22-7-30.
//

#include "virtualMemory.h"

#ifndef YGC_MY_MALLOC_H
#define YGC_MY_MALLOC_H

#endif //YGC_MY_MALLOC_H


//=========FUNCTION=======//
void gc(VirtualMemory_p virtualMemory);

void myMixGc(VirtualMemory_p virtualMemory);

void fullGc(VirtualMemory_p virtualMemory);

int isSurvive(VirtualMemory_p virtualMemory);

int isEden(VirtualMemory_p virtualMemory);

int isOccupancy(VirtualMemory_p virtualMemory);

int getEdenIndex(VirtualMemory_p virtualMemory);

int getHugeIndex(int len,VirtualMemory_p virtualMemory);

long getRealSite(Cell_p edenCell);

Cell_p mallocEden(long byte, VirtualMemory_p virtualMemory);

Cell_p mallocHuge(long byte, VirtualMemory_p virtualMemory);

Cell_p getEdenCell(long byte, VirtualMemory_p virtualMemory);

//----------------------MyMalloc---------------------------//
void myFree(void *site, VirtualMemory_p virtualMemory) {
    MemoryMap_p memoryMap = virtualMemory->memoryMap;
    MemoryMapNode_p memoryMapNode;
    HASH_FIND_INT(memoryMap->mapHead, &site, memoryMapNode);
    HASH_DELETE(hh, memoryMap->mapHead, memoryMapNode);
}

void *myMalloc(long byte, VirtualMemory_p virtualMemory) {
    Cell_p edenCell = getEdenCell(byte,virtualMemory);
    long realSite = getRealSite(edenCell);
    MemoryMapNode_p memoryMapNode_tmp = newMemoryMapNode(realSite,byte,edenCell);
    HASH_ADD_INT(virtualMemory->memoryMap->mapHead, mappingSite_k, memoryMapNode_tmp);
    return memoryMapNode_tmp->mappingSite_k;
}

long getRealSite(Cell_p edenCell) {
    return edenCell->id * CELL_SIZE;
}

Cell_p getEdenCell(long byte, VirtualMemory_p virtualMemory) {
    Cell_p res = NULL;
    while (isOccupancy(virtualMemory)) {
        printf("GC START!\n");
        gc(virtualMemory);
        virtualMemoryDis(virtualMemory);
    }
    while (res == NULL) {
        if (byte > CELL_SIZE) {
            res = mallocHuge(byte, virtualMemory);
        } else {
            res = mallocEden(byte, virtualMemory);
        }
        if (!res) {
            fullGc(virtualMemory);
        }
    }
    return res;
}

Cell_p mallocEden(long byte, VirtualMemory_p virtualMemory) {
    int index = getEdenIndex(virtualMemory);

    if (index < CELL_COUNT) {
        Cell_p tgrCell = virtualMemory->cells[index];
        tgrCell->type_flg = EDEN;
        tgrCell->usedIndex = byte;
        virtualMemory->freeCellId++;
        return tgrCell;
    } else {
        return NULL;
    }
}

int getEdenIndex(VirtualMemory_p virtualMemory) {
    int i;
    for (i = 0; i < CELL_COUNT; ++i) {
        if(getCell(i,virtualMemory)->type_flg == FREE) {
            return i;
        }
    }
    return -1;
}



Cell_p mallocHuge(long byte, VirtualMemory_p virtualMemory) {
    int len = (int) getLen(byte, CELL_SIZE);
    int index = getHugeIndex(len,virtualMemory);

    if (index + len < CELL_COUNT) {
        Cell_p tgrCell;
        for (int i = 0; i < len; ++i) {
            tgrCell = virtualMemory->cells[index + i];
            setFlg(&(tgrCell->type_flg), HUGE);
            tgrCell->usedIndex = byte - (len - i) * CELL_SIZE;
        }
        tgrCell = virtualMemory->cells[index];
        return tgrCell;
    } else {
        return 0;
    }
}

int getHugeIndex(int len,VirtualMemory_p virtualMemory) {
    return 0;
}

int isOccupancy(VirtualMemory_p virtualMemory) {
    float num = 0;
    for (int i = 0; i < CELL_COUNT; ++i) {
        if(virtualMemory->cells[i]->type_flg !=FREE) {
            num++;
        }
    }
    return (num / (float) CELL_COUNT) > .65;
}

//--------------------------gc---------------------//
void gc(VirtualMemory_p virtualMemory) {
    myMixGc(virtualMemory);
}

int surviveGcCond(MemoryMapNode_p tmp) {
    return tmp->cell->type_flg == SURVIVE || tmp->cell->type_flg == OLD;
}

int edenGcCond(MemoryMapNode_p tmp) {
    return tmp->cell->type_flg == EDEN;
}

void freeCell(Cell_p cell) {
    cell->usedIndex = 0;
    cell->type_flg = FREE;
}

void freeCells(int type,VirtualMemory_p virtualMemory) {
    for (int i = 0; i < CELL_COUNT; ++i) {
        Cell_p tmp = virtualMemory->cells[i];
        if(tmp->type_flg == type && !tmp->protectFlg) {
            freeCell(tmp);
        }
    }
}



void collectGc(int(*cond)(MemoryMapNode_p tmp),int type,VirtualMemory_p virtualMemory) {
    long byteSum = 0;
    LinkedList_p memoryMapNodes = newLinkedList();
    LinkedList_p newCells = newLinkedList();
    for (MemoryMapNode_p tmp = virtualMemory ->memoryMap->mapHead; tmp != NULL; tmp = tmp->hh.next) {
        if (cond(tmp) && tmp->hh.next == NULL && !byteSum + tmp->byte > CELL_SIZE) {
            memoryMapNodes->add(tmp, memoryMapNodes);
            byteSum += tmp->byte;
        }
        if (byteSum + tmp->byte > CELL_SIZE || (tmp->hh.next == NULL && !memoryMapNodes->isEmpty(memoryMapNodes))) {
            int index = getEdenIndex(virtualMemory);
            if(index == -1) {
                printf("-----------FULL　GC------------");
                fullGc(virtualMemory);
                exit(128);
            }
            Cell_p cell = getCell(index, virtualMemory);
            cell->type_flg = type;
            cell->protectFlg = 1;
            for (Iterator_p iterator = memoryMapNodes->getIterator(memoryMapNodes); !iterator->isEmpty(iterator); iterator->toNext(iterator)) {
                MemoryMapNode_p mapNode_tmp = iterator->getNode(iterator);
                mapNode_tmp->cell = cell;
                long dest = CELL_SIZE * cell->id + cell->usedIndex;
                cell->usedIndex +=mapNode_tmp->byte;

                memcpy((void *)(dest + virtualMemory->memory),(void *)(mapNode_tmp->realSite_v + virtualMemory->memory), mapNode_tmp->byte);
                mapNode_tmp->realSite_v = dest;
            }

            newCells->add(cell,newCells);
            virtualMemory->freeCellId = getEdenIndex(virtualMemory);
            byteSum = 0;
            memoryMapNodes = newLinkedList();
        }
        if (cond(tmp)) {
            memoryMapNodes->add(tmp, memoryMapNodes);
            byteSum += tmp->byte;
        }
    }
    if(type == OLD) {
        freeCells(OLD,virtualMemory);
    }
    freeCells(type - 1,virtualMemory);
    for (Iterator_p iterator = newCells->getIterator(newCells); !iterator->isEmpty(iterator); iterator->toNext(iterator)) {
        Cell_p cell = iterator->getNode(iterator);

        cell->protectFlg = 0;
    }
}

void myMixGc(VirtualMemory_p virtualMemory) {
    if (isSurvive(virtualMemory)) {
        collectGc(surviveGcCond,OLD,virtualMemory);
    }
    if (isEden(virtualMemory)) {
        collectGc(edenGcCond,SURVIVE,virtualMemory);
    }
}

int isSurvive(VirtualMemory_p virtualMemory) {
    int res = 0;
    for (int i = 0; i < CELL_COUNT; i++) {
        if (virtualMemory->cells[i]->type_flg == SURVIVE) {
            res++;
        }
    }
    return res;
}

int isEden(VirtualMemory_p virtualMemory) {
    int res = 0;
    for (int i = 0; i < CELL_COUNT; i++) {
        if (virtualMemory->cells[i]->type_flg == EDEN) {
            res++;
        }
    }
    return res;
}

void myFullGc(VirtualMemory_p virtualMemory) {
//=====UNDO=====//
}

void fullGc(VirtualMemory_p virtualMemory) {
    myFullGc(virtualMemory);
}


