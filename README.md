# DICM2DCKV

- Dicom Contextualized Key Value (DCKV) is a flat hashmap parser result lenguage for DICOM objects. The  __[wiki](https://github.com/jacquesfauquex/DCKV/wiki)__ part of the repository contains the documentation of this novel  representation.
- This is the base for countless optimizations, thanks to which the parser DICM2DCKV is orders of magnitude faster than any other.
- DCKV fits very well within key value databases. This means it new kind of PACS storing parsed DICOM studies may rely on it.
- the exe DICM2DCKV is written in C and implements a pipe arquitecture with DICM on stdin and the result on stdout.

## structure

The same and unique dataset parser can be completed with many diferent implementations of the value parser. The logics of each of them is contained in a C file and shall conform to the header API.

```
types
  +------------+------------+···
value          value                 
parser       parser
API      implementation1
  |
dataset 
parser
  |
CLI
```


### Examples of implementations:

- dicm2cda (extracts the enclosed CDA from a DICM)
- dicmstructdump (textual dump of the DICM file)
- eDCKVinline (study parser)
- dicm2mdbx (parser to key value database)
- dicm2xml (creates an xml from the result of the parsing)
- dicm2jsonbinref and  dicm2jsoninline (create a json from the result of the parsing)


___
DICOM® is a Registered Mark under the copyright of "National Electrical Manufacturers Association" (USA) 
