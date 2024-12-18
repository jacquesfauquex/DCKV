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
bool initdicm2dckv(void);
bool cleanupdicm2dckv(void);


bool dicmuptosopts(
  s16 *siidx      // SOPinstance index
);

bool dicm2dckvInstance(
   u32 beforebyte, // limite superior de lectura
   u32 beforetag,  // limite superior attr. first traspassing attr is read en kbuf
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
   bool fromStdin,        // ... o from vbuf
   u32 beforebyte,     // limite superior de lectura
   u32 beforetag       // limite superior attr. Al salir, el attr se encuentra leido y guardado en kbuf
);




#endif /* parseDICM_h */
