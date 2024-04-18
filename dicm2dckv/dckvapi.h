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


enum kvDBcategory{
   kvDEFAULT=0,//requisito, recibe todo lo que no este en otra categoria
   kvPRIVATE,  //opcional, separa atributos de grupo impar, privados
   kvSERIES,   //opcional, separa atributos de nivel paciente estudio y serie, comunes a todas las instancias. Agregamos en esta categoria los atributos de nivel instancia de objectos SR y encapsulatedCDA, de tal forma que dichos objetos esten completamente accesibles desde una sola column family.
   kvNATIVE,   //opcional, atributos específicos de la representación nativa (bitmap) de la imagen, y también transfer syntax y atributos relativos
   kvFRAGMENTS //opcional, alternativa a NATIVE para las imágenes en sintaxis de transferencia comprimida y también transfer syntax y atributos relativos
};

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
kvURL,//not equal to vr UR. Refers to file or url origin of the stream. nil = unregistered
kvBIN,//binary, not textually represented
kvSA,//SQ head
kvIA,//item head
kvIZ,//item tail
kvSZ//SQ tail
};


#pragma mark - tx

bool createtx(
   const char * srcFile,
   const char * dstDir,
   uint8_t    * buFFFF,
   uint64 *soloc,         // offset in valbyes for sop class
   uint16 *solen,         // length in valbyes for sop class
   uint16 *soidx,         // index in const char *scstr[]
   uint64 *siloc,         // offset in valbyes for sop instance uid
   uint16 *silen,         // length in valbyes for sop instance uid
   uint64 *stloc,         // offset in valbyes for transfer syntax
   uint16 *stlen,         // length in valbyes for transfer syntax
   uint16 *stidx          // index in const char *csstr[]
);
bool committx(void);//aplica a todos los kv
bool canceltx(void);//aplica a todos los kv


#pragma mark - db

bool createdb(enum kvDBcategory kvdb);//0 -> no se creó, o ya existe, always kvCoerce mode
bool reopendb(enum kvDBcategory kvdb);//0 -> no estaba abierto o no se pudo reabrir
bool existsdb(enum kvDBcategory kvdb);


#pragma mark - cw (create write) obligatorio
//operaciones exclusivas para creación no categorizada
//requiere que todas las enmiendas este clasificadas por key ascendientes


//appendkv uses vStream y puede cargarlo directamente en el buffer de la db
//buFFFF es un búfer de 0xFFFF bytes útil para la lectura del stream en otros tipos de implementaciones. Su dimensión corresponde al tamaño máximo de atributos de tipo vl. si vStream is nil, buFFFF contains the data of v
bool appendkv(
              uint8_t            *kbuf,
              int                klen,
              BOOL               vll,
              enum kvVRcategory  vrcat,
              const char         *vurl,
              unsigned long long vloc,
              unsigned long      vlen,
              BOOL               fromStdin,
              uint8_t            *buFFFF
              );

#pragma mark - ow ((re)opened write) opcional
//operaciones de escritura sobre db categorizada

bool coercekv(
              enum kvDBcategory  kvdb,
              uint8_t            *kbuf,
              int                klen,
              uint8_t            *vbuf,
              unsigned long long vlen
              );

bool coercek8v(
               enum kvDBcategory  kvdb,
               uint64             k8,
               uint8_t            *vbuf,
               unsigned long long vlen
               );


bool supplementkv(
                  enum kvDBcategory  kvdb,
                  uint8_t            *kbuf,
                  int                klen,
                  uint8_t            *vbuf,
                  unsigned long long vlen
                 );

bool supplementk8v(enum kvDBcategory  kvdb,
                   uint64             k8,
                   uint8_t            *vbuf,
                   unsigned long long vlen
                   );


//operaciones remove (vlen is a pointer)
//requieren vbuf de 0xFFFFFFFF length,
//en el cual se escribe el valor borrado
//vlen máx 0xFFFFFFFF indica que el key no existía
bool removetkv(
               enum kvDBcategory  kvdb,
               uint8_t            *kbuf,
               int                klen,
               uint8_t            *vbuf,
               unsigned long long *vlen
              );

bool removek8v(
               enum kvDBcategory  kvdb,
               uint64             k8,
               uint8_t            *vbuf,
               unsigned long long *vlen
               );

#endif /* dckvapi_h */
