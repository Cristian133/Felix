#!/bin/bash

# Pide CAE.
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
# $1 argumento enviado por el sistema: Lista de argumentos separados
#    por iel caracter '_' (guion bajo).
# $2 argumento enviado por el sistema: carpeta de usuario.
#
#--------------------------------------------------------------------

cd /master/intercambio/WSERVICE/$emp/
echo "F3LIX => FELIX VERSION 3.10 30-03-2019" > ./$2/Logs/facele.log
./pide_cae.sh $1 $2 >> ./$2/Logs/facele.log
chmod -v 777 ./$2/Logs/facele.log ./$2/cae*.txt ./$2/error.txt ./$2/Request*.txt >> ./$2/Logs/facele.log
chmod -v 777 $(find ./$2/Logs -name "*.xml"  -type f | tail -1) >> ./$2/Logs/facele.log
