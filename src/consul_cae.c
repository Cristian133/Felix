/*
 * Escribe archivo con respuestas.
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

/*define string nula*/
#define STR_NULL ""

void consul_cae(int CbteTipo,
        int PtoVta,
        long int CbteDesde,
        char *CAE,
        char *CAEFchVto,
        char *FE_Usuario)
{

    printf("*****Entramos a consul_cae*****\n");

    /*strings*/
    char *user = getenv("USER");
    printf("Usuario: %s\n", user);
    printf("FE_Usuario: %s\n", FE_Usuario);
    char strcaespf[128], strcaegen[128], strtime[128];

    /*punteros a FILE*/
    FILE *filecaespf = NULL;     //Especifico "cae_CbteTipo_PtoVta_CbteDesde_CAE_CAEFchVto.txt"
    FILE *filecaegen = NULL;     //General    "cae.txt"

    /*time*/
    time_t fech_actual = time(0);
    struct tm *hora_actual = localtime(&fech_actual);
    strftime(strtime, 64, "%Y%m%d%H%M%S", hora_actual);  //Fecha de Proceso.

    sprintf(strcaegen, "./%s/cae.txt", FE_Usuario);

    /*¿hay CAE?
     * preparamos nombre de archivos*/

    sprintf(strcaespf, "./%s/cae_%02d_%04d_%08ld.txt",
            FE_Usuario,
            CbteTipo,
            PtoVta,
            CbteDesde);

    /* Preguntamos: ¿hay CAE?
     * escribimos en los archivos*/
    if(((filecaespf = fopen(strcaespf, "w")) != NULL) && \
            ((filecaegen = fopen(strcaegen, "w")) != NULL)) {
        if(!strcmp(CAE, STR_NULL)) {
            printf("CAE == STR_NULL\n");
            fprintf(filecaespf,"%02d %04d %08ld 00000000000000 00000000 %s",
                    CbteTipo, PtoVta, CbteDesde, strtime);
            fprintf(filecaegen,"%02d %04d %08ld 00000000000000 00000000 %s",
                    CbteTipo, PtoVta, CbteDesde, strtime);
            printf("Escribe %s\n", strcaespf);
            printf("Escribe %s\n", strcaegen);
            printf("Sin CAE\n");
        }
        else {
            fprintf(filecaespf,"%02d %04d %08ld %s %s %s",
                    CbteTipo, PtoVta, CbteDesde, CAE, CAEFchVto, strtime);
            fprintf(filecaegen,"%02d %04d %08ld %s %s %s",
                    CbteTipo, PtoVta, CbteDesde, CAE, CAEFchVto, strtime);
            printf("Escribe %s\n", strcaespf);
            printf("Escribe %s\n", strcaegen);
            printf("Con CAE\n");
        }
    }
    else
        exit(EXIT_FAILURE);

    fclose(filecaegen);
    fclose(filecaespf);

    printf("*****Salimos de consul_cae*****\n");

    return;
}
