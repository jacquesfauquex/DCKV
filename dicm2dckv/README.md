# dicm2dckv

- **dicm** refers to the binary format of DICOM

- **dckv** refers to our dicom contextual-key value format of DICOM

- These console apps in  c transform dicm into dckv and subproducts (xml,json,mdbx)

- This is still a work in progress. A definitive rust version is worked out in another repository

- the documentation is found in the wiki part of the github

## comando

Seguimos el modelo natural de streaming de linux (stdin, stdout, stderr).

```
command loglevel outdir
```

- El nombre del comando difiere dependiendo el nombre de la implementación de dckvapi
- loglevel es uno de [ D | I | W | E | F ] ( Debug, Info, Warning, Error, Fault )
- outdir (directorio adónde escribir los resultados )

## dckvapi

- dckv relates one-to-one key bytechains to value bytechains within an ordered set.

- this abstraction is independant of any implementations

- we define an api to which any destination implementation should conform
  
  - structdump was chosen for the prototype implementation
  
  - other implementations include 
    
    - mdbx
    
    - xml
    
    - json

- We use non seekable stdin stream, so that the apps can be piped

## dicm

- we developped specific dicm parsing tools
  
  - subroutine parsing up to tag 0002,0010, with sopclass and transfer syntax recognition
    
    - sop class index
    
    - transfert syntax index
    
    - repertoire index
  
  - convenient exhaustive list of uint64 key representations for patient, study and series level attributes found in the ground dataset
