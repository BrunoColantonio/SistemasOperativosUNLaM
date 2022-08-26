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

calcularSalida() {
	for dir in $(ls $directorio)
	do
		mapfile -t fechas < <(cut -d" " -f1 "$directorio/$dir" | sort -u)
		mapfile -t usuarios < <(cut -d"-" -f4 "$directorio/$dir" | sort -u)
		for i in ${!fechas[*]}
		do
			grep "${fechas[$i]}[[:space:]][0-9][0-9]:[0-9][0-9]:[0-9][0-9]-${usuarios[$j]}" "$directorio/$dir" |  sort -k 4 -t"-" | awk '
			{
			    # $2 = 14:22:00-aerodriguez
			    # userLog[1] = 14:22:00
			    # userLog[2] = aerodriguez
			    split($2,userLog,"-")
			    split(userLog[1],hms,":")
			    usuarios[userLog[2]]++
			    contador++
			    # hms[1] = 14
			    # hms[2] = 22
			    # hms[3] = 00
			    absolutos[contador]=(hms[1]*3600)+(hms[2]*60)+(hms[3])
			} 
			END{
			    inicio=1
			    for(i in usuarios){
			        fin+=usuarios[i]
			        for(j=inicio;j<=fin;j+=2){
			          resu+=absolutos[j+1]-absolutos[j]
			          tiempo[i]+=absolutos[j+1]-absolutos[j]
			        }
			        inicio=j
			    }
			
			    # Muestra promedio de tiempo de llamadas (en segundos)
			    resu /= (contador/2)
			    print resu
			
			    # Muestra usuarios y cantidad de llamadas (por usuario)
			    mayor = 0
			    for (i in usuarios){
			        print i,(usuarios[i]/2),(tiempo[i])
			    }
			}'
		done
	done
}

case $1 in
	'--logs')
		if [ $2 ]
		then
			directorio=$2
		else
			directorio=.
		fi
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

