// project: dicm2dckv
// file: dicm2dckv.h
// created by jacquesfauquex on 2024-04-04.

#ifndef parseDICM_h
#define parseDICM_h

#import "dckvapi.h"

/*
 read stream up to transfer syntax.
 Valora sopclassidx
 Valora soptsidx
 Además guarda in vbuf todos los bytes leídos de tal forma a poder crear archivos .bin o .dcm
 
 returns sopuid (with eventual space padding)
 */
bool dicmuptosopts(
  uint8_t *kbuf, // buffer matriz de creación de nuevos keys por diferencial
  uint8_t *vbuf, // lectura del valor del atributo returns with sopiuid
  u64 *inloc, // current stream byte index
  u64 *soloc, // offset in valbyes for sop class
  u16 *solen, // length in valbyes for sop class
  u16 *soidx, // index in const char *scstr[]
  u64 *siloc, // offset in valbyes for sop instance uid
  u16 *silen, // length in valbyes for sop instance uid
  u64 *stloc, // offset in valbyes for transfer syntax
  u16 *stlen, // length in valbyes for transfer syntax
  u16 *stidx,  // index in const char *csstr[]
  s16 *siidx      // SOPinstance index
);

bool dicm2dckvInstance(
   uint8_t *kbuf,     // buffer matriz de creación de nuevos keys por diferencial
   uint8_t *vbuf,     // buffer lectura del valor del atributo
   uint8_t *lbuf,
   u32 *vlen,      // buffer lectura 4-bytes ll de atributos largos
   u64 *inloc,     // offstet en stream
   u32 beforebyte, // limite superior de lectura
   u32 beforetag,  // limite superior attr. first traspassing attr is read en kbuf
   u64 *soloc,     // offset in valbyes for sop class
   u16 *solen,     // length in valbyes for sop class
   u16 *soidx,     // index in const char *scstr[]
   u64 *siloc,     // offset in valbyes for sop instance uid
   u16 *silen,     // length in valbyes for sop instance uid
   u64 *stloc,     // offset in valbyes for transfer syntax
   u16 *stlen,     // length in valbyes for transfer syntax
   u16 *stidx,     // index in const char *csstr[]
   s16 *siidx      // SOPinstance index
);

/*
 RECURSIVE
 parser
 */
bool dicm2dckvDataset(
   uint8_t *kbuf,     // buffer matriz de creación de nuevos keys por diferencial
   u32 kloc,        // offset actual en el búfer matriz (cambia con el nivel de recursión)
   bool readfirstattr,    // true:read desde stream. false:ya está en kbuf
   u16 keycs,          // key charset
   uint8_t *lbuf,
   u32 *vlen,      // buffer lectura 4-bytes ll de atributos largos
   uint8_t *vbuf,     // lectura del valor del atributo
   bool fromStdin,        // ... o from vbuf
   u64 *inloc,           // offstet en stream
   u32 beforebyte,     // limite superior de lectura
   u32 beforetag,       // limite superior attr. Al salir, el attr se encuentra leido y guardado en kbuf
   u16 *soidx,     // index in const char *scstr[]
   u16 *stidx      // index in const char *csstr[]
);




#endif /* parseDICM_h */
