//
// Created by muleisy on 22-5-30.
//

#ifndef MYCOMPILER_MYLINKEDLIST_H
#define MYCOMPILER_MYLINKEDLIST_H

#include <malloc.h>
#include "Exception.h"

typedef struct ListNode *ListNode_p;

struct ListNode {
    void *element;
    ListNode_p next;
    ListNode_p prev;
};

ListNode_p newNode(void *e, ListNode_p next, ListNode_p prev) {
    ListNode_p newNode = (ListNode_p) malloc(sizeof(struct ListNode));
    newNode->next = next;
    newNode->prev = prev;
    newNode->element = e;
    return newNode;
}

void unLink(ListNode_p node) {
    if(node == NULL) {
        nullPointerException();
        return;
    }
    if (node->prev != NULL) {
        node->prev->next = node->next;
    }
    if (node->next != NULL) {
        node->next->prev = node->prev;
    }
    ListNode_p tmpNode = node->next;
//    free(iterator->head);
}

typedef struct LinkedList *LinkedList_p;

typedef ListNode_p (*add_FUN)(void *element,LinkedList_p list);

ListNode_p add(void *element,LinkedList_p list);

typedef struct Iterator *Iterator_p;

Iterator_p newIterator(ListNode_p head);

Iterator_p getIterator(LinkedList_p list);

int isEmpty_(LinkedList_p list);

typedef Iterator_p (*getIterator_FUN)(LinkedList_p list);
typedef int (*isEmpty__FUN)(LinkedList_p list);

struct LinkedList {
    ListNode_p head;
    ListNode_p tail;

    add_FUN add;
    getIterator_FUN getIterator;
    isEmpty__FUN isEmpty;
};

LinkedList_p newLinkedList() {
    LinkedList_p tmp = (LinkedList_p) malloc(sizeof(struct LinkedList));
    tmp->add = add;
    tmp->getIterator = getIterator;
    tmp->isEmpty = isEmpty_;
    return tmp;
}

int isEmpty_(LinkedList_p list) {
    return list->head == NULL ? 1 : 0;
}

ListNode_p add(void *element,LinkedList_p list) {
    if (list->head == NULL) {
        list->head = list->head = list->tail = newNode(element, NULL, NULL);
    } else {
        list->tail->next = newNode(element, NULL, list->tail);
        list->tail = list->tail->next;
    }
    return list->tail->element;
}

Iterator_p getIterator(LinkedList_p list) {
    return newIterator(list->head);
}

void insert(void *element, Iterator_p iterator) ;

void delete(Iterator_p iterator);

void* getNode(Iterator_p iterator);

int isEmpty(Iterator_p iterator);

void* toNext(Iterator_p iterator);

typedef void (*insert_iterator_FUN)(void *element,Iterator_p iterator);

typedef void (*delete_iterator_FUN)(Iterator_p iterator);

typedef void* (*getNext_FUN)(Iterator_p iterator);

typedef int (*isEmpty_FUN)(Iterator_p iterator);

typedef void* (*toNext_FUN)(Iterator_p iterator);

struct Iterator{
    ListNode_p head;

    insert_iterator_FUN insert;
    delete_iterator_FUN delete;
    getNext_FUN getNode;
    isEmpty_FUN isEmpty;
    toNext_FUN toNext;
};

Iterator_p newIterator(ListNode_p head) {
    Iterator_p tmp = (Iterator_p) malloc(sizeof(struct Iterator));
    tmp->head = head;
    tmp->insert = insert;
    tmp->getNode = getNode;
    tmp->delete = delete;
    tmp->isEmpty = isEmpty;
    tmp->toNext = toNext;
    return tmp;
}

void delete(Iterator_p iterator) {
    if(iterator->head == NULL) {
        nullPointerException();
        return;
    }
    if (iterator->head->prev != NULL) {
        iterator->head->prev->next = iterator->head->next;
    }
    if (iterator->head->next != NULL) {
        iterator->head->next->prev = iterator->head->prev;
    }
    ListNode_p tmpNode = iterator->head->next;
//    free(iterator->head);
    iterator->head = tmpNode;
}

void insert(void *element, Iterator_p iterator) {
    if (iterator->head == NULL) {
        nullPointerException();
        return;
    } else {
        ListNode_p tmpNext = iterator->head->next;
        iterator->head->next = newNode(element, tmpNext, iterator->head);
        if (tmpNext != NULL) {
            tmpNext->prev = iterator->head->next;
        }
    }
}

void* toNext(Iterator_p iterator) {
    iterator->head = iterator->head->next == NULL ? NULL : iterator->head->next;
    return iterator->head;
}

void* getNode(Iterator_p iterator) {
    if(!isEmpty(iterator)) {
        return iterator->head->element;
    } else {
        return NULL;
    }
}

int isEmpty(Iterator_p iterator) {
    return iterator->head == NULL ? 1 : 0;
}

void mylinkTest() {
    LinkedList_p link = newLinkedList();
    for (int i = 0; i < 10; ++i) {
        int * tmpE = (int*) malloc(sizeof (int ));
        *tmpE = i;
        link->add(tmpE,link);
    }
    int i = 0;

    for (Iterator_p iterator = link->getIterator(link); !iterator->isEmpty(iterator); iterator->toNext(iterator)) {
        int *e = (int *) iterator->getNode(iterator);
        if(i % 2 == 0) {
            int * tmpE = (int*) malloc(sizeof (int ));
            *tmpE = 999;
            iterator->insert(tmpE,iterator);
        }
        printf(" %d ", *e);
        i++;
    }
}
#endif //MYCOMPILER_MYLINKEDLIST_H
