# eDCKVinline

- prefixes DCKV with e
- multi instances
- create files 
   - studyuid.edckv (study level)
   - studyuid.sdckv (series level)
   - studyuid.idckv  (frames and instances level)


## encapsulated pixel data

- If encapsulated, it has the Value Representation OB and is an octet-stream resulting from one of the encoding processes. It contains the encoded pixel data stream fragmented into one or more Item(s). This Pixel Data Stream may represent a Single or Multi-frame Image
- The Length of the Data Element (7FE0,0010) shall be set to the Value for Undefined Length (FFFFFFFFH).
