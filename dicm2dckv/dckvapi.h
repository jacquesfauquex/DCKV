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
kvUI,//UI 7 unique ID eventualmente terminado por 0x00
               kvII,//UI 8 SOPInstanceUID
          kvIS,//UI 10 SeriesInstanceUID
     kvIE,//UI 9 StudyInstanceUID
//kvIP,//UI 00080019 PyramidUID

kvFD,//FD 0 floating point double
   
kvFL,//FL 1 floating point single
   
kvSL,//SL 2 signed long
   
kvSS,//SS 3 signed short
   
kvUL,//UL 4 unsigned long
   
kvUS,//US 5 unsigned short
   
kvAT,//AT 6 attribute tag, 2 u16 hexa
   
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
     kvIN,//LO 24 InstitutionName 00080080
   
kvTL,//UC UT 25 text long charset
     kvAl,//UT 26 AccessionNumberIssuer local 00080051.00400031
     kvAu,//UT 27 AccessionNumberIssuer universal 00080051.00400032
   
kvTU,//UR 28 text long url-encoded
   
kvPN,//PN 29 person name has a special treatment in json and xml

          kved,//OB 30 Encapsulated​Document 00420011
               kvfo,//OV 31 Extended​Offset​Table fragments offset 7FE00001
               kvfl,//OV 32 Extended​Offset​TableLengths fragments offset 7FE00002
               kvft,//UV 33 Encapsulated​Pixel​Data​Value​Total​Length 7FE00003
   
kvUN,//UN 34
   
kvSA,//SQ 35 head
   
kvIA,//36 item head
   
kvIZ,//37 item tail
   
kvSZ,//38 SQ tail
   
                    kvNM,//39 image native metadata
      //incluye
      //0x00020010: //UI transfert syntax
      //0x00204000: //LT image comment
      //0x00280002: //US samples
      //0x00280010: //US rows
      //0x00280011: //US columns
      //0x00280101: //US bits
      //0x00280103: //US sign
                    kvNB, //40 0x7FE00010: //OB
                    kvNW, //41 0x7FE00010: //OW
                    kvNF, //42 0x7FE00008: //OF float
                    kvND, //43 0x7FE00009: //OD double
   
                    kvCM, //44 image compressed
      //incluye
      //0x00020010://UI transfert syntax
      //0x00082111: //ST kvTS derivation description
      //0x00204000: //LT image comment
      //0x00280002: //US samples
      //0x00280010: //US rows
      //0x00280011: //US columns
      //0x00280101: //US bits
      //0x00280103: //US sign
                    kvCB, //45 0x7E000010: //OB

kv01  //46 other OB OD OF OL OV OW SV UV 34 binary, not textually represented

};


#pragma mark - TO BE IMPLEMENTED FOR EACH TARGET

//called after preliminar parsing of class, sop instance and transfer syntax
bool createdckv(
   const char * dstdir,
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
