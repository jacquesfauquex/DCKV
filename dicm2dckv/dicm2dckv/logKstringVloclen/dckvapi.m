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
bool createdb(enum kvDBcategory kvdb)
{
   //0 -> no se creó, o ya existe, always kvCoerce mode
   //logKstringVloclen: no db
   return true;
}


bool reopendb(enum kvDBcategory kvdb)
{
   //0 -> no estaba abierto o no se pudo reabrir
   //logKstringVloclen: no db
   return false;
}


bool existsdb(enum kvDBcategory kvdb)
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
              BOOL               vll,
              enum kvVRcategory  vrcat,
              NSString           *vurl,
              unsigned long long vloc,
              unsigned long      vlen,
              NSInputStream      *vstream,
              uint8_t            *buFFFF
              )
{
   //uint64* attruint64=(uint64*) kbuf+klen;
   //printf("%*s%016llX\n",klen+klen+1,space, CFSwapInt64(*attruint64));
   uint32* t=(uint32*) kbuf+(klen/4);
   uint16* l=(uint16*) kbuf+((klen/2)+3);
   uint8 v=*(kbuf+(klen+4));
   uint8 r=*(kbuf+(klen+5));      
      
   if (vll)
   {
      if (vstream && vlen && [vstream read:buFFFF maxLength:vlen]!=vlen) return false;
#pragma mark progressive reading of vll data
/*
            if (*ll < 0x10000){
               if (*ll && [stream read:valbytes maxLength:*ll]!=*ll) {
                  //NSLog(@"error");
                  return false;
               } else {
                  [db
                   setData:[NSData dataWithBytesNoCopy:valbytes length:*ll freeWhenDone:false]
                   forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                   error:&error
                  ];
               }
            } else {
               //loop read into NSMutableData
               NSMutableData *lldata=[NSMutableData dataWithLength:*ll];
               while (*ll>0xFFFF)
               {
                  if ([stream read:valbytes maxLength:0xFFFF]!=0xFFFF) {
                     //NSLog(@"error");
                     return false;
                  }
                  [lldata appendBytes:valbytes length: 0xFFFF];
                  *ll-=0xFFFF;
               }
               if (*ll && [stream read:valbytes maxLength:*ll]!=*ll) {
                  //NSLog(@"error");
                  return false;
               }
               [lldata appendBytes:valbytes length: *ll];
               [db
                setData:lldata
                forKey:[NSData dataWithBytesNoCopy:keybytes length:keydepth+8 freeWhenDone:false]
                error:&error
               ];
            }
*/      switch (vrcat) {
         case kvFD://floating point double
         {
            if (klen > 0)
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
         }break;
            
         case kvFL://floating point single
         {
            if (klen > 0)
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
         }break;
            
         case kvTXT://texts ascii or charset
         {
            if (klen > 0)
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
         }break;
            
         case kvURL://not equal to vr UR. Refers to file or url origin of the stream. nil = unregistered
         {
            if (klen > 0)
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
         }break;
            
         case kvBIN://not representable
         {
            if (klen > 0)
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
         }break;
            
         case kvSA://SQ head
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
         }break;

         default: return false;
      }
   }
   else //vl
   {
      if (vstream && vlen && [vstream read:buFFFF maxLength:vlen]!=vlen) return false;
      
      switch (vrcat) {
         case kvFD://floating point double
         {
            if (klen > 0)
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

         }break;
            
         case kvFL://floating point single
         {
            if (klen > 0)
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

         }break;
            
         case kvSL://signed long
         {
            if (klen > 0)
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

         }break;
            
         case kvSS://signed short
         {
            if (klen > 0)
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

         }break;
            
         case kvUL://unsigned long
         {
            if (klen > 0)
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

         }break;
            
         case kvUS://unsigned short
         {
            if (klen > 0)
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

         }break;
            
         case kvAT://attribute tag
         {
            if (klen > 0)
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

         }break;
            
         case kvUI://unique ID
         {
            if (klen > 0)
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

         }break;
            
         case kvTXT://valores representadas por texto
         {
            if (klen > 0)
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

         }break;
            
         case kvIA://item head
         {
            if (klen > 0)
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
         }break;
            
         case kvIZ://item tail
         {
#pragma mark todo (void*)&SZbytes)
            if (klen > 0)
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
         }break;
            
         case kvSZ://SQ tail
         {
            if (klen > 0) printf("%8lld%*s%08XFFFFFFFF\n",
                                 vloc,
                                 klen+1,
                                 space,
                                 CFSwapInt32(*t)
                                 );
            else printf("%8lld %08XFFFFFFFF\n", vloc, CFSwapInt32(*t));
         }break;

         default: return false;
      }
   }
   return true;
}



bool appendk8v(
               uint64             k8,
               BOOL               vll,
               enum kvVRcategory  vrcat,
               NSString           *vurl,
               unsigned long long vloc,
               unsigned long      vlen,
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
              enum kvDBcategory  kvdb,
              uint8_t            *kbuf,
              int                klen,
              uint8_t            *vbuf,
              unsigned long long vlen
              )
{
   return false;
}

bool coercek8v(
               enum kvDBcategory  kvbd,
               uint64             k8,
               uint8_t            *vbuf,
               unsigned long long vlen
               )
{
   return false;
}


bool supplementkv(
                  enum kvDBcategory  kvdb,
                  uint8_t            *kbuf,
                  int                klen,
                  uint8_t            *vbuf,
                  unsigned long long vlen
                 )
{
   return false;
}


bool supplementk8v(enum kvDBcategory  kvdb,
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
               enum kvDBcategory  kvdb,
               uint8_t            *kbuf,
               int                klen,
               uint8_t            *vbuf,
               unsigned long long *vlen
              )
{
   return false;
}


bool removek8v(
               enum kvDBcategory  kvdb,
               uint64             k8,
               uint8_t            *vbuf,
               unsigned long long *vlen
               )
{
   return false;
}
