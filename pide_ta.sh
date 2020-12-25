#!/bin/bash

# Pide Ticket de Acceso.

# Solo para homologacion, comentar en Produccion.
#emp=cda

echo "genera_tra recibe argumento: $1"
./genera_tra.exe $1
echo "Se genera tra.xml"

echo "Se codifica tra.xml.cms a partir de tra.xml"
openssl smime -sign -signer ./certif/cert_$emp.crt -inkey ./certif/privada_$emp -out ./$1/tra.xml.cms -in ./$1/tra.xml -outform DER -nodetach

echo "Se codifica tra.xml.cms.base64 a partir de tra.xml.cms"
openssl base64 -in ./$1/tra.xml.cms -out ./$1/tra.xml.cms.base64

./wsaa.exe "./$1/tra.xml.cms.base64"

chmod -f 777 ta.xml
