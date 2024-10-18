#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "json.h"

#define IN_DEBUG 1

#if defined(IN_DEBUG) && IN_DEBUG
#include "utils.h"
#endif // IN_DEBUG

const char* types[] = { "OBJECT", "ARRAY", "NUMBER", "STRING", "BOOL", "NULL" };

void json_parser_init(json_parser *parser, json_t *tokens, uint16_t max_tokens) {
    parser->obj_idx = 0;
    parser->tokens = tokens;
    parser->max_tokens = max_tokens;
}

int json_parse(const char *buffer, json_parser *parser) {

    char c;                                         // Current character
    int i = 0;                                      // Index through the buffer
    uint8_t add_token = 0;                          // Boolean to add token to parser

    while (buffer[i] != 0)
    {
        c = buffer[i];

        switch (c)
        {
            case '"':
            {
                parser->tokens[parser->obj_idx].type = J_STRING;

                // Skip the first "
                i += 1;

                //Catch empty string
                if (buffer[i] == '"') {
                    // Reset value to empty string if it was set somehow
                    memset(parser->tokens[parser->obj_idx].value, '\0', sizeof(parser->tokens[parser->obj_idx].value));
                    // Advance the index past the second "
                    i++;
                    // Still need to set add_token flag so that the token is built out
                    add_token = 1;
                    break;
                }

                parser->tokens[parser->obj_idx].value[0] = buffer[i];

                int j = 1;
                while ((buffer[++i] != '"' || buffer[i-1] == '\\') && buffer[i] != '\0') {
                    if (buffer[i+1] == 0)
                    {
                        printf("ERROR: String missing end quote. Token: %d, Captured %s\n", parser->obj_idx, parser->tokens[parser->obj_idx].value);
                        printf("Previous token: %d %s %s\n", parser->obj_idx-1, types[parser->tokens[parser->obj_idx-1].type], parser->tokens[parser->obj_idx-1].value);
                        return -1;
                    }
                    parser->tokens[parser->obj_idx].value[j] = buffer[i];
                    j++;
                }
                add_token = 1;

            } break;
            case '-':
            case '0' ... '9':
            case 'N':
            case 'n':
            case 'F':
            case 'f':
            case 'T':
            case 't':
            {
                if (c == '-' || (c >= '0' && c <= '9'))
                {
                    parser->tokens[parser->obj_idx].type = J_NUMBER;
                }
                else if (c == 'n' || c == 'N')
                {
                    parser->tokens[parser->obj_idx].type = J_NULL;
                }
                else if (c == 't' || c == 'T' || c == 'f' || c == 'F')
                {
                    parser->tokens[parser->obj_idx].type = J_BOOL;
                }
                else {
                    printf("How did you get here?");
                    return -1;
                }

                int j = 0;
                while (buffer[i+1] != ',' &&
                       buffer[i+1] != '}' &&
                       buffer[i+1] != ']' &&
                       buffer[i+1] != '\n' &&
                       buffer[i+1] != '\r' )
                {
                    parser->tokens[parser->obj_idx].value[j] = buffer[i];
                    j++;
                    i++;
                }
                // Catch the last character
                parser->tokens[parser->obj_idx].value[j] = buffer[i];

                add_token = 1;

            } break;

            case ':':
            {
                if (parser->tokens[parser->obj_idx-1].type == J_STRING) {
                    // This string is a key to the next object/array or value
                    strcpy(parser->tokens[parser->obj_idx-1].key, parser->tokens[parser->obj_idx-1].value);
                    memset(parser->tokens[parser->obj_idx-1].value, '\0', sizeof(parser->tokens[parser->obj_idx-1].value));
                    parser->tokens[parser->tokens[parser->obj_idx-1].parent].children -= 1;
                    parser->obj_idx--;
                }

            } break;
            case '{':
            {
                parser->tokens[parser->obj_idx].type = J_OBJECT;

                // Handle root seperately
                if (parser->obj_idx == 0) {
                    parser->tokens[parser->obj_idx].parent = 65535;
                    strcpy(parser->tokens[parser->obj_idx].key, "root");
                    memmove(&parser->brackets[1], &parser->brackets[0], sizeof(parser->brackets) - sizeof(*parser->brackets));
                    parser->brackets[0] = -1;
                    parser->obj_idx++;
                }
                else {
                    add_token = 1;
                }

            } break;
            case '}':
            {
                memmove(&parser->brackets[0], &parser->brackets[1], sizeof(parser->brackets) - sizeof(*parser->brackets));
                parser->brackets[31] = 0;

            } break;
            case '[':
            {
                parser->tokens[parser->obj_idx].type = J_ARRAY;

                // Handle root seperately
                if (parser->obj_idx == 0) {
                    parser->tokens[parser->obj_idx].parent = 65535;
                    strcpy(parser->tokens[parser->obj_idx].key, "root");
                    memmove(&parser->brackets[1], &parser->brackets[0], sizeof(parser->brackets) - sizeof(*parser->brackets));
                    parser->brackets[0] = -1;
                    parser->obj_idx++;
                }
                else {
                    add_token = 1;
                }

            } break;
            case ']':
            {
                memmove(&parser->brackets[0], &parser->brackets[1], sizeof(parser->brackets) - sizeof(*parser->brackets));
                parser->brackets[31] = 0;

            } break;
            default:
                break;
        }

        // All similar operations for a found token
        if (add_token == 1) {
            parser->tokens[parser->obj_idx].parent = (parser->brackets[0] != 65535) ? parser->brackets[0] : 0;
            parser->tokens[parser->tokens[parser->obj_idx].parent].children += 1;

            if (parser->tokens[parser->obj_idx].type == J_OBJECT || parser->tokens[parser->obj_idx].type == J_ARRAY) {
                memmove(&parser->brackets[1], &parser->brackets[0], sizeof(parser->brackets) - sizeof(*parser->brackets));
                parser->brackets[0] = parser->obj_idx;
            }

            if (parser->tokens[parser->tokens[parser->obj_idx].parent].type == J_ARRAY) {
                strcpy(parser->tokens[parser->obj_idx].key, parser->tokens[parser->tokens[parser->obj_idx].parent].key);
                strcat(parser->tokens[parser->obj_idx].key, "[");
                itoa(parser->tokens[parser->tokens[parser->obj_idx].parent].children-1, parser->tokens[parser->obj_idx].key + strlen(parser->tokens[parser->obj_idx].key), 10);
                strcat(parser->tokens[parser->obj_idx].key, "]");
            }
            parser->obj_idx++;
            add_token = 0;
        }

        if (parser->obj_idx > parser->max_tokens) {
            printf("ERROR: Not enough token memory dedicated.\n");
            return -1;
        }

        i++;
    }

    return 0;
}

