# dicm2dckv

- **dicm** refers to the binary format of DICOM

- **dckv** refers to our dicom contextual-key value format of DICOM

- These console apps written in  c transform dicm into dckv and subproducts (xml,json,mdbx)


## command

dicm2dckv uses linux streaming (stdin, stdout, stderr).

```
command loglevel outdir
```

- The name of the app may differ depending on the specific implementation of dckvapi.h
- loglevel is one of [ D | I | W | E | F ] ( Debug, Info, Warning, Error, Fault )
- outdir (dir where results are written)

## dckvapi

- dckv relates one-to-one key bytechains to value bytechains within an ordered set.

- this abstraction is independant of any implementations

- we define an api to which any destination implementation should conform
  
  - the implementation "structdump" was chosen for the prototype
  
  - other implementations include 
  
    - "grouplengthremove"
    
    - "mdbx"
    
    - "xml"
    
    - "json"

- all these apps receive non seekable stdin stream, so that data can be piped into them

## dicm

- we developped specific tools for dicm parsing
  
  - subroutine parsing up to tag 0002,0010, with sopclass and transfer syntax recognition
    
    - sop class index
    
    - transfert syntax index
    
    - repertoire index
  
  - convenient exhaustive list of u64 key representations for patient, study and series level attributes found in the ground dataset
