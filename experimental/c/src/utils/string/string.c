#include "string.h"

char* Concat(const char* left, const char* right){
	const size_t len1 = strlen(left);
    const size_t len2 = strlen(right);
    char *result = malloc(len1 + len2 + 1);//+1 for the zero-terminator
    //in real code you would check for errors in malloc here
    memcpy(result, left, len1);
    memcpy(result + len1, right, len2 + 1);//+1 to copy the null-terminator

    return result;
}