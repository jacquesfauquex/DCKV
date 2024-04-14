# dicm2dckv

- **dicm** refers to the binary format of DICOM

- **dckv** refers to our dicom contextual-key value format of DICOM

- These console apps in objective-c (but mainly c) transform dicm into dckv

- This is still a work in progress. A definitive rust version is worked out in another repository

- the documentation is found in the wiki part of the github

## comando
Seguimos el modelo natural de streaming de linux
```
command loglevel errDest outDest inSource [ inSource ...]
```
- El nombre del comando difiere dependiendo el nombre de la implementación de dckvapi
- loglevel es uno de [ D | I | W | E | F ] ( Debug, Info, Warning, Error, Fault )
- errDest [ - | filePath ]  (stderr o archivo dentro del cual se escribe el log )
- outDest [ - | dirPath ]  (stdout o directorio de output )
- inSource [ - | filePath ]  (stdin o archivo a parsear. Soporta también lista de archivos )

Para procesar un dicom obtenido desde url:
```
curl | command loglevel errDest outDest -
```

## dckvapi

- dckv relates one-to-one key bytechains to value bytechains within an ordered set.

- this abstraction is independant of any implementations

- we define an api to which any destination implementation should conform
  
  - mdbx key value database was chosen for the prototype implementation
  
  - other implementations include 
    
    - other key value database
    
    - in-memory ordered maps
    
    - xml
    
    - json
    
    - edition process followed by dicm serialization
    
    - etc....



## dicm

- we developped specific dicm parsing tools
  
  - subroutine parsing up to tag 0002,0010, with sopclass and transfer syntax recognition
    
    - sop class index
    
    - transfert syntax index
  
    - repertoire index
  
  - conveniant exhaustive list of uint64 key representations for patient, study and series level attributes found in the ground dataset
