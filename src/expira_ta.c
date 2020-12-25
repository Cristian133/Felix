/*
 * Chequea validez de ta.xml actual.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

double expira_ta(void)
{
    printf("*****Entramos a expira_ta*****\n");

    char *buf = NULL;
    char *fech_exp = NULL;
    short int j = 0;
    short int i = 0;
    short int nro_lin = 0;
    size_t size = 0;
    FILE *ta = fopen("ta.xml", "r");

    printf("Abrimos ta.xml\n");

    if(ta == NULL) {
        printf("ERROR abriendo ta.xml\n");
        exit(EXIT_FAILURE);
    }
    while((getline(&buf, &size, ta) > 0)) {
        nro_lin++;
        if(nro_lin == 8) {
            fech_exp = realloc(buf, size);
            while(*(fech_exp + i++) != '>')
                ;
            while(*(fech_exp + i) != '<') {
                *(fech_exp + j++) = *(fech_exp + i++);
            }
            *(fech_exp + j) = '\0';
            printf("fecha de exp: %s\n", fech_exp);
            break;
        }
    }
    fclose(ta);

    time_t fech_actual = time(0);
    struct tm fech_ex;
    //struct tm *hora_actual = localtime(&fech_actual);

    /*****año*****/
    char str_year[5];
    for(i = 0; i < 4; i++)
        *(str_year + i) = *(fech_exp + i);
    *(str_year + i) = '\0';
    short int year_ta = atoi(str_year);
    fech_ex.tm_year = year_ta - 1900;

    /*****mes******/
    char str_mes[3];
    for(i = 5, j = 0; i < 7; i++, j++)
        *(str_mes + j) = *(fech_exp + i);
    *(str_mes + j) = '\0';
    short int mes_ta = atoi(str_mes);
    fech_ex.tm_mon = mes_ta - 1;

    /*****dia*****/
    char str_dia[3];
    for(i = 8, j = 0; i < 10; i++, j++)
        *(str_dia + j) = *(fech_exp + i);
    *(str_dia + j) = '\0';
    short int dia_ta = atoi(str_dia);
    fech_ex.tm_mday = dia_ta;

    /*****hora*****/
    char str_hora[3];
    for(i = 11, j = 0; i < 13; i++, j++)
        *(str_hora + j) = *(fech_exp + i);
    *(str_hora + j) = '\0';
    short int hora_ta = atoi(str_hora);
    fech_ex.tm_hour = hora_ta;

    /*****minuto*****/
    char str_min[3];
    for(i = 14, j = 0; i < 16; i++, j++)
        *(str_min + j) = *(fech_exp + i);
    *(str_min + j) = '\0';
    short int min_ta = atoi(str_min);
    fech_ex.tm_min = min_ta;

    /*****segundo*****/
    char str_sec[3];
    for(i = 17, j = 0; i < 19; i++, j++)
        *(str_sec + j) = *(fech_exp + i);
    *(str_sec + j) = '\0';
    short int sec_ta = atoi(str_sec);
    fech_ex.tm_sec = sec_ta;

    /*****horario de verano*****/
    fech_ex.tm_isdst = -1;

    double fech_ex_sec = mktime(&fech_ex);

    printf("%s\n", difftime(fech_ex_sec, fech_actual)>60?"El TA actual es válido":"El TA ha expirado");
    printf("*****Salimos de expira_ta*****\n");

    return difftime(fech_ex_sec, fech_actual);
}
