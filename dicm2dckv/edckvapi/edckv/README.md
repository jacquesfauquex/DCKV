# eDCKVinline

- prefixes DCKV with e. The 'e' refers to an **e**xam (study) to which all the attributes  listed belong. DCKV is the format of one instance only.
- eDCKV is a multi instances format
- eDCKVinline creates files 
  - studyuid.edckv (study level)
  - studyuid.sdckv (series level)
  - studyuid.idckv  (instances level)
  - studyuid.pdckv (private data)
  - studyuid.ndckv (native pixel)
  - studyuid.cdckv (compressed pixel)

## Design

eDCKVinline implements the API edckvapi, which overrides dckvapi. The override is performed through a basic implementation of the methods of dckvapi.

- dckvapi_fread -> edckvapi_fread

- dckvapi_fread8 -> edckvapi_fread8

- createdckv -> createedckv. Also retains sop class and transfert syntax.

- commitdckv -> commitedckv

- closedckv -> closeedckv

- appendkv -> [ appendEXAMkv | appendSERIESkv | appendDEFAULTkv | appendPRIVATEkv | appendNATIVEkv | appendCOMPRESSEDkv ]. La implementación de appendkv uses sopclass, la transfert syntax, static lists of attributes fo select one of the destination methods. 

### eDCKVine's implementation of edckvapi

- createedckv creates the 6 files defined above and corresponding buffers

- commitedckv writes the buffers to the corresponding files

- closeedckv closes the 6 files

## encapsulated pixel data

- If encapsulated, it has the Value Representation OB and is an octet-stream resulting from one of the encoding processes. It contains the encoded pixel data stream fragmented into one or more Item(s). This Pixel Data Stream may represent a Single or Multi-frame Image
- The Length of the Data Element (7FE0,0010) shall be set to the Value for Undefined Length (FFFFFFFFH).

FEFF00E0 first no frame fragment

FEFF00E0 fragments

FEFFDDE0 00000000  last fragment
