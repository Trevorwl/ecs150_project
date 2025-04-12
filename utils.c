#include "utils.h"
#include <string.h>
#include <ctype.h>

/* Checks if a string contains only whitespace characters
 *
 * Returns: 1 if string is whitespace, 0 otherwise
 */
int isWhiteSpace(const char* str){
    int sz = strlen(str);

    for(int i = 0; i < sz; i++){
        if(!isspace((unsigned char)str[i])){
            return 0;
        }
    }

    return 1;
}
