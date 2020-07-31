/*
 * Procesa cuit para sacar caracteres no digitos.
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

#include<stdio.h>
#include<stdlib.h>

char *procesa_cuit(char *cuitemp)
{
    int i, j, k;

    printf("******Entramos a procesa_cuit()*****\n");
    printf("cuit recibido de $cuitemp: %s\n", cuitemp);

    for(i=0; *(cuitemp + i)!='\0'; i++);
    char *numdoc = malloc(i * sizeof(char));
    for (j=0, k=0; j<i; j++){
        if (*(cuitemp + j) >= '0' && *(cuitemp + j) <= '9')	{
            numdoc[k] = cuitemp[j];
            k++;
        }
    }
    *(numdoc + k) = '\0';
    printf("cuit procesado: %s\n", numdoc);

    printf("******Salimos de procesa_cuit()*****\n");
    return numdoc;
}