int json_get_value_by_object(json_t *tokens, int num_tokens, char *search_object, char *value) {

    char temp_object[256]; // tokenized version of search_object
    char *current_search; // current variable being searched for

    int branch = 0; // top level token index used for searching

    strcpy(temp_object, search_object);
    // remove "root" from the object. All object searches must have root at the head
    current_search = strtok(temp_object, ".");

    while (current_search != NULL) {
        // Set the key that is being searched for
        current_search = strtok(NULL, ".");

        if (current_search == NULL) {
            printf("Object not found: %s\n", search_object);
            return 2;
        }

        // Look through all tokens below the branch for the matching value
        for (int j = branch; j < num_tokens; j++) {
            // Compare the current search value with the values of tokens
            if (strcmp(tokens[j].key, current_search) == 0) {
                // We found the matching object
                if (tokens[j].children == 0) {
                    strncpy(value, tokens[j].value, strlen(tokens[j].value));
                    value[strlen(tokens[j].value)] = '\0';
                    return 0;
                }
                branch = j+1;
                break;
            }
        }
    }
    return 0;
}

void print_token(json_t *tokens, int idx) {
    printf("Token %d, Type: %s, Key: %s, Value: %s, Parent: %d, Children %d\n", idx, types[tokens[idx].type], tokens[idx].key, tokens[idx].value, tokens[idx].parent, tokens[idx].children);
}
