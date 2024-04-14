//
//  dicm2dckvapi.h
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

enum dicm2dckvArgs{
   dicm2dckvCmd=0,
   dicm2dckvIn, //path or http url(://) or - (=stdin)
   dicm2dckvOut,//writeable dir for uuid.bin sopuid.dcm or sopuid.dat (+ sopuid.lck) o -
   dicm2dckvErr,//writeable dir for dicm2dckv.log
   dicm2dckvLogLevel,//
};

static NSArray *args=nil;

struct t4r2l2 {
   uint32 t;
   uint16 r;
   uint16 l;
};

struct l4h4 {
   uint32 l;
   uint32 h;
};

//returns true when it was possible to read the 8 bytes
BOOL read8bytes(
   NSInputStream *stream,
   uint8_t *buffer,
   NSInteger *bytesReadRef
);


/*
 NON RECURSIVE
 read stream up to transfer syntax.
 Valora sopclassidx
 Valora soptsidx
 Además guarda in valbytes todos los bytes leídos de tal forma a poder crear archivos .bin o .dcm
 
 returns true if next attribute is already loaded in keybytes
 */
NSString *dicmuptosopts(
  NSString *source,
  uint8_t *keybytes,     // buffer matriz de creación de nuevos keys por diferencial
  uint8_t *valbytes,     // lectura del valor del atributo returns with sopiuid
  NSInputStream *stream, // input
  uint64 *inloc,         // current stream byte index
  uint64 *soloc,         // offset in valbyes for sop class
  uint16 *solen,         // length in valbyes for sop class
  uint16 *soidx,         // index in const char *scstr[]
  uint64 *siloc,         // offset in valbyes for sop instance uid
  uint16 *silen,         // length in valbyes for sop instance uid
  uint64 *stloc,         // offset in valbyes for transfer syntax
  uint16 *stlen,         // length in valbyes for transfer syntax
  uint16 *stidx          // index in const char *csstr[]
);

/*
 RECURSIVE
 parser
 */
BOOL dicm2kvdb(
   NSString *source,
   uint8_t *keybytes,     // buffer matriz de creación de nuevos keys por diferencial
   uint8 keydepth,        // offset actual en el búfer matriz (cambia con el nivel de recursión)
   BOOL readfirstattr,    // true:read desde stream. false:ya está en keybytes
   uint16 keycs,          // key charset
   uint8_t *valbytes,     // lectura del valor del atributo
   NSInputStream *stream, // input
   uint64 *loc,           // offstet en stream
   uint32 beforebyte,     // limite superior de lectura
   uint32 beforetag       // limite superior attr. Al salir, el attr se encuentra leido y guardado en keybytes
);




#endif /* parseDICM_h */
