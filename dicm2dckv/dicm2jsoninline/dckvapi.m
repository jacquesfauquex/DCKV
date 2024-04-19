//
//  dckvapi.m
//  dicm2dckv
//
//  Created by jacquesfauquex on 2024-04-04.
//

/*potencialmente multivalue: AE AS AT CS DA DS DT FD FL IS LO OW PN SH SV TM UC UI UV*/
#include <Foundation/Foundation.h>

#include "dckvapi.h"
#include "log.h"

const char *space=" ";
const char *backslash = "\\";

#pragma mark obligatorios formales
bool createtx(
   const char * srcurl,
   const char * dstdir,
   uint8_t    * buFFFF,
   uint64 *soloc,         // offset in valbyes for sop class
   uint16 *solen,         // length in valbyes for sop class
   uint16 *soidx,         // index in const char *scstr[]
   uint64 *siloc,         // offset in valbyes for sop instance uid
   uint16 *silen,         // length in valbyes for sop instance uid
   uint64 *stloc,         // offset in valbyes for transfer syntax
   uint16 *stlen,         // length in valbyes for transfer syntax
   uint16 *stidx,         // index in const char *csstr[]
   uint16 *siidx,         // SOPinstance index
   uint16 *sitot          // SOPinstance total
){
   printf("%s\n",srcurl);
   printf("     144 %s\n","00020001 OB 0000 {156,2}");
   return true;
}
bool committx(uint16 *siidx,uint16 *sitot){return true;}
bool canceltx(uint16 *siidx,uint16 *sitot){return true;}
bool createdb(enum kvDBcategory kvdb){return true;}


