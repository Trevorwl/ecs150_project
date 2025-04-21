#include "utils.h"
#include "sshell.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
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