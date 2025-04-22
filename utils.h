#ifndef UTILS_H_
#define UTILS_H_

#include <stdbool.h>

/* Checks if a string contains only whitespace characters
 *
 * Returns: 1 if string is whitespace, 0 otherwise
 */
int isWhiteSpace(const char* str);
int* parseInput(char*);

struct vector{
    int capacity;
    int length;
    void** data;
    void (*destructor)(void*);
};

struct vector* vectorConstructor(void (*destructor)(void*));

void vectorDestructor(struct vector* vector);

void* vector_find(struct vector* vector,int index);

void vector_add(struct vector* vector, void* item);

void vector_remove(struct vector* vector, int index);

void strDestructor(void* ptr);
void intDestructor(void*ptr);



#endif
