# FUNCIÓN DE AYUDA
<#
.SYNOPSIS
	Dados dos archivos, uno de notas y otro de materias, se pide obtener la cantidad de alumnos que han 
	promocionado, aprobado o desaprobado

.DESCRIPTION
	Dados dos archivos, uno de notas y otro de materias, se pide obtener la cantidad de alumnos que han
	promocionado (ambos parciales/recuperatorio con nota mayor o igual a 7), aptos para rendir final
	(sin final y con ambos parciales/recuperatorio con nota mayor o igual a 4),
	que recursaran (nota menor a 4 en final o en parciales y/o recuperatorios),
	y que abandonaron la materia (sin nota en algun parcial y sin recuperatorio).
	

.PARAMETER notas
	Este parametro corresponde al archivo "notas" y es de caracter obligatorio.
	

.PARAMETER materias
	Este parametro corresponde al archivo "materias" y es de caracter obligatorio.

.EXAMPLE
	 "./ejercicio5.sh [--notas <nombre_arch_notas> --materias <nombre_arch_materias>]: 
	Realiza los cálculos anteriormente realizados sobre los archivos enviados"
#>

Param (
	[Parameter(Position = 1, Mandatory = $true)]
	[String] $notas,
	[Parameter(Position = 2, Mandatory = $true)]
	[String] $materias
)

function existeDirect([string] $directorio){
   if(!(Test-Path $directorio)){
    Write-Host "No existe el directorio: $directorio"
    return $false
   }
   else{
    return $true
   }
}


 if(!(existeDirect($notas)) -or !(existeDirect($materias))){
    return $false
 }

 $a_materias = Get-Content "$materias"

 $idMateria = @()
 $descripcion = @()
 $departamento = @()

 foreach($linea in $a_materias) {
    $arr = $linea.Split('|')

    if(!($arr[0] -eq "IdMateria")){
        $idMateria+=$arr[0]
        $descripcion+=$arr[1]
        $departamento+=$arr[2]
    }
 }


 $a_notas = Get-Content "$notas"

 $promocionados = @{}
 $final = @{}
 $abandono = @{}
 $recursa = @{}

 foreach ($linea in $a_notas) {
    $arr = $linea.Split('|') 
    
    #procesamos promocionados
    if( ($arr[2] -ge 7 -and $arr[3] -ge 7) -or ($arr[2] -ge 7 -and $arr[4] -ge 7) -or ($arr[3] -ge 7 -and $arr[4] -ge 7) ){
    #verifica que existe el id de la materia
        if( $idMateria.Contains($arr[1])){

            if($promocionados.ContainsKey($arr[1])){

                $aux = $promocionados[$arr[1]]
                $promocionados.Remove( $arr[1] )
                $promocionados.Add($arr[1],$aux+1)
            }
            else{
                $promocionados.Add($arr[1],1)
            }
        }
    }

    #procesamos los que van a final
    elseif( (($arr[2] -ge 4 -and $arr[3] -ge 4) -or ($arr[2] -ge 4 -and $arr[4] -ge 4) -or ($arr[3] -ge 4 -and $arr[4] -ge 4)) -and $arr[5] -eq "" ){
        #verifica que existe el id de la materia
        if( $idMateria.Contains($arr[1])){
            if($final.ContainsKey($arr[1])){

                $aux = $final[$arr[1]]
                $final.Remove( $arr[1] )
                $final.Add($arr[1],$aux+1)
            }
            else{
                $final.Add($arr[1],1)
            }
        }
    }

    #procesamos los que abandonan
    elseif( ($arr[2] -eq "" -and $arr[3] -eq "") -or ($arr[2] -eq "" -and $arr[4] -eq "") -or ($arr[3] -eq "" -and $arr[4] -eq "") ){
        #verifica que existe el id de la materia
        if( $idMateria.Contains($arr[1])){
            if($abandono.ContainsKey($arr[1])){

                $aux = $abandono[$arr[1]]
                $abandono.Remove( $arr[1] )
                $abandono.Add($arr[1],$aux+1)
            }
            else{
                $abandono.Add($arr[1],1)
            }
        }
    }

    #procesamos recursantes

    elseif( $arr[5] -eq "" ){
        #verifica que existe el id de la materia
        if( $idMateria.Contains($arr[1])){
            if($recursa.ContainsKey($arr[1])){

                $aux = $recursa[$arr[1]]
                $recursa.Remove( $arr[1] )
                $recursa.Add($arr[1],$aux+1)
            }
            else{
                $recursa.Add($arr[1],1)
            }
        }
    }

}


$salida = @()

foreach($i in $idMateria){

    $salida+= New-Object PsObject -Property ([ordered] @{
        'idMateria: ' = $i
        'descripcion: ' = $descripcion[$i-1]
        'promocionados: ' = $promocionados[$i]
        'final: ' = $final[$i]
        'recursaron: ' = $recursa[$i]
        'abandonaron: ' = $abandono[$i]
    })
}

$salida | ConvertTo-Json | Out-File salida.json

