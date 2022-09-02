#!/bin/bash

# FUNCIÓN DE AYUDA

ayuda() {
	echo "-------------- AYUDA - DESCRIPCIÓN DEL SCRIPT --------------"
	echo "Dados dos archivos, uno de notas y otro de materias, se pide obtener la cantidad de alumnos que han"
	echo "promocionado (ambos parciales/recuperatorio con nota mayor o igual a 7), aptos para rendir final"
	echo "(sin final y con ambos parciales/recuperatorio con nota mayor o igual a 4),"
	echo "que recursaran (nota menor a 4 en final o en parciales y/o recuperatorios),"
	echo "y que abandonaron la materia (sin nota en algun parcial y sin recuperatorio)"
	echo "---------------- AYUDA - FORMATO DEL SCRIPT ----------------"
	echo "./ejercicio5.sh [-h / -? / --help]: Muestra la ayuda"
	echo "./ejercicio5.sh [--notas <nombre_arch_notas> --materias <nombre_arch_materias>]: Realiza los cálculos anteriormente realizados sobre los archivos enviados"
	return 0
}

validarArchivos() {
	if [[ (! -f "$notas") || (! -f "$materias") ]]
	then
		echo "Error: Archivo inexistente."
		exit
	elif ! test -r "$notas"
	then
		echo "Error:  No se poseen permisos de lectura sobre el archivo."
		exit
	elif ! test -r "$materias"
	then
		echo "Error:  No se poseen permisos de lectura sobre el archivo."
		exit
	
	fi
}

calcularSalida() {

#leer archivo materias y obtener ID


	while IFS="|" read -r idMat desc depto
	do

		arr_id[$idMat]=$idMat
		arr_desc[$idMat]=$desc
		arr_depto[$idMat]=$depto		
	done < "$materias"
	
	while IFS="|" read -r dni id pp sp rec final
	do

		if [[ "${arr_id[$id]}" -eq "$id" ]]
			then
	
				#procesamos los promocionados
	
					if [[ ("$pp" -ge 7 && "$sp" -ge 7) || ("$pp" -ge 7 && "$rec" -ge 7) || ("$sp" -ge 7 && "$rec" -ge 7) ]]
						then 
							(( cantProm[$id]++ ))
	
				#procesamos los que van a final
					elif [[ ( ("$pp" -ge 4 && "$sp" -ge 4 ) || ("$pp" -ge 4 && "$rec" -ge 4 ) || ("$sp" -ge 4 && "$rec" -ge 4) ) && "$final" == "" ]]
						then 
							(( cantFinal[$id]++ ))
	
				#procesamos los que abandonan
						elif [[  ("$pp" == "" && "$sp" == "") ||  ("$pp" == "" && "$rec" == "")  ||  ("$sp" == "" && "$rec" == "") ]]
							then
								(( cantAband[$id]++ ))
	
				#procesamos recursantes
						elif [[  "$final" == ""  ]] 
							then
								(( cantRecu[$id]++ ))
					fi
		fi

	done < "$notas"

	
	
	echo "Promocionan: " "${cantProm[*]}"
	echo "A Final: " "${cantFinal[*]}"
	echo "Recursan: " "${cantRecu[*]}"
	echo "Abandonan: " "${cantAband[*]}"

	echo "${arr_id[*]}"

	JsonData=' [{"departamentos": "["},
	{"Publication":"Apress"}] '

	echo "${JsonData}" | jq '.'

} 

if [[  "$1" == "--notas" && "$3" == "--materias"  ]]
	then
		notas="$2"
		materias="$4"
		validarArchivos
		calcularSalida
elif [[ "$1" == "--help" || "$1" == "-h" || "$1" == "--?"  ]]
	then 
		ayuda
else
	echo "Error de sintaxis en la entrada."
	ayuda
fi
