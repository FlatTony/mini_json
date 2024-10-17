#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../json.h"
#include "../test/ctest.h"

#define MAX_BUFFER_SIZE 4096

json_t tokens[150];
int t, r;

int main() {

    FILE *fp = fopen("test_json.json","r");
    char buffer[MAX_BUFFER_SIZE+1];

    if ( fp != NULL )
    {
        size_t ret_code = fread(buffer, sizeof(char), MAX_BUFFER_SIZE, fp);
        if ( ferror( fp ) != 0 ) {
            fputs("Error reading file", stderr);
        } else {
            /* Add EOF to be safe. */
            buffer[ret_code++] = '\0';
        }
    }
    else
    {
        printf("Error opening file");
        return 1;
    }

    t = json_parse(buffer, tokens, sizeof(tokens) / sizeof(tokens[0]));

     print_tokens(tokens, t);
     printf("\n");

    char temp[255];
    int ret;

    ret = json_get_value_by_object(tokens, t, "root.string", temp);
    if (ret == 0) {
        assertEQ_s("string", temp, "Hello, World!");
    }

    ret = json_get_value_by_object(tokens, t, "root.array[1]", temp);
    if (ret == 0) {
        assertEQ_s("array[1]", temp, "second");
    }

    ret = json_get_value_by_object(tokens, t, "root.array[5].nestedObject2", temp);
    if (ret == 0) {
        assertEQ_s("array[5].nestedObject2", temp, "seven");
    }

    ret = json_get_value_by_object(tokens, t, "root.object.key3[0]", temp);
    if (ret == 0) {
        assertEQ_s("object.key3[0]", temp, "arrayItem1");
    }

    ret = json_get_value_by_object(tokens, t, "root.object.nestedObject.subKey2", temp);
    if (ret == 0) {
        assertEQ_s("object.nestedObject.subKey2", temp, "10");
    }

    ret = json_get_value_by_object(tokens, t, "root.complexArray[2].item", temp);
    if (ret == 0) {
        assertEQ_s("complexArray[2].item", temp, "C");
    }

    ret = json_get_value_by_object(tokens, t, "root.nestedArrays[1][1].nestedKey", temp);
    if (ret == 0) {
        assertEQ_s("root.nestedArrays[1][1].nestedKey", temp, "nestedValue");
    }

    ret = json_get_value_by_object(tokens, t, "root.mixedTypes[3][1]", temp);
    if (ret == 0) {
        assertEQ_s("root.mixedTypes[3][1]", temp, "2");
    }

    ret = json_get_value_by_object(tokens, t, "root.mixedTypes[4]", temp);
    if (ret == 0) {
        assertEQ_s("root.mixedTypes[4]", temp, "true");
    }

    return 0;

}
