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

size_t dckvapi_fread(
                     void * __restrict __ptr,
                     size_t __size,
                     size_t __nitems,
                     FILE * __restrict __stream
                     )
{
   return fread(__ptr,__size,nitms,__stream);
}

uint8 swapchar;


//returns true when 8 bytes were read
BOOL dckvapi_fread8(uint8_t *buffer, unsigned long *bytesReadRef)
{
   *bytesReadRef=fread(buffer, 1, 8, stdin);
   if (ferror(stdin)){
      E("%s","stdin error");
      return false;
   }
   
   if (*bytesReadRef==8){
      swapchar=*buffer;
      *buffer=*(buffer+1);
      *(buffer+1)=swapchar;
      swapchar=*(buffer+2);
      *(buffer+2)=*(buffer+3);
      *(buffer+3)=swapchar;
   }
   else
   {
      *buffer=0xFF;
      *(buffer+1)=0xFF;
      *(buffer+2)=0xFF;
      *(buffer+3)=0xFF;
   }
   return true;
}

const char *space=" ";
const char *backslash = "\\";

#pragma mark obligatorios formales
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
   sint16 *siidx,         // SOPinstance index
   uint16 *sitot          // SOPinstance total
){
   printf("     144 %s\n","00020001 OB 0000 {156,2}");
   return true;
}
bool committx(sint16 *siidx){return true;}
bool closetx(sint16 *siidx){return true;}


