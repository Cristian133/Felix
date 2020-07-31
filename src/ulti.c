/*
 * Consulta numero de ultimo comprobante autorizado.
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

#include "ServiceSoap.nsmap"
#include "soapH.h"
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>

/*define string nula*/
#define STR_NULL ""

/*prototipos de funciones*/
char *procesa_cuit(char *cuitemp);
char *extrae_sign(void);
char *extrae_token(void);
double expira_ta(void);
void consul_cae(int CbteTipo,
        int PtoVta,
        long int CbteDesde,
        char *CAE,
        char *CAEFchVto,
        char *FE_Usuario);

/*****main*****/
int main(int argc, char **argv, char **envp)
{

    printf("*****Entramos a ultimo_comp_aut*****\n");

    char pathta[128];

    /*****Deben ser 3 o 4 argumentos*****/
    if(argc < 4) {
        printf("Cantidad incorrecta de argumentos\n");
        exit(EXIT_FAILURE);
    }

    /*****Si existe TA, Analiza validez de fecha*****/
    FILE *ta = fopen("ta.xml","r");
    if((ta == NULL) || (expira_ta() < 60)) {
        printf("El TA actual no es válido o no hay ta.xml\n");
        sprintf(pathta, "./pide_ta.sh %s", *(argv + 4) ? *(argv + 4) : "");
        system(pathta);
    }

    /*****Aloca memoria e Inicializa estructuras SOAP*****/
    struct soap *soap = soap_new();
    const char *soap_endpoint = NULL;
    const char *soap_action = NULL;

    /*****Estructuras Principales*****/
    struct _ns3__FECompUltimoAutorizado UltimoAuthReq;
    struct _ns3__FECompUltimoAutorizado *pUltimoAuthReq = &UltimoAuthReq;
    struct _ns3__FECompUltimoAutorizadoResponse UltimoAuthRes;
    struct _ns3__FECompUltimoAutorizadoResponse *pUltimoAuthRes = &UltimoAuthRes;

    /*****Estructuras Secundarias*****/
    struct  ns3__FEAuthRequest Autoriza;

    /*****Apuntamos Estructuras internas*****/
    pUltimoAuthReq->Auth = &Autoriza;

    /*****Preguntamos si SOAP no tubo problemas****/
    if(!soap) {
        printf("ERROR en librerías gSOAP\n");
        printf("Reinstalar paquetes:\n");
        printf("gsoap-devel libgsoap libgsoap-devel\n");
        exit (EXIT_FAILURE);
    }

    /*****Seteamos estructuras*****/
    printf("Seteamos estructuras\n");
    /*****Autoriza*****/
    pUltimoAuthReq->Auth->Token = extrae_token();
    pUltimoAuthReq->Auth->Sign = extrae_sign();
    pUltimoAuthReq->Auth->Cuit = atoll(procesa_cuit(*(argv + 1)));
    pUltimoAuthReq->PtoVta = atoi(*(argv + 2));
    pUltimoAuthReq->CbteTipo = atoi(*(argv + 3));

    printf("Recibimos: Cuit: %lld\tPtoVta: %d\tCbteTipo: %d\n",
            pUltimoAuthReq->Auth->Cuit,
            pUltimoAuthReq->PtoVta,
            pUltimoAuthReq->CbteTipo);

    /*****Funcion pide tipos UltimoAuth*****/
    printf("Solicita número de último cbte autorizado a afip\n");
    if(soap_call___ns3__FECompUltimoAutorizado(
                soap,
                soap_endpoint,
                soap_action,
                pUltimoAuthReq,
                pUltimoAuthRes) == SOAP_OK) {

        /*****INICIO Rama Verdadera IF*****/
        printf("Llega respuesta de afip\n");
        /*****guarda*****/
        consul_cae(pUltimoAuthRes->FECompUltimoAutorizadoResult->CbteTipo,
                pUltimoAuthRes->FECompUltimoAutorizadoResult->PtoVta,
                pUltimoAuthRes->FECompUltimoAutorizadoResult->CbteNro,
                STR_NULL,
                STR_NULL,
                *(argv + 4) ? *(argv + 4) : "");
    }
    /*****FIN Rama Verdadera IF*****/
    else {
        /*****INICIO Rama Verdadera IF*****/
        printf("ERROR EN RED\n");
        soap_print_fault(soap, stderr);
        printf("ERROR: %s\n", stderr);
        /*****FIN Rama Verdadera IF*****/
    }

    soap_destroy(soap);
    soap_end(soap);
    soap_free(soap);

    printf("*****Salimos de ultimo_comp_aut*****\n");

    return 0;
}
