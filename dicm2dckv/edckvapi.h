// project: dicm2dckv
// targets: eDCKVinline,dicm2mdbx,...
// file: edckvapi.h
// created by jacquesfauquex on 2024-04-04.

#ifndef edckvapi_h
#define edckvapi_h

#include "dckvapi.h"

#include "blake3.h"
//#include <errno.h>

enum EDKVfamily{
   EDKV, //exam
   SDKV, //series
   PDKV, //private
   IDKV, //instance
   FDKV  //frame: native or encoded
   //kvB,//original binario
   //kvC, //compressed
   //kvF, //fastquality j2k
   //kvH, //highquality j2k
   //kvO, //originalquality j2k
};
   
#pragma mark - possibility to overwrite any  read
size_t EDKVfread(
                     void * __restrict __ptr,
                     size_t __size,
                     size_t __nitems,
                     FILE * __restrict __stream
                      );
bool EDKVfread8(uint8_t *buffer, u64 *bytesReadRef);

bool EDKVDICMbuffer(s32 bytes);

//called after preliminar parsing of class, sop instance and transfer syntax
bool EDKVcreate(
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
bool EDKVcommit(s16 *siidx);//aplica a todos los kv

//finalizes the opened tx
bool EDKVclose(s16 *siidx);//aplica a todos los kv



#pragma mark replace _DKVappend of dckvapi (which should not be implemented directly)

bool EDKVappend( //patient and study level attributes
   uint8_t            *kbuf,    //contextualized key value buffer
   u32                kloc,     //offset of current attribute in key
   bool               vlenisl,  //attribute is long (4 bytes) or short (2 bytes)
   enum kvVRcategory  vrcat,    //propietary vr number (ver enum)
   u64                vloc,     //value location in input stream
   u32                vlen,     //value length
   bool               fromStdin,//value to be read, or already read in vbuf
   uint8_t            *vbuf     //buffer for values
);

bool SDKVappend( //series level attributes. We add to this category the instance level attributes SR and encapsulatedCDA
  uint8_t            *kbuf,    //contextualized key value buffer
  u32                kloc,     //offset of current attribute in key
  bool               vlenisl,  //attribute is long (4 bytes) or short (2 bytes)
  enum kvVRcategory  vrcat,    //propietary vr number (ver enum)
  u64                vloc,     //value location in input stream
  u32                vlen,     //value length
  bool               fromStdin,//value to be read, or already read in vbuf
  uint8_t            *vbuf     //buffer for values
);

bool PDKVappend( //odd group and UN attributes
   uint8_t            *kbuf,    //contextualized key value buffer
   u32                kloc,     //offset of current attribute in key
   bool               vlenisl,  //attribute is long (4 bytes) or short (2 bytes)
   enum kvVRcategory  vrcat,    //propietary vr number (ver enum)
   u64                vloc,     //value location in input stream
   u32                vlen,     //value length
   bool               fromStdin,//value to be read, or already read in vbuf
   uint8_t            *vbuf     //buffer for values
);

bool IDKVappend( //any other instance level attribute
   uint8_t            *kbuf,    //contextualized key value buffer
   u32                kloc,     //offset of current attribute in key
   bool               vlenisl,  //attribute is long (4 bytes) or short (2 bytes)
   enum kvVRcategory  vrcat,    //propietary vr number (ver enum)
   u64                vloc,     //value location in input stream
   u32                vlen,     //value length
   bool               fromStdin,//value to be read, or already read in vbuf
   uint8_t            *vbuf     //buffer for values
);

bool appendnative(
  uint8_t            *kbuf,    //contextualized key value buffer
  u32                kloc,     //offset of current attribute in key
  bool               vlenisl,  //attribute is long (4 bytes) or short (2 bytes)
  enum kvVRcategory  vrcat,    //propietary vr number (ver enum)
  u64                vloc,     //value location in input stream
  u32                vlen,     //value length
  bool               fromStdin,//value to be read, or already read in vbuf
  uint8_t            *vbuf     //buffer for values
);

bool appendnativeOW(
  uint8_t            *kbuf,    //contextualized key value buffer
  u32                kloc,     //offset of current attribute in key
  bool               vlenisl,  //attribute is long (4 bytes) or short (2 bytes)
  enum kvVRcategory  vrcat,    //propietary vr number (ver enum)
  u64                vloc,     //value location in input stream
  u32                vlen,     //value length
  bool               fromStdin,//value to be read, or already read in vbuf
  uint8_t            *vbuf     //buffer for values
);

bool appendnativeOF(
  uint8_t            *kbuf,    //contextualized key value buffer
  u32                kloc,     //offset of current attribute in key
  bool               vlenisl,  //attribute is long (4 bytes) or short (2 bytes)
  enum kvVRcategory  vrcat,    //propietary vr number (ver enum)
  u64                vloc,     //value location in input stream
  u32                vlen,     //value length
  bool               fromStdin,//value to be read, or already read in vbuf
  uint8_t            *vbuf     //buffer for values
);

bool appendnativeOD(
  uint8_t            *kbuf,    //contextualized key value buffer
  u32                kloc,     //offset of current attribute in key
  bool               vlenisl,  //attribute is long (4 bytes) or short (2 bytes)
  enum kvVRcategory  vrcat,    //propietary vr number (ver enum)
  u64                vloc,     //value location in input stream
  u32                vlen,     //value length
  bool               fromStdin,//value to be read, or already read in vbuf
  uint8_t            *vbuf     //buffer for values
);

bool appendencoded(
  uint8_t            *kbuf,    //contextualized key value buffer
  u32                kloc,     //offset of current attribute in key
  bool               vlenisl,  //attribute is long (4 bytes) or short (2 bytes)
  enum kvVRcategory  vrcat,    //propietary vr number (ver enum)
  u64                vloc,     //value location in input stream
  u32                vlen,     //value length
  bool               fromStdin,//value to be read, or already read in vbuf
  uint8_t            *vbuf     //buffer for values
);




#endif /* edckvapi_h */
