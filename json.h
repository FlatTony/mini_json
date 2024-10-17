#ifndef JSON_H_INCLUDED
#define JSON_H_INCLUDED

enum { J_OBJECT, J_ARRAY, J_NUMBER, J_STRING, J_BOOL, J_NULL };

extern const char* types[];

typedef struct json_t
{
    unsigned char type;
    int children;
    int parent;
    char key[80];
    char value[80];
} json_t;

int json_parse(const char* buffer, json_t *tokens, const unsigned int num_tokens);
int json_get_value_by_object(json_t *tokens, int num_tokens, char *search_object, char *value);
void print_tokens(json_t *tokens, int num_tokens);

#endif // JSON_H_INCLUDED
