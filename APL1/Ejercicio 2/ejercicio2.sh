#!/bin/bash

# -------------------------- ENCABEZADO --------------------------

# Nombre del script: ejercicio2.sh
# Número de APL: 1
# Número de ejercicio: 2
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
	echo "./ejercicio2.sh [-h / -? / --help]: Muestra la ayuda"
	echo "./ejercicio2.sh [--logs]: Realiza los cálculos anteriormente realizados sobre el directorio actual"
	echo "./ejercicio2.sh [--logs <direccion_directorio>]: Realiza los cálculos anteriormente mencionados en el directorio cuya ubicación es ubicacion_directorio"
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

calcularSalida() {
	for dir in $(ls "$directorio")
	do
		declare -A cantLlamadasXUsuarioSemana
		declare -A cantLlamadasBajoMediaXUsuarioSemana

		mapfile -t usuarios < <(cut -d"-" -f4 "$directorio/$dir" | sort -u)
		mapfile -t fechas < <(cut -d" " -f1 "$directorio/$dir" | sort -u)
		
		for i in ${!fechas[*]}
		do
			cantLlamadasDia=0
			promedioTiempoDia=0
			cantLlamadasBajoMediaDia=0
			for j in ${!usuarios[*]}
			do
				declare -A cantLlamadasXUsuarioDia
				declare -A promedioLlamadasXUsuarioDia

				cantLlamadasUsuario=0
				promedioTiempoUsuario=0

				mapfile -t llamadas < <(grep "${fechas[$i]}[[:space:]][0-9][0-9]:[0-9][0-9]:[0-9][0-9]-${usuarios[$j]}" "$directorio/$dir")
				for (( k=0; k<${#llamadas[@]}; k+=2 ))
				do
					hora1=$(echo ${llamadas[$k]} | cut -d'-' -f1-3)
					hora2=$(echo ${llamadas[(($k+1))]} | cut -d'-' -f1-3)
					duracion=$(( $(date -d "$hora2" "+%s") - $(date -d "$hora1" "+%s") ))

					((promedioTiempoDia += $duracion ))
					((promedioLlamadasXUsuarioDia[ ${usuarios[$j]} ] += $duracion ))

					((cantLlamadasDia++))
					((cantLlamadasXUsuarioDia[ ${usuarios[$j]} ]++))
					((cantLlamadasXUsuarioSemana[ ${usuarios[$j]} ]++))
				done

				((promedioLlamadasXUsuarioDia[ ${usuarios[$j]} ] /= cantLlamadasXUsuarioDia[ ${usuarios[$j]} ]))

			done
			
			((promedioTiempoDia /= $cantLlamadasDia))

			# Búsqueda de llamadas por debajo de la media en el día

			for j in ${!usuarios[*]}
			do
				mapfile -t llamadas < <(grep "${fechas[$i]}[[:space:]][0-9][0-9]:[0-9][0-9]:[0-9][0-9]-${usuarios[$j]}" "$directorio/$dir")
				for (( k=0; k<${#llamadas[@]}; k+=2 ))
				do
					hora1=$(echo ${llamadas[$k]} | cut -d'-' -f1-3)
					hora2=$(echo ${llamadas[(($k+1))]} | cut -d'-' -f1-3)
					duracion=$(( $(date -d "$hora2" "+%s") - $(date -d "$hora1" "+%s") ))

					if [ $duracion -lt $promedioTiempoDia ]
					then
						(( cantLlamadasBajoMediaDia++ ))
						((cantLlamadasBajoMediaXUsuarioSemana[ ${usuarios[$j]} ]++))
					fi
				done
			done

			echo "---------------------------- Fecha: ${fechas[$i]} ----------------------------"

			echo "1. Promedio de tiempo de las llamadas realizadas en el día: "$promedioTiempoDia

			echo "2. Usuario | Promedio de tiempo | Cantidad de llamadas"
			for i in ${!promedioLlamadasXUsuarioDia[*]}
			do
				echo $i ${promedioLlamadasXUsuarioDia[ $i ]} ${cantLlamadasXUsuarioDia[ $i ]}
			done

			echo "4. Cantidad de llamadas que no superan la media de tiempo por día: $cantLlamadasBajoMediaDia"

			unset promedioLlamadasXUsuarioDia
			unset cantLlamadasXUsuarioDia
		done

		echo "--------------------------- Resumen de la Semana ---------------------------"

		echo "3. Los 3 usuarios con más llamadas en la semana"
		echo "Usuario | Cantidad de llamadas"
		for i in ${!cantLlamadasXUsuarioSemana[*]}
		do
			echo $i ${cantLlamadasXUsuarioSemana[ $i ]}
		done | sort -k 2 -t" " -nr | head -3
		
		echo "5. Usuario con más cantidad de llamadas por debajo de la media en la semana"
		echo "Usuario | Cantidad de llamadas por debajo de la media en la semana"
		for i in ${!cantLlamadasBajoMediaXUsuarioSemana[*]}
		do
			echo $i ${cantLlamadasBajoMediaXUsuarioSemana[ $i ]}
		done | sort -k 2 -t" " -nr | head -1


		unset cantLlamadasXUsuarioSemana
		unset cantLlamadasBajoMediaXUsuarioSemana
	done
}

case $1 in
	'--logs')
		if [ "$2" ]
		then
			directorio=$(realpath "$2")
		else
			directorio=.
		fi
		validarDirectorio
		calcularSalida
		exit
	;;
	'-h' | '--help' | '-?')
		ayuda
		exit
	;;
	*)
		echo "Error: Comando inexistente."
		ayuda
		exit
	;;
esac

# ------------------------ FIN DE ARCHIVO ------------------------