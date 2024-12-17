// project: dicm2dckv
// targets: dicm2cda,dicmstructdump,...
// file: dckvapi.h
// created by jacquesfauquex on 2024-04-04.

#ifndef dckvapi_h
#define dckvapi_h

#include "dckvtypes.h"


#pragma mark - blocking code

size_t dckvapi_fread(
                     void * __restrict __ptr,
                     size_t __size,
                     size_t __nitems,
                     FILE * __restrict __stream
                     );


//returns true when it was possible to read the 8 bytes
bool dckvapi_fread8(uint8_t *buffer, u64 *bytesReadRef);


#pragma mark - propietary vr

//cat  e    s    i    f

enum kvVRcategory{
   
//vl enum
kvFD,//FD 1 floating point double
kvFL,//FL 2 floating point single
kvSL,//SL 3 signed long
kvSS,//SS 4 signed short
kvUL,//UL 5 unsigned long
kvUS,//US 6 unsigned short
                kvspp,//7 US
                kvrows,//8 US
                kvcols,//9 US
                kvalloc,//10 US
                kvstored,//11 US
                kvhigh,//12 US
                kvpixrep,//13 US
                kvplanar,//14 US

kvAT,//AT 15 attribute tag, 2 u16 hexa
   
kvUI,//UI 16 unique ID eventualmente terminado por 0x00
     kveuid,//UI 17 StudyInstanceUID
          kvsuid,//UI 18 SeriesInstanceUID
               kviuid,//UI 19 SOPInstanceUID
               kvpuid,//UI 20 00080019 PyramidUID
               kvcuid,//UI 21 SOP​Instance​UID​Of​Concatenation​Source

kvTP,//22 AS DT TM DA text short ascii pair length
     kvpbirth,//23 DA patient birthdate
     kvedate,//24 DA StudyDate
          kvsdate,//25 DA
          kvstime,//26 TM

kvTA,//27 CS AE DS IS 13 text short ascii
     kvpsex,//28 CS patient sex
     kveat,//29 CS AccessionNumber type 00080051.00400033 (DNS,EUI64,ISO,URI,UUID,X400,X500,...)
          kvsmod,//30 CS 14 Modality
               kvitype,//31 CS 00080008 ImageType
               kvphotocode,//32 CS
   
          kvsnumber,//33 IS SeriesNumber
               kvianumber,//34 IS AcquisitionNumber
               kvinumber,//35 IS InstanceNumber

kvTS,//36 LO LT SH ST text short charset
    kvpay,//37 LO insurance
    kvpide,//38 SH patient id extension
    kvpidr,//39 LO patient id root issuer
    kvimg,//40 LO InstitutionName 00080080
    kvedesc,//41 LO Study name
    kveid,//42 SH StudyID
    kvean,//43 SH AccessionNumber 00080050
    kvecode,//44 SQ/SH Study code 00080100,00080102
        kvsdesc,//45 LO Series name
        kvscdaid,//46 ST HL7InstanceIdentifier 0040E001  root^extension
        kvsdoctitle,//47 ST DocumentTitle 00420010
            kvicomment,//48 LO

kvPN,//49 PN person name has a special treatment in json and xml
    kvpname,//50 PN patient name
    kvcda,//51 PN CDA
    kvref,//52 PN referring
    kvreq,//53 PN requesting
   
kv01,  //54 other OB OD OF OL OV OW SV UV binary
   kvsdocument,//55 OB Encapsulated​Document 00420011 xml cda o pdf
   kvnative, //56 OB 0x7FE00010
   kvencoded, //57 OB 0x7E000010
   kvnativeOW, //58 OW 0x7FE00010
   kvnativeOD, //59 OD 0x7FE00009
   kvnativeOF, //60 OF 0x7FE00008
   kvfo,//61 OV Extended​Offset​Table fragments offset 7FE00001
   kvfl,//62 OV Extended​Offset​TableLengths fragments offset 7FE00002
   kvft,//63 UV Encapsulated​Pixel​Data​Value​Total​Length 7FE00003
   
kvTL,//64 UC UT text long charset
     kveal,//65 UT AccessionNumberIssuer local 00080051.00400031
     kveau,//66 UT AccessionNumberIssuer universal 00080051.00400032

kvTU,//67 UR text long url-encoded
   
kvSA,//68 SQ head
kvIA,//69 item head
kvIZ,//70 item tail
kvSZ,//71 SQ tail

kvUN,//UN 72

};


#pragma mark - TO BE IMPLEMENTED FOR EACH TARGET

bool dicombinarymaxbuffer(s32 bytes);

//called after preliminar parsing of class, sop instance and transfer syntax
bool createdckv(
   uint8_t    * vbuf,
   u64 *soloc,         // offset in vbuf for sop class
   u16 *solen,         // length in vbuf for sop class
   u16 *soidx,         // index in const char *scstr[]
   u64 *siloc,         // offset in vbuf for sop instance uid
   u16 *silen,         // length in vbuf for sop instance uid
   u64 *stloc,         // offset in vbuf for transfer syntax
   u16 *stlen,         // length in vbuf for transfer syntax
   u16 *stidx,         // index in const char *csstr[]
   s16 *siidx          // SOPinstance index (instance numerator starting at 0)
);

//called after parsing successfully all the attributes
bool commitdckv(s16 *siidx);//aplica a todos los kv

//finalizes the opened tx
bool closedckv(s16 *siidx);//aplica a todos los kv

//appendkv called for each parsed attribute, with value already read in vbuf or not.
//vbuf is 0xFFFE bytes long (máx short DICOM vl).

bool appendkv(
              uint8_t            *kbuf,      //contextualized key value buffer
              u32                kloc,       //offset of current attribute in key
              bool               vlenisl,    //attribute is long length (4 bytes) or short length (2 bytes)
              enum kvVRcategory  vrcat,      //propietary vr number (ver enum)
              u64                vloc,       //value location in input stream
              u32                vlen,       //value length
              bool               fromStdin,  //value to be read, or already read in vbuf
              uint8_t            *vbuf       //buffer for values
              );

#endif /* dckvapi_h */
