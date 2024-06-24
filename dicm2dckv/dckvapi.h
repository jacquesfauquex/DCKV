//
//  dckvapi.h
//  same api
//  various implementation
//
//  Created by jacquesfauquex on 2024-04-04.
//

#ifndef dckvapi_h
#define dckvapi_h

#import <Foundation/Foundation.h>

//#include <stdio.h>
//#include "stdbool.h"

#pragma mark blocking code

size_t dckvapi_fread(
                     void * __restrict __ptr,
                     size_t __size,
                     size_t __nitems,
                     FILE * __restrict __stream
                     );

struct t4r2l2 {
   uint32 t;
   uint16 r;
   uint16 l;
};

//returns true when it was possible to read the 8 bytes
BOOL dckvapi_fread8(uint8_t *buffer, unsigned long *bytesReadRef);


#pragma mark - vr category

enum kvVRcategory{
kvFD,//floating point double
kvFL,//floating point single
kvSL,//signed long
kvSS,//signed short
kvUL,//unsigned long
kvUS,//unsigned short
kvAT,//attribute tag, 2 uint16 hexa
kvUI,//unique ID eventualmente terminado por 0x00
kvTXT,//texts ascii or charset or url-encoded
kvPN,//person name has a special treatment in json and xml
kvBIN,//binary, not textually represented
kvSA,//SQ head
kvIA,//item head
kvIZ,//item tail
kvSZ//SQ tail
};


#pragma mark - tx

bool createtx(
   const char * dstdir,
   uint8_t    * vbuf,
   uint64 *soloc,         // offset in valbyes for sop class
   uint16 *solen,         // length in valbyes for sop class
   uint16 *soidx,         // index in const char *scstr[]
   uint64 *siloc,         // offset in valbyes for sop instance uid
   uint16 *silen,         // length in valbyes for sop instance uid
   uint64 *stloc,         // offset in valbyes for transfer syntax
   uint16 *stlen,         // length in valbyes for transfer syntax
   uint16 *stidx,         // index in const char *csstr[]
   sint16 *siidx          // SOPinstance index
);
bool committx(sint16 *siidx);//aplica a todos los kv
bool closetx(sint16 *siidx);//aplica a todos los kv

//appendkv uses vStream y puede cargarlo directamente en el buffer de la db
//vbuf es un búfer de 0xFFFF bytes útil para la lectura del stream en otros tipos de implementaciones. Su dimensión corresponde al tamaño máximo de atributos de tipo vl. si vStream is nil, vbuf contains the data of v

bool appendkv(
              uint8_t            *kbuf,
              unsigned long      kloc,
              BOOL               vlenisl,
              enum kvVRcategory  vrcat,
              unsigned long long vloc,
              unsigned long      vlen,
              BOOL               fromStdin,
              uint8_t            *vbuf
              );

#endif /* dckvapi_h */
