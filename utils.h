#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

void prepend(char* s, const char* t) {
    size_t len = strlen(t);
    memmove(s + len, s, strlen(s) + 1);
    memcpy(s, t, len);
}

void extract_between(const char *str, const char *p1, const char *p2, char *ret) {
    const char *i1 = strstr(str, p1);
    if(i1 != NULL)
    {
        const size_t pl1 = strlen(p1);
        const char *i2 = strstr(i1 + pl1, p2);
        if(p2 != NULL)
        {
            /* Found both markers, extract text. */
            const size_t mlen = i2 - (i1 + pl1);

            if(ret != NULL)
            {
                strncpy(ret, i1 + pl1, mlen);
                ret[mlen] = '\0';
            }
        }
    }
}

void print_array(char *name, int *arr, int elements) {
    printf("%s: [ ", name);
    for (int k = 0; k < elements; k++) {
        printf("%d ", arr[k]);
    }
    printf("]\n");

}

#endif // UTILS_H_INCLUDED
