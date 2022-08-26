#!/bin/bash

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