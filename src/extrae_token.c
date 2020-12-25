/*
 * Extrae Token de ta.xml.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

char *extrae_token(void)
{
    char *buf = NULL;
    char *token = NULL;
    short int i = 0;
    short int j = 0;
    short int nro_lin = 0;
    size_t size = 0;

    FILE *ta = fopen("ta.xml","r");

    if(ta != NULL) {
        while((getline(&buf, &size, ta) > 0)) {
            nro_lin++;
            if(nro_lin == 11) {
                token = realloc(buf, size);
                while(*(token + i++) != '>')
                    ;
                while(*(token + i) != '<') {
                    *(token + j++) = *(token + i++);
                }
                *(token + j) = '\0';
                printf("Extrae TOKEN correctamente.\n");

                return token;
            }
        }
    }
    else {
        printf("ERROR abriendo ta.xml");
        printf("en extrae_token\n");

        exit(EXIT_FAILURE);
    }
}
