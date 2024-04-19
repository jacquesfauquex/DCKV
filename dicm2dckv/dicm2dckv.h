//
//  dicm2dckv.h
//  dicm2dckv
//
//  Created by jacquesfauquex on 2024-02-28.
//#import "Repertoires.h"
#ifndef parseDICM_h
#define parseDICM_h

#import <Foundation/Foundation.h>

#import "Repertoires.h"
#import "SopClasses.h"
#import "TransferSyntaxes.h"

//static NSArray *args=nil;

struct t4r2l2 {
   uint32 t;
   uint16 r;
   uint16 l;
};

//returns true when it was possible to read the 8 bytes
BOOL read8(uint8_t *buffer, unsigned long *bytesReadRef);


/*
 read stream up to transfer syntax.
 Valora sopclassidx
 Valora soptsidx
 Además guarda in vbuf todos los bytes leídos de tal forma a poder crear archivos .bin o .dcm
 
 returns sopuid (with eventual space padding)
 */
char *dicmuptosopts(
  uint8_t *kbuf, // buffer matriz de creación de nuevos keys por diferencial
  uint8_t *vbuf, // lectura del valor del atributo returns with sopiuid
  uint64 *inloc, // current stream byte index
  uint64 *soloc, // offset in valbyes for sop class
  uint16 *solen, // length in valbyes for sop class
  uint16 *soidx, // index in const char *scstr[]
  uint64 *siloc, // offset in valbyes for sop instance uid
  uint16 *silen, // length in valbyes for sop instance uid
  uint64 *stloc, // offset in valbyes for transfer syntax
  uint16 *stlen, // length in valbyes for transfer syntax
  uint16 *stidx  // index in const char *csstr[]
);

BOOL dicm2dckvInstance(
   const char * srcurl,
   const char * dstdir,
   uint8_t *kbuf,     // buffer matriz de creación de nuevos keys por diferencial
   uint8_t *vbuf,     // buffer lectura del valor del atributo
   uint8_t *lbuf,
   uint32 *llul,      // buffer lectura 4-bytes ll de atributos largos
   uint64 *inloc,     // offstet en stream
   uint32 beforebyte, // limite superior de lectura
   uint32 beforetag,  // limite superior attr. first traspassing attr is read en kbuf
   uint64 *soloc,     // offset in valbyes for sop class
   uint16 *solen,     // length in valbyes for sop class
   uint16 *soidx,     // index in const char *scstr[]
   uint64 *siloc,     // offset in valbyes for sop instance uid
   uint16 *silen,     // length in valbyes for sop instance uid
   uint64 *stloc,     // offset in valbyes for transfer syntax
   uint16 *stlen,     // length in valbyes for transfer syntax
   uint16 *stidx,     // index in const char *csstr[]
   uint16 *siidx,     // SOPinstance index
   uint16 *sitot      // SOPinstance total
);

/*
 RECURSIVE
 parser
 */
BOOL dicm2dckvDataset(
   const char * srcurl,
   uint8_t *kbuf,     // buffer matriz de creación de nuevos keys por diferencial
   uint8 keydepth,        // offset actual en el búfer matriz (cambia con el nivel de recursión)
   BOOL readfirstattr,    // true:read desde stream. false:ya está en kbuf
   uint16 keycs,          // key charset
   uint8_t *lbuf,
   uint32 *llul,      // buffer lectura 4-bytes ll de atributos largos
   uint8_t *vbuf,     // lectura del valor del atributo
   BOOL fromStdin,        // ... o from vbuf
   uint64 *inloc,           // offstet en stream
   uint32 beforebyte,     // limite superior de lectura
   uint32 beforetag       // limite superior attr. Al salir, el attr se encuentra leido y guardado en kbuf
);




#endif /* parseDICM_h */
