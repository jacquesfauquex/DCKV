# edckv

- prefixes DCKV with e. The 'e' refers to an **e**xam (study) to which all the attributes  listed belong. DCKV is the format of one instance only.
- eDCKV is a multi instances format

## Design

eDCKVinline implements the API edckvapi, which overrides dckvapi. The override is performed through a basic implementation of the methods of dckvapi.

- dckvapi_fread -> edckvapi_fread

- dckvapi_fread8 -> edckvapi_fread8

- createdckv -> createedckv. Also retains sop class and transfert syntax.

- commitdckv -> commitedckv

- closedckv -> closeedckv

- appendkv -> [ appendEXAMkv | appendSERIESkv | appendDEFAULTkv | appendPRIVATEkv | appendnative | appendnativeOW | appendnativeOF | appendnativeOD | appendencoded ]. La implementación de appendkv uses sopclass, la transfert syntax, static lists of attributes fo select one of the destination methods. 
