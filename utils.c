#include "utils.h"
#include "sshell.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

/* Checks if a string contains only whitespace characters
 *
 * Returns: 1 if string is whitespace, 0 otherwise
 */
int isWhiteSpace(const char *str) {
    int sz = strlen(str);

    for (int i = 0; i < sz; i++) {
        if (!isspace((unsigned char)str[i])) {
            return 0;
        }
    }

    return 1;
}

int *parseInput(char *input) {
    int *res = malloc((MAX_CMD_NUM + 1) * sizeof(int));
    res[0] = 0;
    int pos = 1;
    char *now = strchr(input, '|');
    while (now) {
        // 把管道符改写成'\0'
        *now = '\0';
        // 记录下偏移量
        res[pos++] = now - input + 1;
        if (now + 1 != NULL)
            now = strchr(now + 1, '|');
    }

    // 最后放入-1，指示数组结尾
    res[pos] = -1;
    return res;
}

struct vector* vectorConstructor(void (*destructor)(void*)){
    struct vector* vector=(struct vector*)malloc(sizeof(struct vector));
    if(!vector){
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    vector->capacity=10;
    vector->length=0;

    vector->data=(void**)malloc(10 * sizeof(void*));

    if (!(vector->data)) {
       perror("malloc");
       free(vector);
       exit(EXIT_FAILURE);
   }

    vector->destructor=destructor;

    return vector;
}

void vectorDestructor(struct vector* vector){
    for(int i = 0; i < vector->length; i++){
        vector->destructor(vector->data[i]);
        vector->data[i]=NULL;
    }

    free(vector->data);
    vector->data=NULL;

    free(vector);
}

void* vector_find(struct vector* vector,int index){
    return vector->data[index];
}

void resize(struct vector* vector, int newCapacity){
    if(newCapacity<10){
        newCapacity = 10;
    }

    void**newData=(void**)malloc(newCapacity*sizeof(void*));

   if(!newData){
       perror("malloc");
       exit(EXIT_FAILURE);
   }

   for(int i=0;i<vector->length;i++){
       newData[i]=vector->data[i];
   }

   free(vector->data);


   for(int i=vector->length;i<newCapacity;i++){
         newData[i]=NULL;
     }

   vector->data=newData;
   vector->capacity=newCapacity;
}

void vector_add(struct vector* vector,void* item){
    if(vector->length >= vector->capacity){
        resize(vector,vector->capacity*2);
    }

    vector->data[vector->length++]=item;
}

void vector_remove(struct vector* vector, int index){
    vector->destructor(vector->data[index]);

    if(index + 1 < vector->length){

        for(int i = index; i < vector->length - 1; i++){
             vector->data[i]=vector->data[i + 1];
        }
    }

    vector->length--;

    vector->data[vector->length] = NULL;


    if(vector->length > 0 && vector->length <= vector->capacity / 2){
        resize(vector,vector->capacity/2);
    }
}

void strDestructor(void* ptr){
    char* charPtr=(char*)ptr;
    free(charPtr);
}

void intDestructor(void*ptr){
    int* intPtr=(int*)ptr;
    free(intPtr);
}

