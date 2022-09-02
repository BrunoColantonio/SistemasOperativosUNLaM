#!/bin/bash

# -------------------------- ENCABEZADO --------------------------

# Nombre del script: ejercicio3.sh
# Número de APL: 1
# Número de ejercicio: 3
# Número de entrega: Entrega

# ---------------- INTEGRANTES DEL GRUPO ----------------

#       Apellido, Nombre       	  | DNI
# 	Agostino, Matías          | 43861796
# 	Colantonio, Bruno Ignacio | 43863195
#    	Fernández, Rocío Belén    | 43875244
#    	Galo, Santiago Ezequiel   | 43473506
#  	Panigazzi, Agustín Fabián | 43744593

# -------------------- FIN DE ENCABEZADO --------------------

# ------------ FUNCIONES PRINCIPALES ------------

ayuda(){
	echo " "

	echo "------------ AYUDA - DESCRIPCION DEL SCRIPT -----------"
	
	echo " "	
	
	echo "El script ejecuta un sistema de integracion continua. Cada vez que se detecta un cambio en alguno de los archivos del directorio especificado, se ejecutan las siguientes acciones (las cuales deben ser especificadas, tambien, por el usuario entre comas):

- listar: muestra por pantalla los nombres de los archivos que sufrieron modificaciones.

- peso: muestra por pantalla el peso de los archivos modificados.
	
- compilar: concatena el contenido de todos los archivos dentro del directorio en cuestion y guarda el resultado en una carpeta 'bin' dentro del mismo.
	
- publicar: copia  el contenido del 'proceso de compilacion' en una carpeta especificada por el usuario - solo podra ejecutarse si se ha especificado anteriormente la accion de compilar.

	echo " "
	echo "------------ AYUDA - FORMATO DEL SRCIPT ------------"
	echo " "
	echo "./ejercicio3.sh [-h / -? / --help] => Muestra ayuda"
	echo "./ejercicio3.sh [-c path -a [listar,peso,compilar,publicar] -s path_final] => inicio"	  
	echo "----------- AYUDA - CIERRE DEFINITIVO DEL SCRIPT ------------"
	echo "1) Para cerrar definitivamente el script, ingrese el comnado ps"
	echo "2) Tome nota de los PID de los procesos con nombre 'ejercicio3.sh' y 'inotifywait'"
	echo "3) Ejecute el comando: kill -10 [PID_de_'ejercicio3.sh']"
	echo "4) Ejecute el comando: kill -10 [PID_de_'inotifywait']"


- NOTA: Es necesario, para asegurar el correcto funcionamiento del script, ejecutar los siguientes comandos:
				         ** sudo apt update **
				  ** sudo apt install inotify-tools **
}

function finalizar(){

        echo "finalizado..."
}

function demonio(){

	while out=$(inotifywait -e modify,create,delete,move -r -q --format "%w%f" "$dir")
	do

		if [[ ${map["listar"]} -eq 1 ]]; then
			echo "Archivo modificado"
			echo "$out"
			echo " "
		fi

		if [[ ${map["peso"]} -eq 1 ]]; then
			echo "Peso"
			echo $(du -sh $out)
			echo " "
		fi

		if [[ ${map["compilar"]} -eq 1 ]]; then
			echo "Compilando..."

			mkdir -p "$dir/bin" 
			find $dir -type f -exec cat {} + > "$dir/bin/merged"
			
			echo " "
		fi

		if [[ ${map["publicar"]} -eq 1 ]]; then
			echo "Publicando..."

			if [[ $dirFinIn == false ]]; then
				"No se ingreso el path del directorio de destino. Pruebe con ./ejercicio3.sh [-h, --help, -?]"
				exit
			fi
			
			mkdir -p "$dirFin"
			cp "$dir/bin/merged" "$dirFin"				
			
			echo "[Publicado]"
			echo " "
		fi
	done
}

# ---------- INGRESO DE PARAMETROS ----------

declare -A map
valido=false
comandoIncorrecto=false
dirFinIn=false

if [[ "$1" == "-h" || "$1" == "--help" || "$1" == "-?" ]]
then
	ayuda
	exit
fi

while getopts "c:a:s:" opcion
do
	case $opcion in
		c)
			dT=$(echo "$OPTARG" | tr -d "[:space:]")
			dir=$(realpath "$dT")
			valido=true
		;;
		a)	auxIFS=$IFS
			
			IFS=','

			acciones=($OPTARG)
			
			IFS=$AUXIFS

			for i in ${acciones[*]}
			do
				if [[ $i == "listar" || $i == "peso" || $i == "compilar" || 
			      	      $i == "publicar" ]]; then
					(( map[$i]++ ))
					valido=true	
				else
					comandoIncorrecto=true
				fi
			done

			if [[ $comandoIncorrecto == true ]]; then
				echo "Comando invalido ingresado. Pruebe con ./ejercicio3.sh [-h,--help, -?]"
				exit
			fi
			
			if [[ ${map["publicar"]} -ge 1 && ${map["compilar"]} -eq 0 ]]; then
				echo "Solo se puede publicar, si y solo si, antes se ha compilado. Pruebe con ./ejercicio3.sh [-h, --help, -?]"
				exit
			fi
			
			for i in ${map[*]}
			do
				if [[ $i -gt 1 ]]; then
					echo "Los parametros se repiten. Pruebe con ./ejercicio3.sh [-h, --help, -?]"
					exit
				fi
			done
		;;
		s)	
			dFT=$(echo "$OPTARG" | tr -d "[:space:]")
			dirFin=$(realpath -m "$dFT")
			valido=true
			dirFinIn=true
		;;
		*)
			echo "Error: Parametro invalido. Pruebe ingresando ./ejercicio3.sh [-h, --help, -?]"
			exit
		;;
	esac
done

# ------------ EJECUCION EN MODO "DEMONIO" ------------

if "$valido"; then
	if [[ -d "$dir" && -r "$dir" ]]; then
		demonio &
		trap finalizar SIGUSR1
	else
		echo "$dir no es un directorio o no posee los permiso de lectura"
		exit
	fi
else
	echo "No se ingresaron parametros validos. Pruebe ingresando ./ejercicio3.sh [-h, --help, -?]"
	exit
fi
