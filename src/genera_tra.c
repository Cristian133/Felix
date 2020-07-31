/*
 * Genera Requerimiento de Ticket de Acceso.
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
#include <string.h>
#include <time.h>

//
//Crea mensaje XML para wsaa de AFIP.
//

int main(int argc, char **argv)     //main recibe como argumento la string $user

{
    printf("*****Entramos a genera_tra*****\n");

    char pathtra[128];

    /* Unique ID entero de 32 bits que junto con gen_time
       identifica al requerimiento. */
    time_t id = time(NULL);
    char gen_time[32], exp_time[32];

    /* Momento en que fue generado el requerimiento. */
    time_t tick_time = 1200; //En segundos.
    time_t fech_gen = time(0) - tick_time;
    struct tm *hora_gen = localtime(&fech_gen);
    strftime(gen_time, 32, "%Y-%m-%dT%H:%M:%S-03:00", hora_gen);

    //Momento en que expira la solicitud.
    time_t fech_exp = time(0) + tick_time;
    struct tm *hora_exp = localtime(&fech_exp);
    strftime(exp_time, 32, "%Y-%m-%dT%H:%M:%S-03:00", hora_exp);

    char service[5] = "wsfe";

    /***** Genera STRING XML TRA*****/
    sprintf(pathtra, "./%s/tra.xml", *(argv + 1)?*(argv + 1):"");
    FILE *xml_tra = fopen(pathtra, "w");
    printf("Escribimos %s\n", pathtra);


    fprintf(xml_tra, "<?xml version=\"1.0\" encoding=\"UTF-8\"?><loginTicketRequest version=\"1.0\"><header><uniqueId>%lu</uniqueId><generationTime>%s</generationTime><expirationTime>%s</expirationTime></header><service>%s</service></loginTicketRequest>", id, gen_time, exp_time, service);

    fclose(xml_tra);

    printf("*****Salimos de genera_tra*****\n");

    return 0;
}
