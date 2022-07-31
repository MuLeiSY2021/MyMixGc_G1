//
// Created by muleisy on 22-7-29.
//

#ifndef YGC_TOOL_H
#define YGC_TOOL_H

unsigned int getLen(long num, int top) {
    int res = 0;
    while (num) {
        num /= top;
        ++res;
    }
    return res;
}

unsigned long BKDRHash(char *str) {
    unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;

    while (*str) {
        hash = hash * seed + (*str++);
    }

    return (hash & 0x7FFFFFFF);
}

#endif //YGC_TOOL_H