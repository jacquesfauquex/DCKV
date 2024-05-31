# dicm2jsoninline

transforma dicm a DICOMjson con 
- values <= 0xFFFF bytes string, num, bin como base64 inline
- values > 0xFFFF por referencia
