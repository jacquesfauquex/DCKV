# DICM2DCKV

- Dicom Contextualized Key Value (DCKV) is a flat hashmap parser result lenguage for DICOM objects. The  __[wiki](https://github.com/jacquesfauquex/DCKV/wiki)__ part of the repository contains the documentation of this novel representation.
- DICM2DCKV parser is orders of magnitude faster than any other.
- DCKV fits very well within key value databases. New kinds of PACS storing parsed DICOM studies may rely on it.
- DICM2DCKV exe is written in C and implements a pipe arquitecture with DICM on stdin and the result on stdout.

- FURTHERMORE, DICM2DCKV comes with 2 APIs:
   - _DKV exposes the attributes of an instance
   - EDKV organizes the attributes of every instances of an exam and exposes them using sqlite classified by:
      - EDKV patient and study attributes
      - SDKV series attributes
      - PDKV private attributes
      - IDKV instance attributes
      - FDKV frame attributes


### Examples of implementations:

- _DKV: dicm2cda (extracts the enclosed CDA from a DICM)
- _DKV: dicmstructdump (textual dump of the DICM file)
- EDKV: edckv (study parser)
___
DICOM® is a Registered Mark under the copyright of "National Electrical Manufacturers Association" (USA) 
