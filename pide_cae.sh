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

# Solo para Homologacion, comentar en Produccion.
#cuitemp=20214162874

FE_Usuario=$2

var=0

if [ $# -lt 1 ]
then
    echo "Falta argumento que envia sistema"
else
    echo $1 > ./$FE_Usuario/str_arg #args separados por _
    sed 's/_/ /g' ./$FE_Usuario/str_arg > ./$FE_Usuario/str_args #args separados por espacios
    echo "Se procesa argumentos con exito, cambiamos \"_\" por \" \""

    if [ -f ./$FE_Usuario/str_args_ctrl ]; then
        rm ./$FE_Usuario/str_args_ctrl
    fi
    for i in $(cat ./$FE_Usuario/str_args); do
            if [ $((var++)) -lt 26 ]; then #lee los primeros 27 argumentos, descarta los demas
                echo $i  >> ./$FE_Usuario/str_args_ctrl
            fi
    done
    args=$(cat ./$FE_Usuario/str_args_ctrl)
    echo $args
    set $args
    ./wsfe.exe $cuitemp $args $FE_Usuario
fi
