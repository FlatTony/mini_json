#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../json.h"
#include "../test/ctest.h"

#define MAX_BUFFER_SIZE 256
#define MAX_TOKENS 80

json_t tokens[MAX_TOKENS];
json_parser parser;

int ret;
char temp[255];

int main() {

    FILE *fp = fopen("test_json.json","r");
    char buffer[MAX_BUFFER_SIZE];

    json_parser_init(&parser, tokens, MAX_TOKENS);

    if ( fp != NULL )
    {
        while (fgets(buffer, MAX_BUFFER_SIZE, fp) != NULL) {
            ret = json_parse(buffer, &parser);
            if (ret != 0) {
                return 2;
            }
            // Uncomment to print out all tokens
            // for (int i = 0; i < parser.obj_idx; i++) {
            //     print_token(parser.tokens, i);
            // }
        }
    }
    else
    {
        printf("Error opening file");
        return 1;
    }

    ret = json_get_value_by_object(parser.tokens, parser.obj_idx, "root.string", temp);
    if (ret == 0) {
        assertEQ_s("string", temp, "Hello, World!");
    }

    ret = json_get_value_by_object(parser.tokens, parser.obj_idx, "root.array[1]", temp);
    if (ret == 0) {
        assertEQ_s("array[1]", temp, "second");
    }

    ret = json_get_value_by_object(parser.tokens, parser.obj_idx, "root.array[5].nestedObject2", temp);
    if (ret == 0) {
        assertEQ_s("array[5].nestedObject2", temp, "seven");
    }

    ret = json_get_value_by_object(parser.tokens, parser.obj_idx, "root.object.key3[0]", temp);
    if (ret == 0) {
        assertEQ_s("object.key3[0]", temp, "arrayItem1");
    }

    ret = json_get_value_by_object(parser.tokens, parser.obj_idx, "root.object.nestedObject.subKey2", temp);
    if (ret == 0) {
        assertEQ_s("object.nestedObject.subKey2", temp, "10");
    }

    ret = json_get_value_by_object(parser.tokens, parser.obj_idx, "root.complexArray[2].item", temp);
    if (ret == 0) {
        assertEQ_s("complexArray[2].item", temp, "C");
    }

    ret = json_get_value_by_object(parser.tokens, parser.obj_idx, "root.nestedArrays[1][1].nestedKey", temp);
    if (ret == 0) {
        assertEQ_s("root.nestedArrays[1][1].nestedKey", temp, "nestedValue");
    }

    ret = json_get_value_by_object(parser.tokens, parser.obj_idx, "root.mixedTypes[3][1]", temp);
    if (ret == 0) {
        assertEQ_s("root.mixedTypes[3][1]", temp, "2");
    }

    ret = json_get_value_by_object(parser.tokens, parser.obj_idx, "root.mixedTypes[4]", temp);
    if (ret == 0) {
        assertEQ_s("root.mixedTypes[4]", temp, "true");
    }

    return 0;

}
