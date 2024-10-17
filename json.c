#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "json.h"

#define IN_DEBUG 1

#if defined(IN_DEBUG) && IN_DEBUG
#include "utils.h"
#endif // IN_DEBUG

#define MAX_TOKENS      255
#define MAX_BRACKETS    50

const char* types[] = { "OBJECT", "ARRAY", "NUMBER", "STRING", "BOOL", "NULL" };

int json_parse(const char* json, json_t *tokens, const unsigned int num_tokens) {

    static json_t js_tokens[MAX_TOKENS];            // Array of tokens

    static int brackets[MAX_BRACKETS];              // Array of indexes to open curly brackets

    char c;                                         // Current character
    int i = 0;                                      // Index through the buffer
    int obj_idx = 0;                                // Index of the current token

    unsigned char found = 0;

    if (num_tokens > MAX_TOKENS) {
        printf("Too many tokens defined to be used. Max number allowed: %d", MAX_TOKENS);
        return -1;
    }

    while (json[i] != 0)
    {
        c = json[i];

        switch (c)
        {
            case '"':
            {
                js_tokens[obj_idx].type = J_STRING;

                // Skip the first "
                i += 1;

                //Catch empty string
                if (json[i] == '"') {
                    // Reset value to empty string if it was set somehow
                    memset(js_tokens[obj_idx].value, '\0', sizeof(js_tokens[obj_idx].value));
                    // Advance the index past the second "
                    i++;
                    // Still need to set found flag so that the token is built out
                    found = 1;
                    break;
                }

                js_tokens[obj_idx].value[0] = json[i];

                int j = 1;
                while ((json[++i] != '"' || json[i-1] == '\\') && json[i] != '\0') {
                    if (json[i+1] == 0)
                    {
                        printf("ERROR: String missing end quote. Token: %d, Captured %s\n", obj_idx, js_tokens[obj_idx].value);
                        printf("Previous token: %d %s %s\n", obj_idx-1, types[js_tokens[obj_idx-1].type], js_tokens[obj_idx-1].value);
                        return -1;
                    }
                    js_tokens[obj_idx].value[j] = json[i];
                    j++;
                }
                found = 1;

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
                    js_tokens[obj_idx].type = J_NUMBER;
                }
                else if (c == 'n' || c == 'N')
                {
                    js_tokens[obj_idx].type = J_NULL;
                }
                else if (c == 't' || c == 'T' || c == 'f' || c == 'F')
                {
                    js_tokens[obj_idx].type = J_BOOL;
                }
                else {
                    printf("How did you get here?");
                    return -1;
                }

                int j = 0;
                while (json[i+1] != ',' &&
                       json[i+1] != '}' &&
                       json[i+1] != ']' &&
                       json[i+1] != '\n' &&
                       json[i+1] != '\r' )
                {
                    js_tokens[obj_idx].value[j] = json[i];
                    j++;
                    i++;
                }
                // Catch the last character
                js_tokens[obj_idx].value[j] = json[i];

                found = 1;

            } break;

            case ':':
            {
                if (js_tokens[obj_idx-1].type == J_STRING) {
                    // This string is a key to the next object/array or value
                    strcpy(js_tokens[obj_idx-1].key, js_tokens[obj_idx-1].value);
                    memset(js_tokens[obj_idx-1].value, '\0', sizeof(js_tokens[obj_idx-1].value));
                    js_tokens[js_tokens[obj_idx-1].parent].children -= 1;
                    obj_idx--;
                }

            } break;
            case '{':
            {
                js_tokens[obj_idx].type = J_OBJECT;

                // Handle root seperately
                if (obj_idx == 0) {
                    js_tokens[obj_idx].parent = -1;
                    strcpy(js_tokens[obj_idx].key, "root");
                    memmove(&brackets[1], &brackets[0], sizeof(brackets) - sizeof(*brackets));
                    brackets[0] = -1;
                    obj_idx++;
                }
                else {
                    found = 1;
                }

            } break;
            case '}':
            {
                memmove(&brackets[0], &brackets[1], sizeof(brackets) - sizeof(*brackets));
                brackets[MAX_BRACKETS-1] = 0;

            } break;
            case '[':
            {
                js_tokens[obj_idx].type = J_ARRAY;

                // Handle root seperately
                if (obj_idx == 0) {
                    js_tokens[obj_idx].parent = -1;
                    strcpy(js_tokens[obj_idx].key, "root");
                    memmove(&brackets[1], &brackets[0], sizeof(brackets) - sizeof(*brackets));
                    brackets[0] = -1;
                    obj_idx++;
                }
                else {
                    found = 1;
                }

            } break;
            case ']':
            {
                memmove(&brackets[0], &brackets[1], sizeof(brackets) - sizeof(*brackets));
                brackets[MAX_BRACKETS-1] = 0;

            } break;
            default:
                break;
        }

        // All similar operations for a found token
        if (found == 1) {
            js_tokens[obj_idx].parent = (brackets[0] > 0) ? brackets[0] : 0;
            js_tokens[js_tokens[obj_idx].parent].children += 1;

            if (js_tokens[obj_idx].type == J_OBJECT || js_tokens[obj_idx].type == J_ARRAY) {
                memmove(&brackets[1], &brackets[0], sizeof(brackets) - sizeof(*brackets));
                brackets[0] = obj_idx;
            }

            if (js_tokens[js_tokens[obj_idx].parent].type == J_ARRAY) {
                strcpy(js_tokens[obj_idx].key, js_tokens[js_tokens[obj_idx].parent].key);
                strcat(js_tokens[obj_idx].key, "[");
                itoa(js_tokens[js_tokens[obj_idx].parent].children-1, js_tokens[obj_idx].key + strlen(js_tokens[obj_idx].key), 10);
                strcat(js_tokens[obj_idx].key, "]");
            }
            obj_idx++;
            found = 0;
        }

        if (obj_idx > num_tokens) {
            printf("ERROR: Not enough token memory dedicated.\n");
            return -1;
        }

        i += 1;
    }

    // Check that all brackets had a similar closing one
    for (i = 0; i < MAX_BRACKETS; i++) {
        if (brackets[i] != 0) {
            //print_array("brackets", brackets, MAX_BRACKETS);
            printf("ERROR: Token %d is malformed.\n", i);
            return -1;
        }
    }

    memcpy(tokens, &js_tokens, sizeof(js_tokens[0])*(obj_idx));
    return obj_idx;
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

#if defined(IN_DEBUG) && IN_DEBUG
void print_tokens(json_t *tokens, int num_tokens) {
    for (int i = 0; i < num_tokens; i++)
    {
        printf("Token %d, Type: %s, Key: %s, Value: %s, Parent: %d, Children %d\n", i, types[tokens[i].type], tokens[i].key, tokens[i].value, tokens[i].parent, tokens[i].children);
        //printf("Token %d, Type: %s, Key: %s, Value: %s\n", i, types[tokens[i].type], tokens[i].key, tokens[i].value);
    }
}
#endif // IN_DEBUG
