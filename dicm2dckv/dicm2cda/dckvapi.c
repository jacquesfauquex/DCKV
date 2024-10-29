// project: dicm2dckv
// target: dicm2cda
// file: dckvapi.c
// created by jacquesfauquex on 2024-04-04.

#include "dckvapi.h"

FILE *outFile;
static char *dbpath;

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

//returns true when 8 bytes were read
//possibility to overwrite any tag,vr,length reading
u8 swapchar;
bool dckvapi_fread8(uint8_t *buffer, u64 *bytesReadRef)
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

bool createdckv(
   const char * dstdir,
   uint8_t    * vbuf,
   u64 *soloc,         // offset in valbyes for sop class
   u16 *solen,         // length in valbyes for sop class
   u16 *soidx,         // index in const char *scstr[]
   u64 *siloc,         // offset in valbyes for sop instance uid
   u16 *silen,         // length in valbyes for sop instance uid
   u64 *stloc,         // offset in valbyes for transfer syntax
   u16 *stlen,         // length in valbyes for transfer syntax
   u16 *stidx,         // index in const char *csstr[]
   s16 *siidx          // SOPinstance index
){
   (*siidx)++;
   D("#%d",*siidx);
   
   dbpath=malloc(0xFF);
   strcat(dbpath,dstdir);
   strcat(dbpath, "/");
   char *ibuf = malloc(*silen);
   memcpy(ibuf, vbuf+*siloc+8, *silen);
   strcat(dbpath,ibuf);
   strcat(dbpath, ".dscd.xml");
   outFile=fopen(dbpath, "w");

   return true;
}
bool commitdckv(s16 *siidx){
   return closedckv(siidx);
}
bool closedckv(s16 *siidx){
   fclose(outFile);
   return true;
}

bool appendkv(
              uint8_t           *kbuf,
              u32                kloc,
              bool               vlenisl,
              enum kvVRcategory  vrcat,
              u64                vloc,
              u32                vlen,
              bool               fromStdin,
              uint8_t           *vbuf
              )
{
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
         D("%d\n",vrcat);
         if (fromStdin && vlen)
         {
            u32 toberead=vlen;
            while (toberead>0xFFFE)
            {
               if (fread(vbuf,1,0xFFFE,stdin)!=0xFFFE) return false;
               toberead-=0xFFFE;
               if ((toberead==0)&&(vbuf[0xFFFD]==0x0)&&(fwrite(vbuf ,1, 0xFFFD , outFile)!=0xFFFD));
               else if (fwrite(vbuf ,1, 0xFFFE , outFile)!=0xFFFE) return false;
            }
            if (toberead)
            {
               if ((!feof(stdin)) && (fread(vbuf,1,toberead,stdin)!=toberead)) return false;
               if (vbuf[toberead-1]==0x0) toberead--;
               if (fwrite(vbuf ,1, toberead , outFile)!=toberead) return false;
            }
         }
          
      } break;
      
      default:
      {
         if (fromStdin && vlen)
         {
            while (vlen>0xFFFE)
            {
               if (fread(vbuf,1,0xFFFE,stdin)!=0xFFFE) return false;
               vlen-=0xFFFE;
            }
            if (vlen && (!feof(stdin)) && (fread(vbuf,1,vlen,stdin)!=vlen)) return false;
         }
      }
   }

   return true;
}
