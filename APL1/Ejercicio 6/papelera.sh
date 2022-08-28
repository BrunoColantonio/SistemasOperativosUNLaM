#!/bin/bash

# -------------------------- ENCABEZADO --------------------------

# Nombre del script: papelera.sh
# Número de APL: 1
# Número de ejercicio: 6
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
	echo "Este script emula el comportamiento del comando rm, pero utilizando el concepto de "papelera de reciclaje", teniendo la posibilidad de recuperar un objeto en el futuro."
	echo "Dicha papelera es un archivo comprimido ZIP y se encuentra alojada en el home del usuario que ejecuta el comando."
	echo "---------------- AYUDA - FORMATO DEL SCRIPT ----------------"
	echo "./papelera.sh [-h / -? / --help]: Muestra la ayuda"
	echo "./papelera.sh [--listar]: Lista los archivos que contienen la papelera de reciclaje, informando nombre de archivo y su ubicación original."
	echo "./papelera.sh [--recuperar <nombre_archivo>]: Recupera el archivo pasado por parámetro a su ubicación original"
	echo "./papelera.sh [--vaciar]: Vacía la papelera de reciclaje (elimina definitivamente los archivos)"
	echo "./papelera.sh [--eliminar <ubicacion_archivo>]: Elimina el archivo, enviándolo a la papelera de reciclaje"
	echo "./papelera.sh [--borrar <nombre_archivo>]: Borra un archivo de la papelera, haciendo que no se pueda recuperar"
}

validarPapelera() {
	papeleraAct="$(ls -a $dirPapelera | grep "papelera*.zip")"
	if [[ $papeleraAct && $papeleraAct != $nombrePapelera ]]
	then
		rm "$dirPapelera/$nombrePapelera"
	fi
}

listarArchivos() {
	zipinfo -1 "$dirPapelera/$nombrePapelera"
}


vaciarPapelera() {
	rm "$dirPapelera/$nombrePapelera"
	echo "Papelera vaciada de manera exitosa."
}

eliminarArchivo() {
	zip -m "$dirPapelera/$nombrePapelera" "$(realpath "$archivo")">/dev/null
	echo "$archivo enviado a la papelera de manera exitosa."
}

borrarArchivo() {
	zip -d "$dirPapelera/$nombrePapelera" "$archivo">/dev/null
	echo "$archivo fue eliminado de manera exitosa."
}

dirPapelera=$HOME
nombrePapelera="papelera v1.0.zip"

validarPapelera

case $1 in
	'--listar')
		listarArchivos
		exit
	;;
	'--recuperar')
		archivo=$2
		recuperarArchivo
		exit
	;;
	'--vaciar')
		vaciarPapelera
		exit
	;;
	'--eliminar')
		archivo=$2
		eliminarArchivo
		exit
	;;
	'--borrar')
		archivo=$2
		borrarArchivo
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