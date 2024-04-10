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


enum kvCategory{
   kvDEFAULT=0,//requisito, recibe todo lo que no este en otra categoria
   kvPRIVATE,  //opcional, separa atributos de grupo impar, privados
   kvSERIES,   //opcional, separa atributos de nivel paciente estudio y serie, comunes a todas las instancias. Agregamos en esta categoria los atributos de nivel instancia de objectos SR y encapsulatedCDA, de tal forma que dichos objetos esten completamente accesibles desde una sola column family.
   kvNATIVE,   //opcional, atributos específicos de la representación nativa (bitmap) de la imagen, y también transfer syntax y atributos relativos
   kvFRAGMENTS //opcional, alternativa a NATIVE para las imágenes en sintaxis de transferencia comprimida y también transfer syntax y atributos relativos
};


#pragma mark - tx

bool urltx(NSURL* url);//url para todos los kv. Abre una tx.
bool committx(void);//aplica a todos los kv
bool canceltx(void);//aplica a todos los kv


#pragma mark - db

bool createdb(enum kvCategory kv);//0 -> no se creó, o ya existe, always kvCoerce mode
bool reopendb(enum kvCategory kv);//0 -> no estaba abierto o no se pudo reabrir
bool existsdb(enum kvCategory kv);


#pragma mark - cw
//operaciones exclusivas para primera creación
//requiere que todas las enmiendas este clasificadas por key ascendientes
//vsource vloc es informativo
//uses vStream (may be from url) or vbuf
bool appendkv(
              uint8_t            *kbuf,
              int                klen,
              NSString           *vsource,
              unsigned long long vloc,
              unsigned long      vlen,
              NSInputStream      *vstream,
              uint8_t            *vbuf
              );
bool appendk8v(
               uint64             k8,
               NSString           *vsource,
               unsigned long long vloc,
               unsigned long      vlen,
               NSInputStream      *vStream,
               uint8_t            *vbuf
               );


#pragma mark - ow
//operaciones de escritura sobre db preexistente reabierta

bool coercekv(
              enum kvCategory    kv,
              uint8_t            *kbuf,
              int                klen,
              uint8_t            *vbuf,
              unsigned long long vlen
              );

bool coercek8v(
               enum kvCategory    kv,
               uint64             k8,
               uint8_t            *vbuf,
               unsigned long long vlen
               );


bool supplementkv(
                  enum kvCategory    kv,
                  uint8_t            *kbuf,
                  int                klen,
                  uint8_t            *vbuf,
                  unsigned long long vlen
                 );

bool supplementk8v(enum kvCategory    kv,
                   uint64             k8,
                   uint8_t            *vbuf,
                   unsigned long long vlen
                   );


//operaciones remove (vlen is a pointer)
//requieren vbuf de 0xFFFFFFFF length,
//en el cual se escribe el valor borrado
//vlen máx 0xFFFFFFFF indica que el key no existía
bool removetkv(
               enum kvCategory    kv,
               uint8_t            *kbuf,
               int                klen,
               uint8_t            *vbuf,
               unsigned long long *vlen
              );

bool removek8v(
               enum kvCategory    kv,
               uint64             k8,
               uint8_t            *vbuf,
               unsigned long long *vlen
               );

#endif /* dckvapi_h */
