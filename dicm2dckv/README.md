# dicm2_dkv

- **dicm** refers to the binary format of DICOM

- **_dkv** refers to our dicom contextual-key value format of DICOM and relates one-to-one key bytechains to value bytechains within an ordered set.

- this abstraction is independant of any implementations

- dicm2_dkv is the framework  for various console apps written in  c which transform dicm into subproducts based on dckv

- what differentiates all these subproducts is the implementation of same apis. Each of the subproduct is a different target of the same app with the appropriate implementation of the API. The rest of the app is common (file meta information parser, dicm dataset syntactical parser, etc)



## command

dicm2_dkv uses linux streaming (stdin, stdout, stderr).

```
command loglevel outdir [testfile]
```

- The name of the app differ depending on the specific implementation of _dkvapi.h or edkvapi.h
- loglevel is one of [ D | I | W | E | F ] ( Debug, Info, Warning, Error, Fault )
- outdir (dir where results are written)

## APIs

The basic API exposes handlers which manages the instances with separate transactions framed by three handlers:
- _DKVcreate (which provides file meta information)
- _DKVappend (invoked for each of the attributes)
- _DKVcommit (finalizes successfull parsing before closing)
- _DKVclose (automatically called by _DKVcommit, or executed alone)

### basic dckvapi

_DKVappend  realizes the syntactical parsing of the attribute, with the exception of the value, which is not read yet from stdin. This last task is reserved for specific implementation.

Examples of targets adopting this api include:

- structdump (textual dump in stdout)
- dicm2cda (extraction of encapsulatedCDA) 

## edckvapi

edckvapi is a special implementation of dckvapi which provides a higher level edckv api replacing the _DKVappend callback by several callbacks classifying the attributes in categories:

- EDKVappend: (patient and study level attributes)

- SDKVappend: (series level attributes. We add to this category the instance level attributes SR and encapsulatedCDA)

- PDKVappend (odd group and UN attributes)

- IDKVappend: (any other instance level attribute)
 
 
 ### monoframe
 
- appendnativeOB 

- appendnativeOW 

- appendnativeOF 

- appendnativeOD 

- appendnativeOC 


### multiframe

- appendframesOB 

- appendframesOC 


Además, el VR disponible en dckvapi está representado por VRcategory que junta algunos VR similar y define categorias nuevas para atributos fundamentales para un pacs.
