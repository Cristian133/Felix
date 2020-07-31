#!/bin/bash

# Pide Ticket de Acceso.
#
# Copyright (C) 2019  Cristian Andrione.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

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
