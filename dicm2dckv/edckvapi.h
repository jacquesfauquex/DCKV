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
   nativeOB,
   nativeOW,
   nativeOF,
   nativeOD,
   nativeOC,
   framesOB,
   framesOC
};

bool EDKVcreate(
   u64 soloc,         // offset in vbuf for sop class
   u16 solen,         // length in vbuf for sop class
   u16 soidx,         // index in const char *scstr[]
   u64 siloc,         // offset in vbuf for sop instance uid
   u16 silen,         // length in vbuf for sop instance uid
   u64 stloc,         // offset in vbuf for transfer syntax
   u16 stlen,         // length in vbuf for transfer syntax
   u16 stidx         // index in const char *csstr[]
);
bool EDKVcommit(bool hastrailing);
bool EDKVclose(void);

bool appendEDKV(    int kloc,enum kvVRcategory vrcat,u32 vlen);
bool appendSDKV(    int kloc,enum kvVRcategory vrcat,u32 vlen);
bool appendPDKV(    int kloc,enum kvVRcategory vrcat,u32 vlen);
bool appendIDKV(    int kloc,enum kvVRcategory vrcat,u32 vlen);
bool appendpixelOF(int kloc,enum kvVRcategory vrcat,u32 vlen);//7FE00008
bool appendpixelOD(int kloc,enum kvVRcategory vrcat,u32 vlen);//7FE00009
bool appendpixelOB(int kloc,enum kvVRcategory vrcat,u32 vlen);//7FE00010
bool appendpixelOW(int kloc,enum kvVRcategory vrcat,u32 vlen);
bool appendpixelOL(int kloc,enum kvVRcategory vrcat,u32 vlen);
bool appendpixelOV(int kloc,enum kvVRcategory vrcat,u32 vlen);

#endif /* edckvapi_h */
