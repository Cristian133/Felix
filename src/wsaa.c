/*
 * Metodo para pedir Ticket de Acceso.
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

#include "LoginCmsSoapBinding.nsmap"
#include "soapH.h"
#include<stdlib.h>

char *file2str(FILE *file);

int main(int argc, char **argv, char **envp)
{

    printf("*****Entramos a wsaa_wsfe*****\n");

    char pathta[128], pathtra[128] ;

    /*****Aloca memoria e Inicializa estructuras SOAP*****/
    struct soap *soap = soap_new();
    const char *soap_endpoint = NULL;
    const char *soap_action = NULL;

    struct _ns1__loginCms wsaa_loginCms;
    struct _ns1__loginCms *pwsaa_loginCms = &wsaa_loginCms;
    struct _ns1__loginCmsResponse wsaa_loginCmsResponse;
    struct _ns1__loginCmsResponse *pwsaa_loginCmsResponse = &wsaa_loginCmsResponse;

    if(!soap) {
        printf("ERROR en librerías gSOAP\n");
        printf("Reinstalar paquetes:\n");
        printf("gsoap-devel libgsoap libgsoap-devel\n");
        exit (EXIT_FAILURE);
    }

    FILE *tra = fopen(*(argv + 1), "r");
    printf("path tra %s\n", *(argv + 1));

    if (!tra) {
        perror(*(argv + 1));
        exit(EXIT_FAILURE);
    };

    pwsaa_loginCms->in0 = file2str(tra);
    //printf("Enviamos %s\n", pwsaa_loginCms->in0);
    fclose(tra);

    /*****FUNCION gSOAP definida en soapClient.c*****/
    printf("Solicita ta a afip\n");
    if (soap_call___ns2__loginCms(soap,
                soap_endpoint,
                soap_action,
                pwsaa_loginCms,
                pwsaa_loginCmsResponse) == SOAP_OK) {

        /*****INICIO Rama Verdadera IF*****/
        printf("Llega respuesta de afip\n");
        //printf("Llega: %s\n", pwsaa_loginCmsResponse->loginCmsReturn);
        FILE *ta = fopen("ta.xml", "w");
        printf("Se escribe ta.xml\n");
        fprintf(ta, "%s", pwsaa_loginCmsResponse->loginCmsReturn);
        fclose(ta);
    }
    else {
        /*****INICIO Rama Falsa IF*****/
        printf("ERROR EN RED\n");
        soap_print_fault(soap, stderr);
    }

    soap_destroy(soap);
    soap_end(soap);
    soap_free(soap);

    printf("*****Salimos de wsaa_wsfe*****\n");

    return 0;
}

/*
 * Convierte el contenido de un archivo a string
 */

char *file2str(FILE *file)
{
    char *buf = NULL;
    char *string = NULL;
    size_t size = 0, acc_size = 0;

    while((getline(&buf, &size, file)) > 0) {
        acc_size += size;
        string = realloc(string, acc_size);
        strcat(string, buf);
    }
    free(buf), buf=NULL;

    return string;
}
