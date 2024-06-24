//
//  main.m
//  dicm2dckv
//
//  Created by jacquesfauquex on 2024-04-03.
//
#include <Foundation/Foundation.h>

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
 main controls dicm2dckv.
 dicm2dckv may be used by any other app controlling dicm2dckv
 all fread get processed by dckvapi.m
 */

int main(int argc, const char * argv[]) {
   
   if (argc < 3){
      E("%s","syntax: command DIWEF outdir");
      return errorArgs;
   }
   /*
   - El nombre del comando difiere dependiendo el nombre de la implementación de dckvapi
   - loglevel es uno de [ D | I | W | E | F ] ( Debug, Info, Warning, Error, Fault )
   - outdir (directorio adónde escribir los resultados )
   */
   
   uint8_t *kbuf = malloc(0xA0);//max use 16 bytes x 10 encapsulation levels
   uint8_t *vbuf = malloc(0xFFFE);//max size of vl attribute values
   uint8_t *lbuf = malloc(0x4);//vll bytes
   uint32 vlen;//size of large attributes
   uint64 inloc;//inputstream index
   uint64 soloc,siloc,stloc;//so=sopClass, si=sopInstance, st=sopTransfer
   uint16 solen,silen,stlen;
   uint16 soidx,stidx;
   
   //file or stdin
   sint16 siidx=1;//instances count

   FILE *file = NULL;
   if (argc==4)
   {
      file=freopen(argv[3],"rb",stdin);//para testing
      if (file==NULL) return errorArgs;
      siidx=-1;
   }
   else freopen(NULL, "rb", stdin);
   setvbuf(stdin, NULL, _IOFBF, 0xFFFE);// | O_NONBLOCK  buffer binario largo 0xFFFE
      
   while (siidx)
   {
      if (   dicmuptosopts(
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
                          )
      
          && dicm2dckvInstance(
               argv[2],
               kbuf,
               vbuf,
               lbuf,
               &vlen,
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
      }
   }
 
   if (file!=NULL)fclose(file);
   return exitOK;
}
