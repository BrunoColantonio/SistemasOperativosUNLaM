#!/bin/bash

# -------------------------- ENCABEZADO --------------------------

# Nombre del script: ejercicio3.sh
# Número de APL: 1
# Número de ejercicio: 3
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
	echo "./ejercicio3.sh [-h / -? / --help]: Muestra la ayuda"
	echo "./ejercicio3.sh [--logs]: Realiza los cálculos anteriormente realizados sobre el directorio actual"
	echo "./ejercicio3.sh [--logs <direccion_directorio>]: Realiza los cálculos anteriormente mencionados en el directorio cuya ubicación es ubicacion_directorio"
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

listarArchCambios() {
#	mapfile -t usuarios < <(cut -d"-" -f4 "$directorio/$dir" | sort -u)
	
	#tiempoActual=$(stat "$directorio"  | grep Inodes | awk '{print $5}')
	#tiempoActual=$(stat "$directorio" | grep Modify | awk '{print$3}')
	#if [ "$tiempoActual" != "$ultimoTiempo" ]
	#then
	#find "$directorio" -newermt '1 second ago'
	#fueModificado="true"
	#fi
	
	inotifywait -e modify,create,delete_self -r "$directorio"
}

pesoArchCambios() {
	echo "Listando pesos"
}

compilarArchivos() {
	echo "Compilando archivos"
}

publicarArchCompilado() {
	echo "Publicando"
}

if [[ "$1" == "--help" ||  "$1" == "-h" || "$1" == "--?" ]]
then
	ayuda
	exit
fi


while getopts "c:a:s:" opcion
do
	case $opcion in
		c)
			directorio=$(realpath "$OPTARG")
		;;
		a)
			oldIFS=$IFS
			IFS=','
			opciones=($OPTARG)
			IFS=$oldIFS
		;;
		s)
			directorioP=$(realpath "$OPTARG")
		;;
		*)
			echo "Error: Comando inexistente."
			ayuda
			exit
		;;
	esac
done

#fueModificado="false"
#ultimoTiempo=$(stat "$directorio"  | grep Inodes | awk '{print $5}')
#ultimoTiempo=$(stat "$directorio" | grep Modify | awk '{print$3}')
while true
do
	for i in ${!opciones[*]}
	do
		case ${opciones[ $i ]} in
			'listar')
				listarArchCambios
			;;
			'peso')
				pesoArchCambios
			;;
			'compilar')
				compilarArchivos
			;;
			'publicar')
				publicarArchCompilado
			;;
		esac
	done
	
	#if [ "$fueModificado" == "true" ]
	#then
	#ultimoTiempo=$(stat "$directorio"  | grep Inodes | awk '{print $5}')
	#ultimoTiempo=$(stat "$directorio" | grep Modify | awk '{print$3}')
	#fueModificado="true"
	#fi
done


# ------------------------ FIN DE ARCHIVO ------------------------