bool appendkv(
              uint8_t            *kbuf,
              unsigned long      kloc,
              BOOL               vlenisl,
              enum kvVRcategory  vrcat,
              unsigned long long vloc,
              unsigned long      vlen,
              BOOL               fromStdin,
              uint8_t            *vbuf
              )
{
   //uint64* attruint64=(uint64*) kbuf+kloc;
   //printf("%*s%016llX\n",kloc+kloc+1,space, CFSwapInt64(*attruint64));
   uint32* t=(uint32*) kbuf+(kloc/4);
   uint16* l=(uint16*) kbuf+((kloc/2)+3);
   uint8 v=*(kbuf+(kloc+4));
   uint8 r=*(kbuf+(kloc+5));
      
   if (vlenisl==vll)
   {
      if (fromStdin && vlen)
      {
#pragma mark ¿replace by seek?

         unsigned long ll=vlen;
         while (ll>0xFFFF)
         {
            if (fread(vbuf,1,0xFFFF,stdin)!=0xFFFF) return false;
            ll-=0xFFFF;
         }
         if (ll && (!feof(stdin)) && (fread(vbuf,1,ll,stdin)!=ll)) return false;
      }
      switch (vrcat) {
            
         case kvBIN://not representable
         case kvTXT://texts ascii or charset
         {
            if (kloc > 0)printf("%8lld%*s%08X %08X %c%c %04X {%llu,%lu}\n",vloc,kloc+1,space,CFSwapInt32(*((uint32*) kbuf+((kloc/4)-1))),CFSwapInt32(*t),v,r,*l,vloc+12,vlen);
            else printf("%8lld %08X %c%c %04X {%llu,%lu}\n",vloc,CFSwapInt32(*t),v,r,*l,vloc+12,vlen);
         }break;
                        
            
         case kvSA://SQ head
         {
            if (kloc > 0) printf("%8lld%*s%08X00000000\n",
                                vloc,
                                kloc+1,
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
      if (fromStdin && vlen && (fread(vbuf,1,vlen,stdin)!=vlen)) return false;
      
      switch (vrcat) {
         case kvFD://floating point double
         {
            if (kloc > 0)printf("%8lld%*s%08X %08X %c%c %04X",vloc,kloc+1,space,CFSwapInt32(*((uint32*) kbuf+((kloc/4)-1))),CFSwapInt32(*t),v,r,*l);
            else printf("%8lld %08X %c%c %04X",vloc,CFSwapInt32(*t),v,r,*l);
            if (vlen > 0)
            {
               double *v=(double*)vbuf;
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
            if (kloc > 0)printf("%8lld%*s%08X %08X %c%c %04X",vloc,kloc+1,space,CFSwapInt32(*((uint32*) kbuf+((kloc/4)-1))),CFSwapInt32(*t),v,r,*l);
            else printf("%8lld %08X %c%c %04X",vloc,CFSwapInt32(*t),v,r,*l);
            if (vlen > 0)
            {
               float *v=(float*)vbuf;
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
            if (kloc > 0)printf("%8lld%*s%08X %08X %c%c %04X",vloc,kloc+1,space,CFSwapInt32(*((uint32*) kbuf+((kloc/4)-1))),CFSwapInt32(*t),v,r,*l);
            else printf("%8lld %08X %c%c %04X",vloc,CFSwapInt32(*t),v,r,*l);
            if (vlen > 0)
            {
               sint32 *v=(sint32*)vbuf;
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
            if (kloc > 0)printf("%8lld%*s%08X %08X %c%c %04X",vloc,kloc+1,space,CFSwapInt32(*((uint32*) kbuf+((kloc/4)-1))),CFSwapInt32(*t),v,r,*l);
            else printf("%8lld %08X %c%c %04X",vloc,CFSwapInt32(*t),v,r,*l);
            if (vlen > 0)
            {
               sint16 *v=(sint16*)vbuf;
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
            if (kloc > 0)printf("%8lld%*s%08X %08X %c%c %04X",vloc,kloc+1,space,CFSwapInt32(*((uint32*) kbuf+((kloc/4)-1))),CFSwapInt32(*t),v,r,*l);
            else printf("%8lld %08X %c%c %04X",vloc,CFSwapInt32(*t),v,r,*l);
            if (vlen > 0)
            {
               uint32 *v=(uint32*)vbuf;
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
            if (kloc > 0)printf("%8lld%*s%08X %08X %c%c %04X",vloc,kloc+1,space,CFSwapInt32(*((uint32*) kbuf+((kloc/4)-1))),CFSwapInt32(*t),v,r,*l);
            else printf("%8lld %08X %c%c %04X",vloc,CFSwapInt32(*t),v,r,*l);
            if (vlen > 0)
            {
               uint16 *v=(uint16*)vbuf;
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
            if (kloc > 0)printf("%8lld%*s%08X %08X %c%c %04X",vloc,kloc+1,space,CFSwapInt32(*((uint32*) kbuf+((kloc/4)-1))),CFSwapInt32(*t),v,r,*l);
            else printf("%8lld %08X %c%c %04X",vloc,CFSwapInt32(*t),v,r,*l);
            if (vlen > 0)
            {
               uint16 *v=(uint16*)vbuf;
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
            if (kloc > 0)printf("%8lld%*s%08X %08X %c%c %04X",vloc,kloc+1,space,CFSwapInt32(*((uint32*) kbuf+((kloc/4)-1))),CFSwapInt32(*t),v,r,*l);
            else printf("%8lld %08X %c%c %04X",vloc,CFSwapInt32(*t),v,r,*l);
            if (vlen > 0)
            {
               printf(" (");
               vbuf[vlen]=0x0;//terminate even uid lists
               char *token = strtok(vbuf, backslash);
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
            if (kloc > 0)printf("%8lld%*s%08X %08X %c%c %04X",vloc,kloc+1,space,CFSwapInt32(*((uint32*) kbuf+((kloc/4)-1))),CFSwapInt32(*t),v,r,*l);
            else printf("%8lld %08X %c%c %04X",vloc,CFSwapInt32(*t),v,r,*l);
            if (vlen > 0)
            {
               printf(" (");
               vbuf[vlen]=0x0;//terminate even uid lists
               char *token = strtok(vbuf, backslash);
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
            if (kloc > 0)printf("%8lld%*s%08X %08X %c%c %04X\n",vloc,kloc+1,space,CFSwapInt32(*((uint32*) kbuf+((kloc/4)-1))),CFSwapInt32(*t),v,r,*l);
            else printf("%8lld %08X %c%c %04X\n",vloc,CFSwapInt32(*t),v,r,*l);
         }break;
            
         case kvIZ://item tail
         {
            if (kloc > 0)printf("%8lld%*s%08X %08X %c%c %04X\n",vloc,kloc+1,space,CFSwapInt32(*((uint32*) kbuf+((kloc/4)-1))),CFSwapInt32(*t),v,r,*l);
            else printf("%8lld %08X %c%c %04X\n",vloc,CFSwapInt32(*t),v,r,*l);
         }break;
            
         case kvSZ://SQ tail
         {
            if (kloc > 0) printf("%8lld%*s%08XFFFFFFFF\n",
                                 vloc,
                                 kloc+1,
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
