# dicmstructdump



- es una variante prototipo de dicm2dckv
- permitió definir la función crucial de dckvapi (escritura de un kv)
- cumple con dckvapi
  - servirá de base para derivar 
    - dicm2json
    - dicm2xml
    - dicm2mdbx
- permite comparación con otras herramientas dicom dump (dcmtk y dcm4chee). No obstante, dicmstructdump no incorpora dicionario, por lo cual tiene ventaja competitiva en particular sobre dcmtk dcmdump.

## comparación: parseo de un CT pesando 552 KB

- dicmstructdump parsea 
  
  - el contenido de los values hasta 0xFFFF bytes
  - incluyendo la separación de los multivalues en sus componentes
  - indica offset y largo para valores de tipo vll
  - no copia el nombre de los atributos desde un diccionario. Contiene exclusivamente diccionarios de sop class, transfer syntax y repertoire

- el ejecutable pesa 129 KB
- tiene dependencia a NSInputStream de Objective-c (a reeemplazar por zmq)
- usa 7 MB de memoria para su ejecución, independientemente del tamaño del objeto dicom

RESULTADO

```
time dicmstructdump ct.dcm -
...
real   0m0.015s
user   0m0.005s
sys    0m0.006s
```

### versus dcmtk dcmdump v3.6.7+ DEV

- usa el diccionario para mostrar el nombre de los atributos

- no separa los componentes de los multivalues

- no registra los offsets de attributos

- el ejecutable pesa 4600 KB y no tiene dependencias

```
time dcmdump -q -M +R 64 +ev ct.dcm
...
real   0m0.063s
user   0m0.032s
sys    0m0.012s
```


### versus dcm4che dcmdump 5.26.1

NOTA: el proyecto dcm4che contiene una carpeta js, con módulos de diccionarios (en particular diccionarios privados de AGFA,SIEMENS,PHILIPS,GNE,etc, que se podrían incorporar en el proyecto dckv por ejemplo dentro de dbs diccionarios mdbx...

- el ejecutable es un script de 2KB que ejecuta desde el motor java (actualmente java 17+):
- dcm4che-tool-dcmdump-5.26.1.jar
- dcm4che-core-5.26.1.jar
- dcm4che-dict-priv-5.26.1.jar
- dcm4che-tool-common-5.26.1.jar
- slf4j-api-1.7.32.jar
- logback-core-1.2.9.jar
- logback-classic-1.2.9.jar
- commons-cli-1.4.jar
  El peso total de los jar está cerca de 2500 KB

dcm4che dcmdump :

- limita el tamaño de los valores representados al ancho del terminal -w (por defecto 78, incluyendo offset y tag descripción)
- no separa los componentes de los multivalues
- no copia el nombre de los atributos desde un diccionario

```
time dcmdump ct.dcm
...
real   0m0.840s
user   0m0.950s
sys    0m0.154s
```

### Resultado

| ejecutable      | tamaño KB |       | velocidad ms |       |
| --------------- | --------- | ----- | ------------ | ----- |
| dicmstructdump  | 129       | mejor | 0.015        | mejor |
| dcmtk dcmdump   | 4600      | x 35  | 0.063        | x 4   |
| dcm4che dcmdump | 2500      | x 19  | 0.840        | x 56  |

¡¡¡ Resultado muy prometedor para dicm2dckv !!! 

De lejos es el mejor candidato para contenedor webassembly  y ejecución desde navegador.


