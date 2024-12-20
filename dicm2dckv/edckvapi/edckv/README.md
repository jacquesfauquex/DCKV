# edckv

- prefixes DCKV with e. The 'e' refers to an **e**xam (study) to which all the attributes  listed belong. DCKV is the format of one instance only.
- eDCKV is a multi instances format

## Design

eDCKVinline implements the API edckvapi, which overrides dckvapi. The override is performed through a basic implementation of the methods of dckvapi.

- _DKVfread -> EDKVfread

- _DKVfread8 -> EDKVfread8

- _DKVcreate -> EDKVcreate. Also retains sop class and transfert syntax.

- _DKVcommit -> EDKVcommit

- _DKVclose -> EDKVclose

- _DKVappend -> [ EDKVappend | SDKVappend | IDKVappend | PDKVappend | FPDKVappend | appendnativeOB | appendnativeOW | appendnativeOF | appendnativeOD | appendnativeOC | appendframesOB | appendframesOC ]
