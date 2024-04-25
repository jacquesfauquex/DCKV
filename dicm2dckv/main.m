//
//  main.m
//  dicm2dckv
//
//  Created by jacquesfauquex on 2024-04-03.
//

//C
#include <stdio.h>
#include <sys/time.h>
/* For malloc() */
#include <stdlib.h>
/* For uuid_generate() and uuid_unparse()
#include <uuid/uuid.h>*/

//propietario
#include "dicm2dckv.h"
#include "log.h"
#include "dckvapi.h"
//#include "seriesk8tags.h"

/*
 main administra el control de dicm2dckv para procesar el input.
 dicm2dckv puede incorporarse en otra aplicación, que reemplaza la gestión realizada en main por otra gestión propia
 */



int main(int argc, const char * argv[]) {
   if (argc < 3){
      E("%s","syntax: command DIWEF outdir");
      return errorArgs;
   }

   uint8_t *kbuf = malloc(0xA0);//max use 16 bytes x 10 encapsulation levels
   uint8_t *vbuf = malloc(0xFFFE);//max size of vl attribute values
   uint8_t *lbuf = malloc(0x4);//vll bytes
   uint32 llul;
   uint64 inloc;//inputstream index
   uint64 soloc,siloc,stloc;
   uint16 solen,silen,stlen;
   uint16 soidx,stidx;
   uint16 siidx=1;
   
   FILE *testFILE=NULL;
   if (argc==4) testFILE=freopen(argv[3],"rb",stdin);//para testing
   else freopen(NULL, "rb", stdin);
   setvbuf(stdin, NULL, _IOFBF, 0xFFFE);// | O_NONBLOCK  buffer binario largo 0xFFFE
   
   kbuf[0]=0xFF;
   kbuf[1]=0xFC;
   kbuf[2]=0xFF;
   kbuf[3]=0xFC;

   while ((kbuf[0]==0xFF)&&(kbuf[1]==0xFC)&&(kbuf[2]==0xFF)&&(kbuf[3]==0xFC))
   {
      char *sopiuid=dicmuptosopts(
                         kbuf,
                         vbuf,
                         &inloc,
                         &soloc,
                         &solen,
                         &soidx,
                         &siloc,
                         &silen,
                         &stloc,
                         &stlen,
                         &stidx
                        );
      if (   strlen(sopiuid)>0
          && dicm2dckvInstance(
               argv[2],
               kbuf,
               vbuf,
               lbuf,
               &llul,
               &inloc,
               0xFFFFFFFF, //beforebyte
               0xFFFCFFFC,  //beforetag agradado en dcmtk storescp al final de cada instancia, para delimitarla dentro del stream
               &soloc,
               &solen,
               &soidx,
               &siloc,
               &silen,
               &stloc,
               &stlen,
               &stidx,
               &siidx
          )
         ) D("%s", "dicm2dckv OK");
      else
      {
         E("%s", "dicm2dckv error");
#pragma mark TODO register error log
         //reinitiate storescp loop
         kbuf[0]=0xFF;
         kbuf[1]=0xFC;
         kbuf[2]=0xFF;
         kbuf[3]=0xFC;
      }
      uint32 *attruint32=(uint32*) kbuf;
      NSLog(@"%08X",*attruint32);
     }
 
   if (testFILE!=NULL)fclose(testFILE);
   return exitOK;
}
