/*
 * Extrae Sign de ta.xml.
 *
 * Copyright (C) 2019  Cristian Andrione.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
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
