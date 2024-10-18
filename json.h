#ifndef JSON_H_INCLUDED
#define JSON_H_INCLUDED

enum { J_OBJECT, J_ARRAY, J_NUMBER, J_STRING, J_BOOL, J_NULL };

extern const char* types[];

typedef struct json_t
{
    unsigned char type;
    int children;
    int parent;
    char key[41];
    char value[81];
} json_t;

typedef struct json_parser
{
    struct json_t *tokens;
    uint16_t obj_idx;
    uint16_t brackets[32];
    uint16_t max_tokens;
} json_parser;

void json_parser_init(json_parser *parser, json_t *tokens, uint16_t max_tokens);
int json_parse(const char *buffer, json_parser *parser);
int json_get_value_by_object(json_t *tokens, int num_tokens, char *search_object, char *value);
void print_token(json_t *tokens, int idx);

#endif // JSON_H_INCLUDED
