// project: dicm2dckv
// targets: dicm2cda, TODO eDCKVinline,dicm2mdbx,dicmstructdump,...
// file: dckvapi.h
// created by jacquesfauquex on 2024-04-04.//

#ifndef dckvapi_h
#define dckvapi_h

#include "dckvtype.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#pragma mark blocking code

size_t dckvapi_fread(
                     void * __restrict __ptr,
                     size_t __size,
                     size_t __nitems,
                     FILE * __restrict __stream
                     );

struct t4r2l2 {
   u32 t;
   u16 r;
   u16 l;
};

//returns true when it was possible to read the 8 bytes
bool dckvapi_fread8(uint8_t *buffer, u64 *bytesReadRef);


#pragma mark - vr category

enum kvVRcategory{
kvFD,//FD 0 floating point double
   
kvFL,//FL 1 floating point single
   
kvSL,//SL 2 signed long
   
kvSS,//SS 3 signed short
   
kvUL,//UL 4 unsigned long
   
kvUS,//US 5 unsigned short
   
kvAT,//AT 6 attribute tag, 2 u16 hexa
   
kvUI,//UI 7 unique ID eventualmente terminado por 0x00
kvII,//UI 8 SOPInstanceUID
kvIE,//UI 9 StudyInstanceUID
kvIS,//UI 10 SeriesInstanceUID
     //kvPU,//UI 00080019 PyramidUID

kvTP,//AS DT TM DA 11 text short ascii pair length
kvEd,//DA 12 StudyDate

kvTA,//AE DS IS CS 13 text short ascii
kvSm,//CS 14 Modality
kvAt,//CS 15 AccessionNumber type 00080051.00400033 (DNS,EUI64,ISO,URI,UUID,X400,X500,...)
kvIs,//IS 16 SeriesNumber
kvIi,//IS 17 InstanceNumber
kvIa,//IS 18 AcquisitionNumber
     //kvIT,//CS 00080008 ImageType

kvTS,//LO LT SH ST 19 text short charset
kvHC,//ST 20 HL7InstanceIdentifier 0040E001  root^extension
kvEi,//SH 21 StudyID
kvAn,//SH 22 AccessionNumber 00080050
kvdn,//ST 23 DocumentTitle 00420010
kvIN,//LO InstitutionName 00080080
   
kvTL,//UC UT 24 text long charset
kvAl,//UT 25 AccessionNumberIssuer local 00080051.00400031
kvAu,//UT 26 AccessionNumberIssuer universal 00080051.00400032
   
kvTU,//UR 27 text long url-encoded
   
kvPN,//PN 28 person name has a special treatment in json and xml

kved,//OB 29 Encapsulated​Document 00420011
kvfo,//OV 30 Extended​Offset​Table fragments offset 7FE00001
kvfl,//OV 31 Extended​Offset​TableLengths fragments offset 7FE00002
kvft,//UV 32 Encapsulated​Pixel​Data​Value​Total​Length 7FE00003
kv01,//OB OD OF OL OV OW SV UV 33 binary, not textually represented
   
kvUN,//UN 34
   
kvSA,//SQ 35 head
   
kvIA,//36 item head
   
kvIZ,//37 item tail
   
kvSZ//38 SQ tail
};


#pragma mark - tx

bool createtx(
   const char * dstdir,
   uint8_t    * vbuf,
   u64 *soloc,         // offset in valbyes for sop class
   u16 *solen,         // length in valbyes for sop class
   u16 *soidx,         // index in const char *scstr[]
   u64 *siloc,         // offset in valbyes for sop instance uid
   u16 *silen,         // length in valbyes for sop instance uid
   u64 *stloc,         // offset in valbyes for transfer syntax
   u16 *stlen,         // length in valbyes for transfer syntax
   u16 *stidx,         // index in const char *csstr[]
   s16 *siidx          // SOPinstance index
);
bool committx(s16 *siidx);//aplica a todos los kv
bool closetx(s16 *siidx);//aplica a todos los kv

//appendkv uses vStream y puede cargarlo directamente en el buffer de la db
//vbuf es un búfer de 0xFFFF bytes útil para la lectura del stream en otros tipos de implementaciones. Su dimensión corresponde al tamaño máximo de atributos de tipo vl. si vStream is nil, vbuf contains the data of v

bool appendkv(
              uint8_t            *kbuf,
              u32                kloc,
              bool               vlenisl,
              enum kvVRcategory  vrcat,
              u64                vloc,
              u32                vlen,
              bool               fromStdin,
              uint8_t            *vbuf
              );

#endif /* dckvapi_h */
