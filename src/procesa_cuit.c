/*
 * Procesa cuit para sacar caracteres no digitos.
 *
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

