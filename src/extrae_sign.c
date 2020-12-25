/*
 * Extrae Sign de ta.xml.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

char *extrae_sign(void)
{
    char *buf = NULL;
    char *sign = NULL;
    short int i = 0;
    short int j = 0;
    short int nro_lin = 0;
    size_t size = 0;

    FILE *ta = fopen("ta.xml","r");

    if(ta != NULL) {
        while((getline(&buf, &size, ta) > 0)) {
            nro_lin++;
            if(nro_lin == 12) {
                sign = realloc(buf, size);
                while(*(sign + i++) != '>')
                    ;
                while(*(sign + i) != '<') {
                    *(sign + j++) = *(sign + i++);
                }
                *(sign + j) = '\0';
                printf("Extrae SIGN correctamente.\n");

                return sign;
            }
        }
    }
    else {
        printf("ERROR abriendo ta.xml");
        printf("en extrae_sign\n");

        exit(EXIT_FAILURE);

    }
}
