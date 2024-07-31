//
//  dckvapi.m
//  dicm2dckv
//
//  Created by jacquesfauquex on 2024-04-04.
//

/*potencialmente multivalue: AE AS AT CS DA DS DT FD FL IS LO OW PN SH SV TM UC UI UV*/
#include <Foundation/Foundation.h>

#include "dckvapi.h"


//possibility to overwrite any value read
size_t dckvapi_fread(
                     void * __restrict __ptr,
                     size_t __size,
                     size_t __nitems,
                     FILE * __restrict __stream
                     )
{
   return fread(__ptr,__size,__nitems,__stream);
}

uint8 swapchar;


//returns true when 8 bytes were read
//possibility to overwrite any tag,vr,length reading
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
   sint16 *siidx          // SOPinstance index
){
   (*siidx)++;
   D("#%d",*siidx);
   return true;
}
bool committx(sint16 *siidx){
   return closetx(siidx);
}
bool closetx(sint16 *siidx){
   return true;
}


#pragma mark - parseo y agregado

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
   D("%d\n",vrcat);

   switch (vrcat) {
      case kvdn://ST  DocumentTitle 00420010
      {
         if (vlen && (fread(vbuf,1,vlen,stdin)!=vlen)) return false;
         printf("%s","title");
         if (vlen > 0)
         {
            printf(" (");
            vbuf[vlen]=0x0;//terminate even uid lists
            char *token = strtok((char *)vbuf, backslash);
            while( token != NULL ) {
               printf( " «%s»", token );
               token = strtok(NULL, backslash);
            }
            printf(" )");
         }
         printf("\n");
      } break;
      
      case kved://OB encapsulaed document
      {
         printf("%s","doc\n");
         
         unsigned long ll=vlen;
         while (ll>0xFFFE)
         {
            if (fread(vbuf,1,0xFFFE,stdin)!=0xFFFE) return false;
            ll-=0xFFFE;
         }
         if (ll && (!feof(stdin)) && (fread(vbuf,1,ll,stdin)!=ll)) return false;
          
      } break;
      
      default:
      {
         if (fromStdin && vlen && (fread(vbuf,1,vlen,stdin)!=vlen)) return false;
      }
   }

   return true;
}
