/*
 * Consulta datos de comprobante ya autorizado a AFIP.
 *
 */

#include "ServiceSoap.nsmap"
#include "soapH.h"
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>
#include <errno.h>
#include <time.h>

#define TRUE 1
#define FALSE 0

/*****Salva calling enviroment*****/
static jmp_buf env_err;
static jmp_buf env_cbe;

/*****Prototipos de funciones*****/
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

/*****Prototipos de Manejadores de señales*****/
void handle_SIGSEGV_Err(int signal);
void handle_SIGSEGV_Cbe(int signal);

/*****main*****/
int main(int argc, char **argv, char **envp)
{

    printf("*****Entramos a consulta*****\n");

    int cbte_c;
    char pathta[128];

    /*****Deben ser 4 o 5 argumentos*****/
    if(argc < 5) {
        printf("Cantidad incorrecta de argumentos\n");
        exit(EXIT_FAILURE);
    }

    /*****Si existe TA, Analiza validez de fecha*****/
    FILE *ta = fopen("ta.xml","r");
    if((ta == NULL) || (expira_ta() < 60)) {
        printf("El TA ha expirado o no se encuentra ta.xml\n");
        sprintf(pathta, "./pide_ta.sh %s", *(argv + 5)?*(argv + 5):"");
        system(pathta);
    }

    /*****Aloca memoria e Inicializa estructuras SOAP*****/
    struct soap *soap = soap_new();
    const char *soap_endpoint = NULL;
    const char *soap_action = NULL;

    /*****Preguntamos si SOAP no tuvo problemas****/
    if(!soap) {
        printf("ERROR en librerías gSOAP\n");
        printf("Reinstalar paquetes:\n");
        printf("gsoap-devel libgsoap libgsoap-devel\n");
        exit (EXIT_FAILURE);
    }

    /*****Declaraciones de estructuras definidas por gsoap*****/
    /*****Estructuras Principales*****/
    struct _ns3__FECompConsultar CompReq;
    struct _ns3__FECompConsultar *pCompReq = &CompReq;
    struct _ns3__FECompConsultarResponse CompRes;
    struct _ns3__FECompConsultarResponse *pCompRes = &CompRes;

    /*****Estructuras Secundarias*****/
    struct ns3__FEAuthRequest Autoriza;
    struct ns3__FECompConsultaReq Req;

    /*****Apuntamos Estructuras internas*****/
    pCompReq->Auth = &Autoriza;
    pCompReq->FeCompConsReq = &Req;

    /*****Seteamos estructuras*****/
    printf("Seteamos estructuras\n");
    /*****Autoriza*****/
    pCompReq->Auth->Token  = extrae_token();
    pCompReq->Auth->Sign   = extrae_sign();
    pCompReq->Auth->Cuit   = atoll(procesa_cuit(*(argv + 1)));
    /*****Cuerpo*****/
    pCompReq->FeCompConsReq->PtoVta = atoi(*(argv + 2));
    pCompReq->FeCompConsReq->CbteTipo = atoi(*(argv + 3));
    pCompReq->FeCompConsReq->CbteNro = atoll(*(argv + 4));

    printf("Recibimos CUIT: %lld\tPtoVta: %d\tCbteTipo: %d\tCbteNro: %lld\n",
            pCompReq->Auth->Cuit,
            pCompReq->FeCompConsReq->PtoVta,
            pCompReq->FeCompConsReq->CbteTipo,
            pCompReq->FeCompConsReq->CbteNro);

    /*****Funcion pide Datos Comprobantes*****/
    printf("Solicita Datos Cbte autorizado a AFIP\n");
    if(soap_call___ns3__FECompConsultar(
                soap,
                soap_endpoint,
                soap_action,
                pCompReq,
                pCompRes) == SOAP_OK) {

        /*****INICIO Rama Verdadera IF*****/
        printf("Llega respuesta de afip\n");
        /*****Escribe Resultados*****/
        int i;
        char resultado[128];
        char error[128];
        char strTime[128];
        time_t fechActual = time(0);
        struct tm *horaActual = localtime(&fechActual);

        strftime(strTime, 128, "%Y%m%d%H%M%S", horaActual);
        sprintf(resultado, "./%s/Logs/NroCAE_%02d_%04d_%08ld.txt",
                *(argv + 5)?*(argv + 5):"",
                pCompReq->FeCompConsReq->CbteTipo,
                pCompReq->FeCompConsReq->PtoVta,
                pCompReq->FeCompConsReq->CbteNro);
        FILE *res = fopen(resultado, "w");
        sprintf(error, "./%s/error.txt", *(argv + 5)?*(argv + 5):"");
        FILE *e = fopen(error, "w");

        if((res != NULL) && (e != NULL)) {
            signal(SIGSEGV, handle_SIGSEGV_Cbe);
            if(setjmp(env_cbe) == 0) {
                fprintf(res, "        Concepto: %d\n", pCompRes->FECompConsultarResult->ResultGet->Concepto);
                printf("Legan Datos de Cbte pedido\n");
                fprintf(res, "        DocTipo : %d\n", pCompRes->FECompConsultarResult->ResultGet->DocTipo);
                fprintf(res, "        DocNro  : %lld\n", pCompRes->FECompConsultarResult->ResultGet->DocNro);
                fprintf(res, "        CbteNro : %ld\n", pCompRes->FECompConsultarResult->ResultGet->CbteDesde);
                fprintf(res, "        CbteFch : %s\n", pCompRes->FECompConsultarResult->ResultGet->CbteFch);
                fprintf(res, "        ImpTotal: %.2f\n", pCompRes->FECompConsultarResult->ResultGet->ImpTotal);
                fprintf(res, "        ImpTotConc: %.2f\n", pCompRes->FECompConsultarResult->ResultGet->ImpTotConc);
                fprintf(res, "        ImpNeto : %.2f\n", pCompRes->FECompConsultarResult->ResultGet->ImpNeto);
                fprintf(res, "        ImpOpEx : %.2f\n", pCompRes->FECompConsultarResult->ResultGet->ImpOpEx);
                fprintf(res, "        ImpTrib : %.2f\n", pCompRes->FECompConsultarResult->ResultGet->ImpTrib);
                fprintf(res, "        ImpIva  : %.2f\n", pCompRes->FECompConsultarResult->ResultGet->ImpIVA);
                fprintf(res, "        FchServHasta : %s\n", pCompRes->FECompConsultarResult->ResultGet->FchServHasta);
                fprintf(res, "        FchVtoPago   : %s\n", pCompRes->FECompConsultarResult->ResultGet->FchVtoPago);
                fprintf(res, "        MonId   : %s\n", pCompRes->FECompConsultarResult->ResultGet->MonId);
                fprintf(res, "        MonCotiz: %.2f\n", pCompRes->FECompConsultarResult->ResultGet->MonCotiz);
                fprintf(res, "        Alicuotas de IVA:\n");

                cbte_c = FALSE;
                if(atoi(*(argv + 3)) == 11 || atoi(*(argv + 3)) == 12 || atoi(*(argv + 3)) == 13)
                    cbte_c = TRUE;

                if(cbte_c){
                    for(i = 0; i <  pCompRes->FECompConsultarResult->ResultGet->Iva->__sizeAlicIva; i++) {
                        fprintf(res, "            Id: %d\n", pCompRes->FECompConsultarResult->ResultGet->Iva->AlicIva[i].Id);
                        fprintf(res, "            BaseImp: %.2f\n", pCompRes->FECompConsultarResult->ResultGet->Iva->AlicIva[i].BaseImp);
                        fprintf(res, "            Importe: %.2f\n", pCompRes->FECompConsultarResult->ResultGet->Iva->AlicIva[i].Importe);
                    }
                    if(pCompRes->FECompConsultarResult->ResultGet->ImpTrib > 0) {
                        fprintf(res, "        Tributos:\n");
                        for(i = 0; i <  pCompRes->FECompConsultarResult->ResultGet->Tributos->__sizeTributo; i++) {
                            fprintf(res, "            Id: %d\n", pCompRes->FECompConsultarResult->ResultGet->Tributos->Tributo[i].Id);
                            fprintf(res, "            Desc: %s\n", pCompRes->FECompConsultarResult->ResultGet->Tributos->Tributo[i].Desc);
                            fprintf(res, "            BaseImp: %.2f\n", pCompRes->FECompConsultarResult->ResultGet->Tributos->Tributo[i].BaseImp);
                            fprintf(res, "            Alicuota: %.2f\n", pCompRes->FECompConsultarResult->ResultGet->Tributos->Tributo[i].Alic);
                            fprintf(res, "            Importe: %.2f\n", pCompRes->FECompConsultarResult->ResultGet->Tributos->Tributo[i].Importe);
                        }
                    }
                }
                fprintf(res, "        Resultado: %s\n", pCompRes->FECompConsultarResult->ResultGet->Resultado);
                fprintf(res, "        CodAutorizacion: %s\n", pCompRes->FECompConsultarResult->ResultGet->CodAutorizacion);
                fprintf(res, "        EmisionTipo: %s\n", pCompRes->FECompConsultarResult->ResultGet->EmisionTipo);
                fprintf(res, "        FchVto: %s\n", pCompRes->FECompConsultarResult->ResultGet->FchVto);
                fprintf(res, "        FchProceso: %s\n", pCompRes->FECompConsultarResult->ResultGet->FchProceso);
                fprintf(res, "        PtoVta: %d\n", pCompRes->FECompConsultarResult->ResultGet->PtoVta);
                fprintf(res, "        CbteTipo: %d\n", pCompRes->FECompConsultarResult->ResultGet->CbteTipo);
                /*****guarda*****/
                consul_cae(pCompRes->FECompConsultarResult->ResultGet->CbteTipo,
                        pCompRes->FECompConsultarResult->ResultGet->PtoVta,
                        pCompRes->FECompConsultarResult->ResultGet->CbteDesde,
                        pCompRes->FECompConsultarResult->ResultGet->CodAutorizacion,
                        pCompRes->FECompConsultarResult->ResultGet->FchVto,
                        *(argv + 5) ? *(argv + 5) : "");
            }
            else {
                signal(SIGSEGV, handle_SIGSEGV_Err);
                if(setjmp(env_err) == 0) {
                    for(i = 0; i < pCompRes->FECompConsultarResult->Errors->__sizeErr; i++) {
                        printf("Llega ERROR de Cbte pedido\n");
                        fprintf(res, "Error Codigo %d\n",pCompRes->FECompConsultarResult->Errors->Err[i].Code);
                        fprintf(e, "Error Codigo %d\n",pCompRes->FECompConsultarResult->Errors->Err[i].Code);
                        fprintf(res, "Descripcion: %s\n",pCompRes->FECompConsultarResult->Errors->Err[i].Msg);
                        fprintf(e, "Descripcion: %s\n",pCompRes->FECompConsultarResult->Errors->Err[i].Msg);
                    }
                }
                else {
                    for(i = 0; i < pCompRes->FECompConsultarResult->ResultGet->Observaciones->__sizeObs; i++) {
                        printf("Llegan OBSERVACIONES sobre el Cbte pedido\n");
                        fprintf(res, "       Observaciones: %d\n",pCompRes->FECompConsultarResult->ResultGet->Observaciones->Obs[i].Code);
                        fprintf(e, "       Observaciones: %d\n",pCompRes->FECompConsultarResult->ResultGet->Observaciones->Obs[i].Code);
                        fprintf(res, "                      %s\n",pCompRes->FECompConsultarResult->ResultGet->Observaciones->Obs[i].Msg);
                        fprintf(e, "                      %s\n",pCompRes->FECompConsultarResult->ResultGet->Observaciones->Obs[i].Msg);
                    }
                }
                fclose(e);
                fclose(res);
            }
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
        /*****INICIO Rama Falsa IF****/
        printf("ERROR EN RED\n");
        soap_print_fault(soap, stderr);
        /*****FIN Rama Falsa IF****/
    }

    soap_destroy(soap);
    soap_end(soap);
    soap_free(soap);

    printf("*****Salimos de comp_consulta*****\n");
    return 0;
}

void handle_SIGSEGV_Cbe(int signal)
{
    longjmp(env_cbe, 1);
    exit(EXIT_FAILURE);
}

void handle_SIGSEGV_Err(int signal)
{
    longjmp(env_err, 1);
    exit(EXIT_FAILURE);
}
