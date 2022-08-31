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
	echo "Este es un script que cuenta la cantidad de lineas de codigo y de comentarios que poseen los archivos en la ruta pasada"
	echo "Solo analiza los archivos con las extensiones pasadas por parametro, y lo hace recursivamente para cada subdirectorio"
	echo "---------------- AYUDA - FORMATO DEL SCRIPT ----------------"
	echo "./ejercicio4.sh [-h / -? / --help]: Muestra la ayuda"
	echo "./ejercicio4.sh [--ruta <ubicacion_directorio> --ext <extension1,extension2,extensionN>]: Realiza el analisis en el directorio cuya ubicación es ubicacion_directorio para las extensiones especificadas luego de --ext"
	return 0
}

calcularSalida(){

	cantArchivos=0
	comentariosTotales=0
	codigosTotales=0
	lineasTotales=0
	
	for i in ${!extensiones[*]}
	do
		
		rutaPadre=$(find "$directorio" -name "*."${extensiones[i]}"") 

		#for arch in "${directorio}"**/*."${extensiones[i]}" 
		
		for arch in $rutaPadre 
		do
			if [ -f "$arch" ]
			then
			(( cantArchivos++ ))
			comentarios=0
			comentariosMultilinea=0
			codigos=0	
				
			#(( lineasTotales+=$(cat "$arch" | wc -l) ))
			
			
			echo El archivo analizado es: $arch
			
			#Se analizan los comentarios del tipo /* */ (incluyendo multilinea)
			#Lo unico que no se tiene en cuenta son los del tipo /* here is your comment */ code :( (no saliò)
			
			(( comentariosMultilinea+=$(awk '
			BEGIN{ 
			comentarios=0 
			}
			/\/\*/,/\*\//{
			++comentarios 
			}
			END{
			print comentarios
			}' $arch) ))
			
			
			#A continuaciòn se analizan los comentarios de una sola linea, //
			#Tambièn incluye el analisis de code //here is your comment :)
		
			#respuestas es una variable que representa la salida del awk
			#luego, a travès del comando read se pasa a un array (validas)
			#(optimizar!!!)
			
			respuestas=$(cat $arch | awk '
			BEGIN{
			FS="//"
			comentarios=0
			codigos=0
			}
			{
			
			if(NF == 1){
				gsub("[[:blank:]]", "", $1)
				
				if(length($1) > 0)	
					codigos++
			}
			if(NF > 1){
				gsub("[[:blank:]]", "", $1)
				
				if(length($1) > 0){
					codigos++
					comentarios++
				} else {
					comentarios++
				}
			}
			}
			END{
				print codigos,comentarios
			}
			' )
			
			read -ra validas <<< "$respuestas"
			
			#A codigos se le resta los comentarios multilinea porque en el segundo analisis los contarìamos como còdigo
			
			((codigos+=validas[0]))
			((comentarios+=validas[1]))
			((comentarios+=comentariosMultilinea))
			((codigos-=comentariosMultilinea))
			
			(( lineasTotales+=comentarios ))
			(( lineasTotales+=codigos ))
			echo Cantidad de lineas de codigo: $codigos 
			echo Cantidad de comentarios: $comentarios
			
			echo
			(( comentariosTotales+=comentarios ))
			(( codigosTotales+=codigos )) 
			fi
		done		
	done
	
	if [ $cantArchivos -gt 0 ]
	then
		porcentajeCodigo=`bc -l <<< "scale=2; ($codigosTotales/$lineasTotales)*100"`
		porcentajeComentarios=`bc -l <<< "scale=2; ($comentariosTotales/$lineasTotales)*100"`
		echo "-------INFORMACION SOLICITADA-------"
		echo "Archivos totales analizados: " $cantArchivos 
		echo "Lineas totales analizadas: " $lineasTotales
		echo "Lineas de codigo totales: " $codigosTotales "($porcentajeCodigo %)"
		echo "Comentarios totales: " $comentariosTotales "($porcentajeComentarios %)"
	else
		echo "No hay archivos para analizar en la ruta pasada con esas extensiones."
	fi
	
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
    calcularSalida
elif [[ "$1" == "--help" ||  "$1" == "-h" || "$1" == "--?" ]]
then
	ayuda
else
    echo "Error de sintaxis en la entrada."
    ayuda
fi

# ------------------------ FIN DE ARCHIVO ------------------------
