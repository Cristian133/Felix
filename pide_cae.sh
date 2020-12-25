#!/bin/bash

# Pide CAE.

# Solo para Homologacion, comentar en Produccion.
#cuitemp=

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
