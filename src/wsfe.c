/*
 * Metodo encargado de recibir los argumentos que conforman un comprobante
 * y pedir CAE a AFIP.
 *
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

#define FALSE 0
#define TRUE  1

/*****Salva calling enviroment*****/
static jmp_buf env_err;
static jmp_buf env_evt;

/*****Prototipos de funciones*****/
char *procesa_cuit(char *cuitemp);
char *extrae_sign(void);
char *extrae_token(void);
double expira_ta(void);
void guarda_cae(int CbteTipo,
        int PtoVta,
        long int CbteDesde,
        char *CAE,
        char *CAEFchVto,
        char *FE_Usuario);

/*****Manejadores de señales*****/
void handle_SIGSEGV_Err(int signal);
void handle_SIGSEGV_Evt(int signal);

/*****main*****/
int main(int argc, char **argv, char **envp)
{

    printf("*****Entramos a wsfe*****\n");
    int i, cbte_a, cbte_b, cbte_c, cbte_m, trib;
    char pathta[128];

    /*****Deben ser 27 o 28 argumentos*****/
    if(argc < 27) {
        printf("Cantidad incorrecta de argumentos.\n");
        exit(EXIT_FAILURE);
    }

    printf("*(argv + 28) = %s\n", *(argv + 28)?*(argv + 28):"");

    /*strings*/
    char *user = getenv("USER");
    char strreqcae[128];
    char strtime[128];

    /*punteros a FILE*/
    FILE *filereqcae = NULL;     // "Request.txt"

    /*time*/
    time_t fechActual = time(0);
    struct tm *horaActual = localtime(&fechActual);
    strftime(strtime, 128, "%Y%m%d%H%M%S", horaActual);  //Fecha de Proceso.

    sprintf(strreqcae, "./%s/Request.xml", *(argv + 28)?*(argv + 28):"");

    /* escribimos en los archivos*/
    if((filereqcae = fopen(strreqcae, "w")) != NULL)
        fprintf(filereqcae, "Usuario:\t%s\n", user);
    else
        exit(EXIT_FAILURE);

    /*****Si existe TA, Analiza validez de fecha*****/
    FILE *ta = fopen("ta.xml","r");
    if((ta == NULL) || (expira_ta() < 60)) {
        printf("El TA ha expirado o no se encuentra ta.xml\n");
        sprintf(pathta, "./pide_ta.sh %s", *(argv + 28)?*(argv + 28):"");
        system(pathta);
    }

    /*****Aloca memoria e Inicializa estructuras SOAP*****/
    struct soap *soap = soap_new();
    const char *soap_endpoint = NULL;
    const char *soap_action = NULL;

    /*****Preguntamos si SOAP no tubo problemas****/
    if(!soap) {
        printf("ERROR en librerías gSOAP\n");
        printf("Reinstalar paquetes:\n");
        printf("gsoap-devel libgsoap libgsoap-devel\n");
        exit (EXIT_FAILURE);
    }

    /*****Si es Comprobante A****/
    cbte_a = FALSE;
    if(atoi(*(argv + 5)) == 1 || atoi(*(argv + 5)) == 2 || atoi(*(argv + 5)) == 3){
        printf("Comprobante tipo A: %d\n", atoi(*(argv + 5)));
        cbte_a = TRUE;
    }
    printf("cbte_a = %s\n", (cbte_a == 0)?"FALSE":"TRUE");

    /*****Si es Comprobante B****/
    cbte_b = FALSE;
    if(atoi(*(argv + 5)) == 6 || atoi(*(argv + 5)) == 7 || atoi(*(argv + 5)) == 8){
        printf("Comprobante tipo B: %d\n", atoi(*(argv + 5)));
        cbte_b = TRUE;
    }
    printf("cbte_b = %s\n", (cbte_b == 0)?"FALSE":"TRUE");

    /*****Si es Comprobante C****/
    cbte_c = FALSE;
    if(atoi(*(argv + 5)) == 11 || atoi(*(argv + 5)) == 12 || atoi(*(argv + 5)) == 13){
        printf("Comprobante tipo C: %d\n", atoi(*(argv + 5)));
        cbte_c = TRUE;
    }
    printf("cbte_c = %s\n", (cbte_c == 0)?"FALSE":"TRUE");

    /*****Si es Comprobante M****/
    cbte_m = FALSE;
    if(atoi(*(argv + 5)) == 51 || atoi(*(argv + 5)) == 52 || atoi(*(argv + 5)) == 53){
        printf("Comprobante tipo M: %d\n", atoi(*(argv + 5)));
        cbte_m = TRUE;
    }
    printf("cbte_m = %s\n", (cbte_m == 0)?"FALSE":"TRUE");

    /*****Si hay Tributos****/
    trib = FALSE;
    if(atoi(*(argv + 27)) > (double) 0){
        printf("Hay Tributo => ");
        trib = TRUE;
    }
    printf("trib = %s\n", (trib==0)?"FALSE":"TRUE");

    /*****Declaraciones de estructuras definidas por gsoap*****/
    /*****Estructuras Principales*****/
    struct _ns3__FECAESolicitar Solicita;
    struct _ns3__FECAESolicitar *pSolicita = &Solicita;
    struct _ns3__FECAESolicitarResponse Responde;
    struct _ns3__FECAESolicitarResponse *pResponde = &Responde;

    /*****Estructura Internas*****/
    struct  ns3__FEAuthRequest Autoriza;
    struct  ns3__FECAERequest Cuerpo;
    struct  ns3__FECAECabRequest Cabecera;
    struct  ns3__ArrayOfFECAEDetRequest Determinante;
    struct  ns3__FECAEDetRequest Matriz;

    /*****Apuntamos Estructuras internas*****/
    pSolicita->Auth = &Autoriza;
    pSolicita->FeCAEReq = &Cuerpo;
    pSolicita->FeCAEReq->FeCabReq = &Cabecera;
    pSolicita->FeCAEReq->FeDetReq = &Determinante;
    pSolicita->FeCAEReq->FeDetReq->__sizeFECAEDetRequest = (int) 1;
    pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest = &Matriz;

    pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->CbtesAsoc = NULL;
    pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Opcionales = NULL;
    pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Compradores = NULL;

    /*****Estructuras Opcionales*****/
    /*****Alicuotas de Iva*****/
    struct ns3__ArrayOfAlicIva ArrayIva;
    struct ns3__AlicIva EstIva;
    int __sizeAlicIva;

    /*****Cantidad de Alicuotas de Iva*****/
    if(!cbte_c && fabs(atof(*(argv +14)) - atof(*(argv + 9))) > 0.0001 && fabs(atof(*(argv +10)) - atof(*(argv + 9))) > 0.0001) {
        //entra si no es cbte_c ni es cualquier otro con importe exento igual a importe total o importe neto no grabado igual a importe total.
        if((atof(*(argv + 19)) > (double) 0) && (atof(*(argv + 21)) > (double) 0)) {
            __sizeAlicIva = (int) 2;
            printf("Se informan dos Alicuotas de IVA: sizeAlicIva = 2\n");
        }
        else {
            __sizeAlicIva = (int) 1;
            printf("Se informa una Alicuota de IVA: sizeAlicIva = 1\n");
        }
    }
    else
        printf("No se informan Alicuotas de IVA\n");

    /*****Tributos*****/
    struct ns3__ArrayOfTributo ArrayTributos;
    struct ns3__Tributo EstTributo;
    int __sizeTributo;

    /*****Cantidad de Tributos*****/
    if(trib)
        __sizeTributo = (int) 1;

    /*****Reservamos memoria a  array Alicuota de Iva si no es Comprobante tipo C*****/
    if(!cbte_c && fabs(atof(*(argv +14)) - atof(*(argv + 9))) > 0.0001 && fabs(atof(*(argv +10)) - atof(*(argv + 9))) > 0.0001) {
        //entra si no es cbte_c ni es cualquier otro con importe exento igual a importe total o importe neto no grabado igual a importe total.
        pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Iva = &ArrayIva;
        pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Iva->__sizeAlicIva = __sizeAlicIva;
        pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Iva->AlicIva = (struct ns3__AlicIva *) soap_malloc(soap, __sizeAlicIva*sizeof(EstIva));
    }
    else
        pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Iva = NULL;

    /*****Reservamos memoria para el array Tributos*****/
    if(trib){
        pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Tributos = &ArrayTributos;
        pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Tributos->Tributo = &EstTributo;
        pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Tributos->__sizeTributo = __sizeTributo;
        pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Tributos->Tributo = (struct ns3__Tributo *) soap_malloc(soap, __sizeTributo*sizeof(EstTributo));
    }
    else
        pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Tributos = NULL;

    /*****Comenzamos a setear elementos de estructuras*****/
    /*****Autoriza*****/
    pSolicita->Auth->Token  = extrae_token();
    pSolicita->Auth->Sign   = extrae_sign();
    pSolicita->Auth->Cuit   = atoll(procesa_cuit(*(argv + 1)));
    printf("CUIT:\t%lld\n", pSolicita->Auth->Cuit);
    fprintf(filereqcae, "CUIT:\t%lld\n", pSolicita->Auth->Cuit);

    /*****Cabecera*****/
    pSolicita->FeCAEReq->FeCabReq->CantReg  = (int) 1;
    pSolicita->FeCAEReq->FeCabReq->PtoVta   = atoi(*(argv + 6));
    fprintf(filereqcae, "PtoVta:\t%d\n", pSolicita->FeCAEReq->FeCabReq->PtoVta);
    pSolicita->FeCAEReq->FeCabReq->CbteTipo = atoi(*(argv + 5));
    fprintf(filereqcae, "CbteTipo:\t%d\n", pSolicita->FeCAEReq->FeCabReq->CbteTipo);

    /*****Cuerpo*****/
    if((atoi(*(argv + 2)) == 0) || (atoi(*(argv + 2)) == 1)) {
        pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Concepto = (int) 1;
        fprintf(filereqcae, "Concepto:\t%d\n", pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Concepto);
    }
    else {
        pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Concepto= atoi(*(argv + 2));
        fprintf(filereqcae, "Concepto:\t%d\n", pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Concepto);
    }

    pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->DocTipo     = atoi(*(argv + 3));
    fprintf(filereqcae, "DocTipo:\t%d\n", pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->DocTipo);
    pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->DocNro      = atoll(*(argv + 4));
    fprintf(filereqcae, "DocNro:\t%lld\n", pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->DocNro);
    pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->CbteDesde   = atoll(*(argv + 7));
    fprintf(filereqcae, "CbteDesde:\t%lld\n", pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->CbteDesde);
    pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->CbteHasta   = atoll(*(argv + 8));
    fprintf(filereqcae, "CbteHasta:\t%lld\n", pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->CbteHasta);

    /*****FECAEDetRequest*****/
    pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->CbteFch     =      *(argv +15);
    fprintf(filereqcae, "CbteFch:\t%s\n", pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->CbteFch);
    pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->ImpTotal    = atof(*(argv + 9));
    fprintf(filereqcae, "ImpTotal:\t%.2f\n", pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->ImpTotal);
    pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->ImpTotConc  = atof(*(argv +10));
    fprintf(filereqcae, "ImpTotConc:\t%.2f\n", pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->ImpTotConc);
    pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->ImpNeto     = atof(*(argv +11));
    fprintf(filereqcae, "ImpNeto:\t%.2f\n", pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->ImpNeto);
    pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->ImpOpEx     = atof(*(argv +14));
    fprintf(filereqcae, "ImpOpEx:\t%.2f\n", pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->ImpOpEx);
    pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->ImpTrib     = atof(*(argv +27));
    fprintf(filereqcae, "ImpTrib:\t%.2f\n", pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->ImpTrib);
    pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->ImpIVA      = (int) ((atof(*(argv +20)) + atof(*(argv +22))) * 100 + 0.50)/ (double) 100;
    fprintf(filereqcae, "ImpIVA:\t%f\n", pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->ImpIVA);

    if(( pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Concepto == 2) || (pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Concepto == 3)) {
        pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->FchServDesde = *(argv +16);
        fprintf(filereqcae, "FchServDesde:\t%s\n", pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->FchServDesde);
        pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->FchServHasta = *(argv +17);
        fprintf(filereqcae, "FchServHasta:\t%s\n", pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->FchServHasta);
        pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->FchVtoPago   = *(argv +18);
        fprintf(filereqcae, "FchVtoPago:\t%s\n", pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->FchVtoPago);
    }
    else {
        pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->FchServDesde = NULL;
        fprintf(filereqcae, "FchServDesde:\t%s\n", pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->FchServDesde);
        pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->FchServHasta = NULL;
        fprintf(filereqcae, "FchServHasta:\t%s\n", pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->FchServHasta);
        pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->FchVtoPago   = NULL;
        fprintf(filereqcae, "FchServDesde:\t%s\n", pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->FchServDesde);
    }

    pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->MonId        = "PES";
    fprintf(filereqcae, "MonId:\t%s\n", pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->MonId);
    pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->MonCotiz     = (double)1;
    fprintf(filereqcae, "MonCotiz:\t%.2f\n", pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->MonCotiz);

    if(!cbte_c && fabs(atof(*(argv +14)) - atof(*(argv + 9))) > 0.0001 && fabs(atof(*(argv +10)) - atof(*(argv + 9))) > 0.0001) {
        //entra si no es cbte_c ni es cualquier otro con importe exento igual a importe total o importe neto no grabado igual a importe total.
        /*****Iva 0%*****/
        if( pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->ImpIVA == 0) {
            pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Iva->AlicIva[0].Id      = (int) 3;
            fprintf(filereqcae, "Id.AlicIva:\t%d\n", pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Iva->AlicIva[0].Id);
            pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Iva->AlicIva[0].BaseImp = (double) 0;
            fprintf(filereqcae, "BaseImp.AlicIva:\t%.2f\n", pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Iva->AlicIva[0].BaseImp);
            pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Iva->AlicIva[0].Importe = (double) 0;
            fprintf(filereqcae, "Importe.AlicIva:\t%.2f\n", pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Iva->AlicIva[0].Importe);
        }

        /*****Iva21%*****/
        if(atof(*(argv + 19)) > (double) 0) {
            pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Iva->AlicIva[0].Id      = (int) 5;
            fprintf(filereqcae, "Id.AlicIva:\t%d\n", pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Iva->AlicIva[0].Id);
            pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Iva->AlicIva[0].BaseImp = atof(*(argv + 19));
            fprintf(filereqcae, "BaseImp.AlicIva:\t%.2f\n", pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Iva->AlicIva[0].BaseImp);
            pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Iva->AlicIva[0].Importe = atof(*(argv + 20));
            fprintf(filereqcae, "Importe.AlicIva:\t%.2f\n", pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Iva->AlicIva[0].Importe);
            /*****Iva10.5%*****/
            if(atof(*(argv + 21)) > (double) 0) {
                pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Iva->AlicIva[1].Id      = (int) 4;
                fprintf(filereqcae, "Id.AlicIva:\t%d\n", pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Iva->AlicIva[1].Id);
                pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Iva->AlicIva[1].BaseImp = atof(*(argv + 21));
                fprintf(filereqcae, "BaseImp.AlicIva:\t%.2f\n", pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Iva->AlicIva[1].BaseImp);
                pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Iva->AlicIva[1].Importe = atof(*(argv + 22));
                fprintf(filereqcae, "Importe.AlicIva:\t%.2f\n", pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Iva->AlicIva[1].Importe);
            }
        }

        /*****Iva10.5% && no Iva21%*****/
        if(!(atof(*(argv + 19)) > (double) 0) && (atof(*(argv + 21)) > (double) 0)) {
            pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Iva->AlicIva[0].Id      = (int) 4;
            fprintf(filereqcae, "Id.AlicIva:\t%d\n", pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Iva->AlicIva[0].Id);
            pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Iva->AlicIva[0].BaseImp = atof(*(argv + 21));
            fprintf(filereqcae, "BaseImp.AlicIva:\t%.2f\n", pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Iva->AlicIva[0].BaseImp);
            pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Iva->AlicIva[0].Importe = atof(*(argv + 22));
            fprintf(filereqcae, "Importe.AlicIva:\t%.2f\n", pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Iva->AlicIva[0].Importe);
        }
    }

    if(trib){
        /*****Tributos*****/
        pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Tributos->Tributo->Id       = (short) atoi(*(argv + 23));
        fprintf(filereqcae, "Id.Tributo:\t%d\n", pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Tributos->Tributo->Id);
        pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Tributos->Tributo->Desc     = *(argv + 24);
        fprintf(filereqcae, "Desc.Tributo:\t%s\n", pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Tributos->Tributo->Desc);
        pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Tributos->Tributo->BaseImp  = atof(*(argv + 25));
        fprintf(filereqcae, "BaseImp.Tributo:\t%.2f\n", pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Tributos->Tributo->BaseImp);
        pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Tributos->Tributo->Alic     = atof(*(argv + 26));
        fprintf(filereqcae, "Alic.Tributo:\t%.2f\n", pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Tributos->Tributo->Alic);
        pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Tributos->Tributo->Importe  = atof(*(argv + 27));
        fprintf(filereqcae, "Importe.Tributo:\t%.2f\n", pSolicita->FeCAEReq->FeDetReq->FECAEDetRequest->Tributos->Tributo->Importe);
    }

    /*****FUNCION gSOAP definida en soapClient.c*****/
    printf("Solicita CAE a AFIP\n");
    if(soap_call___ns3__FECAESolicitar(
                soap,
                soap_endpoint,
                soap_action,
                pSolicita,
                pResponde) == SOAP_OK) {

        /*****INICIO Rama Verdadera IF*****/
        printf("Llega respuesta de afip\n");
        /*****Escribe Resultado*****/
        char resultado[128];
        sprintf(resultado, "./%s/Logs/%02d_%04d_%08ld.xml",
                *(argv + 28)?*(argv + 28):"",
                pResponde->FECAESolicitarResult->FeCabResp->CbteTipo,
                pResponde->FECAESolicitarResult->FeCabResp->PtoVta,
                pResponde->FECAESolicitarResult->FeDetResp->FECAEDetResponse->CbteDesde);

        printf("Leemos respuesta de afip\n");
        FILE *res = fopen(resultado, "w");
        sprintf(resultado, "./%s/error.txt", *(argv + 28)?*(argv + 28):"");
        FILE *e = fopen(resultado, "w");
        if((res != NULL) && (e != NULL)) {
            fprintf(res, "\n\n\nCabecera\n\n");
            fprintf(res, "    Cuit: %lld\n", pResponde->FECAESolicitarResult->FeCabResp->Cuit);
            fprintf(res, "    PtoVta: %d\n", pResponde->FECAESolicitarResult->FeCabResp->PtoVta);
            fprintf(res, "    CbteTipo: %d\n", pResponde->FECAESolicitarResult->FeCabResp->CbteTipo);
            fprintf(res, "    FechaProc:  %s\n", pResponde->FECAESolicitarResult->FeCabResp->FchProceso);
            fprintf(res, "    CantReg: %d\n", pResponde->FECAESolicitarResult->FeCabResp->CantReg);
            fprintf(res, "    Resultado: %s\n", pResponde->FECAESolicitarResult->FeCabResp->Resultado);
            fprintf(res, "    Reproceso: %s\n", pResponde->FECAESolicitarResult->FeCabResp->Reproceso);
            fprintf(res, "\n\nCuerpo\n\n");
            fprintf(res, "    Concepto:   %d\n", pResponde->FECAESolicitarResult->FeDetResp->FECAEDetResponse->Concepto);
            fprintf(res, "    DocTipo:   %d\n", pResponde->FECAESolicitarResult->FeDetResp->FECAEDetResponse->DocTipo);
            fprintf(res, "    DocNro:   %lld\n", pResponde->FECAESolicitarResult->FeDetResp->FECAEDetResponse->DocNro);
            fprintf(res, "    CbteDesde:   %lld\n", pResponde->FECAESolicitarResult->FeDetResp->FECAEDetResponse->CbteDesde);
            fprintf(res, "    CbteHasta:   %lld\n", pResponde->FECAESolicitarResult->FeDetResp->FECAEDetResponse->CbteHasta);
            fprintf(res, "    CbteFch:   %s\n", pResponde->FECAESolicitarResult->FeDetResp->FECAEDetResponse->CbteFch);
            fprintf(res, "    Resultado:   %s\n", pResponde->FECAESolicitarResult->FeDetResp->FECAEDetResponse->Resultado);
            fprintf(res, "    CAE:   %s\n", pResponde->FECAESolicitarResult->FeDetResp->FECAEDetResponse->CAE);
            fprintf(res, "    CAEFchVto:   %s\n",    pResponde->FECAESolicitarResult->FeDetResp->FECAEDetResponse->CAEFchVto);

            /*****Guardamos CAE*****/
            guarda_cae(pResponde->FECAESolicitarResult->FeCabResp->CbteTipo,
                    pResponde->FECAESolicitarResult->FeCabResp->PtoVta,
                    pResponde->FECAESolicitarResult->FeDetResp->FECAEDetResponse->CbteDesde,
                    pResponde->FECAESolicitarResult->FeDetResp->FECAEDetResponse->CAE,
                    pResponde->FECAESolicitarResult->FeDetResp->FECAEDetResponse->CAEFchVto,
                    *(argv + 28)?*(argv + 28):"");

            /*****Manejador de señal SIGSEGV en ERRORES u OBSERVACIONES*****/
            if(!strcmp(pResponde->FECAESolicitarResult->FeCabResp->Resultado, "R")) {
                signal(SIGSEGV, handle_SIGSEGV_Err);
                if(setjmp(env_err) != 0) {
                    for(i=0; i<pResponde->FECAESolicitarResult->Errors->__sizeErr; i++) {
                        fprintf(res, "    Errores:       %d\n", pResponde->FECAESolicitarResult->Errors->Err->Code);
                        fprintf(e, "    Errores:       %d\n", pResponde->FECAESolicitarResult->Errors->Err->Code);
                        fprintf(res, "         %s\n", pResponde->FECAESolicitarResult->Errors->Err->Msg);
                        fprintf(e, "         %s\n", pResponde->FECAESolicitarResult->Errors->Err->Msg);
                    }
                }
                else {
                    for(i=0; i<pResponde->FECAESolicitarResult->FeDetResp->FECAEDetResponse->Observaciones->__sizeObs; i++) {
                        fprintf(res, "    Observaciones: %d\n",pResponde->FECAESolicitarResult->FeDetResp->FECAEDetResponse->Observaciones->Obs[i].Code);
                        fprintf(e, "    Observaciones: %d\n",pResponde->FECAESolicitarResult->FeDetResp->FECAEDetResponse->Observaciones->Obs[i].Code);
                        fprintf(res, "         %s\n",pResponde->FECAESolicitarResult->FeDetResp->FECAEDetResponse->Observaciones->Obs[i].Msg);
                        fprintf(e, "         %s\n",pResponde->FECAESolicitarResult->FeDetResp->FECAEDetResponse->Observaciones->Obs[i].Msg);
                    }
                }
            }
            /*****Manejador de señal SIGSEGV en EVENTOS*****/
            signal(SIGSEGV, handle_SIGSEGV_Evt);
            if(setjmp(env_evt) != 0) {
                for(i=0; i<pResponde->FECAESolicitarResult->Events->__sizeEvt; i++) {
                    fprintf(res, "    Evento:        %d\n", pResponde->FECAESolicitarResult->Events->Evt[i].Code);
                    fprintf(res, "         %s\n", pResponde->FECAESolicitarResult->Events->Evt[i].Msg);
                }
            }
            fclose(e);
            fclose(res);
        }
        else {
            printf("ERROR al abrir archivo o directorio\n");
            printf("Revisar existencia, propietario, grupo y permisos de:\n");
            printf("    /master/intercambio/WSERVICE/$emp/$user/Logs\n");
            exit(EXIT_FAILURE);
        }
    }
    /*****FIN Rama Verdadera IF*****/
    else {
        /*****INICIO Rama Falsa IF*****/
        printf("ERROR DE RED\n");
        soap_print_fault(soap, stderr);
        /*****FIN Rama Falsa IF*****/
    }
    soap_destroy(soap);
    soap_end(soap);
    soap_free(soap);

    printf("*****Salimos de wsfe_sin_trib*****\n");

    return 0;
}

void handle_SIGSEGV_Err(int signal)
{
    longjmp(env_err, 1);
    printf("KERNEL PANIC\n");
    exit(EXIT_FAILURE);
}

void handle_SIGSEGV_Evt(int signal)
{
    longjmp(env_evt, 1);
    printf("KERNEL PANIC\n");
    exit(EXIT_FAILURE);
}