#pragma mark - parseo y agregado

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
      if (fromStdin && vlen)
      {
#pragma mark ¿replace by seek?

         unsigned long ll=vlen;
         while (ll>0xFFFF)
         {
            if (fread(buFFFF,1,0xFFFF,stdin)!=0xFFFF) return false;
            ll-=0xFFFF;
         }
         if (ll && (!feof(stdin)) && (fread(buFFFF,1,ll,stdin)!=ll)) return false;
      }
      switch (vrcat) {
            
         case kvBIN://not representable
         case kvTXT://texts ascii or charset
         {
            if (klen > 0)printf("%8lld%*s%08X %08X %c%c %04X {%llu,%lu}\n",vloc,klen+1,space,CFSwapInt32(*((uint32*) kbuf+((klen/4)-1))),CFSwapInt32(*t),v,r,*l,vloc+12,vlen);
            else printf("%8lld %08X %c%c %04X {%llu,%lu}\n",vloc,CFSwapInt32(*t),v,r,*l,vloc+12,vlen);
         }break;
            
         case kvURL://not equal to vr UR. Refers to file or url origin of the stream. nil = unregistered
         {
            if (klen > 0)printf("%8lld%*s%08X %08X %c%c %04X",vloc,klen+1,space,CFSwapInt32(*((uint32*) kbuf+((klen/4)-1))),CFSwapInt32(*t),v,r,*l);
            else printf("%8lld %08X %c%c %04X",vloc,CFSwapInt32(*t),v,r,*l);
            if ((vlen > 0) && (vlen < 0xFFFF))
            {
               printf(" (");
               buFFFF[vlen]=0x0;//terminate even uid lists
               char *token = strtok(buFFFF, backslash);
               while( token != NULL ) {
                     printf( " %s", token );
                     token = strtok(NULL, backslash);
               }
               printf(" )");
            }
            printf("\n");
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
      if (fromStdin && vlen && (fread(buFFFF,1,vlen,stdin)!=vlen)) return false;
      
      switch (vrcat) {
         case kvFD://floating point double
         {
            if (klen > 0)printf("%8lld%*s%08X %08X %c%c %04X",vloc,klen+1,space,CFSwapInt32(*((uint32*) kbuf+((klen/4)-1))),CFSwapInt32(*t),v,r,*l);
            else printf("%8lld %08X %c%c %04X",vloc,CFSwapInt32(*t),v,r,*l);
            if (vlen > 0)
            {
               double *v=(double*)buFFFF;
               printf(" (");
               for (uint16 idx=0; idx<(vlen>>3); idx++)
               {
                  printf(" %f",v[idx]);
               }
               printf(" )");
            }
            printf("\n");
         }break;
            
         case kvFL://floating point single
         {
            if (klen > 0)printf("%8lld%*s%08X %08X %c%c %04X",vloc,klen+1,space,CFSwapInt32(*((uint32*) kbuf+((klen/4)-1))),CFSwapInt32(*t),v,r,*l);
            else printf("%8lld %08X %c%c %04X",vloc,CFSwapInt32(*t),v,r,*l);
            if (vlen > 0)
            {
               float *v=(float*)buFFFF;
               printf(" (");
               for (uint16 idx=0; idx<(vlen>>2); idx++)
               {
                  printf(" %f",v[idx]);
               }
               printf(" )");
            }
            printf("\n");
         }break;
            
         case kvSL://signed long
         {
            if (klen > 0)printf("%8lld%*s%08X %08X %c%c %04X",vloc,klen+1,space,CFSwapInt32(*((uint32*) kbuf+((klen/4)-1))),CFSwapInt32(*t),v,r,*l);
            else printf("%8lld %08X %c%c %04X",vloc,CFSwapInt32(*t),v,r,*l);
            if (vlen > 0)
            {
               sint32 *v=(sint32*)buFFFF;
               printf(" (");
               for (uint16 idx=0; idx<(vlen>>2); idx++)
               {
                  printf(" %d",v[idx]);
               }
               printf(" )");
            }
            printf("\n");
         }break;
            
         case kvSS://signed short
         {
            if (klen > 0)printf("%8lld%*s%08X %08X %c%c %04X",vloc,klen+1,space,CFSwapInt32(*((uint32*) kbuf+((klen/4)-1))),CFSwapInt32(*t),v,r,*l);
            else printf("%8lld %08X %c%c %04X",vloc,CFSwapInt32(*t),v,r,*l);
            if (vlen > 0)
            {
               sint16 *v=(sint16*)buFFFF;
               printf(" (");
               for (uint16 idx=0; idx<(vlen>>1); idx++)
               {
                  printf(" %hd",v[idx]);
               }
               printf(" )");
            }
            printf("\n");
         }break;
            
         case kvUL://unsigned long
         {
            if (klen > 0)printf("%8lld%*s%08X %08X %c%c %04X",vloc,klen+1,space,CFSwapInt32(*((uint32*) kbuf+((klen/4)-1))),CFSwapInt32(*t),v,r,*l);
            else printf("%8lld %08X %c%c %04X",vloc,CFSwapInt32(*t),v,r,*l);
            if (vlen > 0)
            {
               uint32 *v=(uint32*)buFFFF;
               printf(" (");
               for (uint16 idx=0; idx<(vlen>>2); idx++)
               {
                  printf(" %u",v[idx]);
               }
               printf(" )");
            }
            printf("\n");
         }break;
            
         case kvUS://unsigned short
         {
            if (klen > 0)printf("%8lld%*s%08X %08X %c%c %04X",vloc,klen+1,space,CFSwapInt32(*((uint32*) kbuf+((klen/4)-1))),CFSwapInt32(*t),v,r,*l);
            else printf("%8lld %08X %c%c %04X",vloc,CFSwapInt32(*t),v,r,*l);
            if (vlen > 0)
            {
               uint16 *v=(uint16*)buFFFF;
               printf(" (");
               for (uint16 idx=0; idx<(vlen>>1); idx++)
               {
                  printf(" %hu",v[idx]);
               }
               printf(" )");
            }
            printf("\n");
         }break;
            
         case kvAT://attribute tag
         {
            if (klen > 0)printf("%8lld%*s%08X %08X %c%c %04X",vloc,klen+1,space,CFSwapInt32(*((uint32*) kbuf+((klen/4)-1))),CFSwapInt32(*t),v,r,*l);
            else printf("%8lld %08X %c%c %04X",vloc,CFSwapInt32(*t),v,r,*l);
            if (vlen > 0)
            {
               uint16 *v=(uint16*)buFFFF;
               printf(" (");
               for (uint16 idx=0; idx<(vlen>>2); idx+=2)
               {
                  printf(" %04x%04x",v[idx],v[idx+1]);
               }
               printf(" )");
            }
            printf("\n");
         }break;
            
         case kvUI://unique ID
         {
            if (klen > 0)printf("%8lld%*s%08X %08X %c%c %04X",vloc,klen+1,space,CFSwapInt32(*((uint32*) kbuf+((klen/4)-1))),CFSwapInt32(*t),v,r,*l);
            else printf("%8lld %08X %c%c %04X",vloc,CFSwapInt32(*t),v,r,*l);
            if (vlen > 0)
            {
               printf(" (");
               buFFFF[vlen]=0x0;//terminate even uid lists
               char *token = strtok(buFFFF, backslash);
               while( token != NULL ) {
                     printf( " %s", token );
                     token = strtok(NULL, backslash);
               }
               printf(" )");
            }
            printf("\n");
         }break;
            
         case kvTXT://valores representadas por texto
         {
            if (klen > 0)printf("%8lld%*s%08X %08X %c%c %04X",vloc,klen+1,space,CFSwapInt32(*((uint32*) kbuf+((klen/4)-1))),CFSwapInt32(*t),v,r,*l);
            else printf("%8lld %08X %c%c %04X",vloc,CFSwapInt32(*t),v,r,*l);
            if (vlen > 0)
            {
               printf(" (");
               buFFFF[vlen]=0x0;//terminate even uid lists
               char *token = strtok(buFFFF, backslash);
               while( token != NULL ) {
                     printf( " «%s»", token );
                     token = strtok(NULL, backslash);
               }
               printf(" )");
            }
            printf("\n");
         }break;
            
         case kvIA://item head
         {
            if (klen > 0)printf("%8lld%*s%08X %08X %c%c %04X\n",vloc,klen+1,space,CFSwapInt32(*((uint32*) kbuf+((klen/4)-1))),CFSwapInt32(*t),v,r,*l);
            else printf("%8lld %08X %c%c %04X\n",vloc,CFSwapInt32(*t),v,r,*l);
         }break;
            
         case kvIZ://item tail
         {
            if (klen > 0)printf("%8lld%*s%08X %08X %c%c %04X\n",vloc,klen+1,space,CFSwapInt32(*((uint32*) kbuf+((klen/4)-1))),CFSwapInt32(*t),v,r,*l);
            else printf("%8lld %08X %c%c %04X\n",vloc,CFSwapInt32(*t),v,r,*l);
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
