//
//  dckvapi.m
//  dicm2mdbx
//
//  Created by jacquesfauquex on 2024-04-04.
//
#include <Foundation/Foundation.h>

#include "dckvapi.h"
#include "os_log.h"

const char *space=" ";

#pragma mark tx
bool urltx(NSURL* url)
{
   //url para todos los kv. Abre una tx.
   //logKstringVloclen no crea transacción ni db
   return true;
}


bool committx(void)
{
   //aplica a todos los kv
   //logKstringVloclen no crea transacción ni db
   return true;
}


bool canceltx(void)
{
   //aplica a todos los kv
   //logKstringVloclen no crea transacción ni db
   return true;
}


#pragma mark - db

//apertura kv
bool createdb(enum kvCategory kv)
{
   //0 -> no se creó, o ya existe, always kvCoerce mode
   //logKstringVloclen: no db
   return true;
}


bool reopendb(enum kvCategory kv)
{
   //0 -> no estaba abierto o no se pudo reabrir
   //logKstringVloclen: no db
   return false;
}


bool existsdb(enum kvCategory kv)
{
   //logKstringVloclen: no db
   return false;
}


#pragma mark - cw
//operaciones exclusivas para primera creación
//requiere que todas las enmiendas este clasificadas por key ascendientes
bool appendkv(
              uint8_t            *kbuf,
              int                klen,
              NSString           *vsource,
              unsigned long long vloc,
              unsigned long      vlen,
              NSInputStream      *vstream,
              uint8_t            *vbuf
              )
{
   //uint64* attruint64=(uint64*) kbuf+klen;
   //printf("%*s%016llX\n",klen+klen+1,space, CFSwapInt64(*attruint64));
   uint32* t=(uint32*) kbuf+(klen/4);
   uint16* l=(uint16*) kbuf+((klen/2)+3);
   uint8 v=*(kbuf+(klen+4));
   uint8 r=*(kbuf+(klen+5));
   if (v==0x00) //SQ A
   {
      if (klen > 0) printf("%8lld%*s%08X00000000\n",
                          vloc,
                          klen+1,
                          space,
                          CFSwapInt32(*t)
                          );
      else printf("%8lld %08X00000000\n",
                  vloc,
                  CFSwapInt32(*t));

   }
   else if (v==0xFF) //SQ E
   {
      if (klen > 0) printf("%8lld%*s%08XFFFFFFFF\n",
                           vloc,
                           klen+1,
                           space,
                           CFSwapInt32(*t)
                           );
      else printf("%8lld %08XFFFFFFFF\n", vloc, CFSwapInt32(*t));
   }
   else if (klen > 0)
   {
    uint32* i=(uint32*) kbuf+((klen/4)-1);
    printf("%8lld%*s%08X %08X %c%c %04X\n",
           vloc,
           klen+1,
           space,
           CFSwapInt32(*i),
           CFSwapInt32(*t),
           v,
           r,
           *l
           );
   }
   else printf("%8lld %08X %c%c %04X\n",
               vloc,
               CFSwapInt32(*t),
               v,
               r,
               *l
               );

   
   return true;
}

bool appendk8v(
               uint64             k8,
               NSString           *vsource,
               unsigned long long vloc,
               unsigned long      vlen,
               NSInputStream      *vStream,
               uint8_t            *vbuf
               )
{
   printf("%8lld %08X %c%c %04X\n",
          vloc,
          CFSwapInt32((uint32)(k8%0x100000000)),
          ((uint32)(k8/0x100000000))&0xff,
          ((uint32)(k8/0x10000000000))&0xff ,
          CFSwapInt16(k8/0x1000000000000)
          );
   return true;

}

#pragma mark - ow
//operaciones de escritura sobre db preexistente reabierta



bool coercekv(
              enum kvCategory    kv,
              uint8_t            *kbuf,
              int                klen,
              uint8_t            *vbuf,
              unsigned long long vlen
              )
{
   return false;
}

bool coercek8v(
               enum kvCategory    kv,
               uint64             k8,
               uint8_t            *vbuf,
               unsigned long long vlen
               )
{
   return false;
}


bool supplementkv(
                  enum kvCategory    kv,
                  uint8_t            *kbuf,
                  int                klen,
                  uint8_t            *vbuf,
                  unsigned long long vlen
                 )
{
   return false;
}


bool supplementk8v(enum kvCategory    kv,
                   uint64             k8,
                   uint8_t            *vbuf,
                   unsigned long long vlen
                   )
{
   return false;
}



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
              )
{
   return false;
}


bool removek8v(
               enum kvCategory    kv,
               uint64             k8,
               uint8_t            *vbuf,
               unsigned long long *vlen
               )
{
   return false;
}
