# dicm2dckv

- **dicm** refers to the binary format of DICOM

- **dckv** refers to our dicom contextual-key value format of DICOM and relates one-to-one key bytechains to value bytechains within an ordered set.

- this abstraction is independant of any implementations

- dicm2dckv is the framework  for various console apps written in  c which transform dicm into subproducts based on dckv

- what differentiates all these subproducts is the implementation of a a same api. Each of the subproduct is a different target of the same app with the appropriate implementation of the API. The rest of the app is common (file meta information parser, dicm dataset syntactical parser, etc)



## command

dicm2dckv uses linux streaming (stdin, stdout, stderr).

```
command loglevel outdir [testfile]
```

- The name of the app may differ depending on the specific implementation of dckvapi.h
- loglevel is one of [ D | I | W | E | F ] ( Debug, Info, Warning, Error, Fault )
- outdir (dir where results are written)

## APIs

The basic API exposes handlers which manages the input instances in separate transactions framed by three handlers:
- createdckv (which provides file meta information)
- commitdckv
- closedckv (automatically called by commitdckv, or executed alone)

### basic dckvapi

While the transaction remains open, the implementation of the api receives for each of the attributes one call to the handler appendkv, which offers in parameters the result of the syntactical parsing of the attribute, with the exception of the value, which is not read yet from stdin. This last task is reserved for specific implementation.

Examples of targets adopting this api include:

- structdump (textual dump in stdout)
- dicm2cda (extraction of encapsulatedCDA)

## edckvapi

alternative to dckvapi where the appendkv callback is replaced by several callbacks classifying the attributes in categories:

- appendEXAMkv: (patient and study level attributes)

- appendSERIESkv: (series level attributes. We add to this category the instance level attributes SR and encapsulatedCDA)

- appendPRIVATEkv (odd group and UN attributes)

- appendDEFAULTkv: (any other instance level attribute)
 
 - appendnative
 
 - appendnativeOW
 
 - appendnativeOF
 
 - appendnativeOD

 - appendencoded
