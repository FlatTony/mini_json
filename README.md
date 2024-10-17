# mini json
A small json tokenizer intended to be used in PLC environments with static C support. The parser tokenizes the JSON into usable and searchable objects. After tokenization, the original JSON is not needed due to the tokens containing all relevant information.

---
## Usage
Given the JSON string `{ "object": { "string1" : "Hello", "string2": World }, "array": [0, true, null] }` , the tokenization is as follows:
```
Token 0: Type: OBJECT, Key: root, Value: , Parent: -1, Children: 3
Token 1: Type: OBJECT, Key: object, Value: , Parent: 0, Children: 2
Token 2: Type: OBJECT, Key: string1, Value: Hello, Parent: 1, Children: 0
Token 3: Type: OBJECT, Key: string2, Value: World, Parent: 1, Children: 0
Token 4: Type: ARRAY, Key: array, Value: , Parent: 0, Children: 3
Token 5: Type: NUMBER, Key: array[0], Value: 0, Parent: 4, Children: 0
Token 6: Type: BOOL, Key: array[1], Value: true, Parent: 4, Children: 0
Token 7: Type: NULL, Key: array[2], Value: null, Parent: 4, Children: 0
```

Searching for a specific value is made easy with `json_get_value_by_object`

```
json_t tokens[150]; //tokens
int t;              //number of tokens
char temp[255];     //placeholder for value string

json_get_value_by_object(tokens, t, "root.object.string1", temp) --> temp = "Hello\0"
json_get_value_by_object(tokens, t, "root.array[1]", temp) --> temp = "true\0"
```

## Limitations
1. The number of tokens currently allowed is 255
2. The max size of a key/value is 80 characters
