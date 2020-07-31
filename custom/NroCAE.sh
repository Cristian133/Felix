#!/bin/bash

# Consulta Datos de Comprobante autorizado.
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

#--------------------------------------------------------------------
# $1 TipoCbte   	-> ver método TiposCbtes.
# $2 PtoVta     	-> ver método PtosVenta.
# $3 CbteDesde  	-> nro de comprobante.
# $4 PWD        	-> carpeta de usuario.
#--------------------------------------------------------------------

cd /master/intercambio/WSERVICE/$emp
echo "F3LIX => FELIX VERSION 3.10 30-03-2019" > ./$4/Logs/NroCAE.log
./consulta.exe $cuitemp $2 $1 $3 $4 >> ./$4/Logs/NroCAE.log
chmod -v 777 ./$4/cae*.txt ./$4/error.txt >> ./$4/Logs/NroCAE.log
chmod -v 777 $(find ./$4/Logs/ -name "NroCAE*.txt" -type f | talil -1) >> ./$4/Logs/NroCAE.log
