#!/bin/bash

# -------------------------- ENCABEZADO --------------------------

# Nombre del script: ejercicio3.sh
# Número de APL: 1
# Número de ejercicio: 4
# Número de entrega: Entrega 

# ---------------- INTEGRANTES DEL GRUPO ----------------

#          Apellido, Nombre         | DNI
#           Agostino, Matías          | 43861796
# Colantonio, Bruno Ignacio | 43863195
#    Fernández, Rocío Belén    | 43875244
#    Galo, Santiago Ezequiel    | 43473506
#  Panigazzi, Agustín Fabián  | 43744593

# -------------------- FIN DE ENCABEZADO --------------------

# FUNCIÓN DE AYUDA

ayuda() {
	echo "-------------- AYUDA - DESCRIPCIÓN DEL SCRIPT --------------"
	echo "Dados archivos de log donde se registraron todas las llamadas realizadas en una semana por un call center, el script obtiene y muestra por pantalla los siguientes datos:"
	echo "1. Promedio de tiempo de las llamadas realizadas por día"
	echo "2. Promedio de tiempo y cantidad por usuario por día"
	echo "3. Los 3 usuarios con más llamadas en la semana"
	echo "4. Cuántas llamadas no superan la media de tiempo por día"
	echo "5. El usuario que tiene más cantidad de llamadas por debajo de la media en la semana"
	echo "---------------- AYUDA - FORMATO DEL SCRIPT ----------------"
	echo "./ejercicio4.sh [-h / -? / --help]: Muestra la ayuda"
	echo "./ejercicio4.sh [--ruta <ubicacion_directorio> --ext <extension1,extension2,extensionN>]: Realiza el analisis en el directorio cuya ubicación es ubicacion_directorio para las extensiones especificadas luego de --ext"
	return 0
}

validarDirectorio() {
	if [ ! -d "$directorio" ]
	then
		echo "Error: Directorio inexistente."
		exit
	elif ! test -r "$directorio"
	then
		echo "Error:  No se poseen permisos de lectura sobre el directorio."
		exit
	fi
}

if [[ "$1" == "--ruta" && "$3" == "--ext" ]]
then
    directorio="$2"
    IFS=',' read -ra extensiones <<< "$4"
    validarDirectorio
else
    echo "Error de sintaxis en la entrada."
    ayuda
fi


if [[ "$1" == "--help" ||  "$1" == "-h" || "$1" == "--?" ]]
then
	ayuda
	exit
fi

# ------------------------ FIN DE ARCHIVO ------------------------
