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
   if (argc < 5){
      E("requires 5 args (command, DIWEF, err, out, in ). args count %d",argc);
      return errorArgs;
   }

   uint8_t *kbuf = malloc(0xFF);//max use 16 bytes x 10 encapsulation levels
   uint8_t *vbuf = malloc(0xFFFF);//max size of vl attribute values
   uint8_t *lbuf = malloc(0x4);//vll bytes
   uint32 llul;//4 bytes little endian buffer for ll reading form stdin
   uint64 inloc=0;//inputstream index
   uint64 soloc,siloc,stloc;
   uint16 solen,silen,stlen;
   uint16 soidx,stidx;

   uint16 siidx=0;
   uint16 sitot=argc-3;
   int argidx;
   for (argidx=4;argidx < argc; argidx++)
   {
      siidx++;
      const char *srcurl=argv[argidx];
   
      if (strcmp(argv[argidx], "-")  == 0){//stdin
          argidx=argc+1;
          freopen(NULL, "rb", stdin);
      } else {
          if (false){// "://" //url
          } else { //path
             if (freopen(argv[argidx],"rb",stdin)==NULL){
                E("freopen error %d: %s",errno,argv[argidx]);
                EXIT_FAILURE;
             }

          }
      }
      setvbuf(stdin, NULL, _IOFBF, 0xFFFF);//buffer binario largo 0xFFFF
      inloc=0;//inputstream index
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
      if ((soidx==0)||(stidx==1)) //not DICM or DICM ile
      {
         char *path=malloc(0xFF);
         if (soidx!=0) sprintf( path, "%s/%s.ile.dcm", argv[3], sopiuid);
         else{
            struct timeval tv;
            gettimeofday(&tv,NULL);
            // seconds
            // microseconds
            sprintf( path, "%s/%ld.%d.bin", argv[3], tv.tv_sec,tv.tv_usec);
         }
         FILE *fp;
         fp=freopen(path, "a", stdout);
         NSInteger bytesRead=inloc;
         ssize_t bytesWritten=0;
         while (!feof(stdin))
         {
            if (bytesRead) bytesWritten=write(1,vbuf,bytesRead);//1=stdout
            if (bytesWritten!=bytesRead)
            {
               E("error writing %s",path);
               fclose(fp);
               return errorWrite;
            }
            bytesRead=fread(vbuf, 1, 0xFFFF, stdin);
            inloc+=bytesRead;
         }
         fclose(fp);
         W("written %llu bytes to %s",inloc,path );
      }
      else if (dicm2dckvInstance(
               srcurl,
               argv[3],
               kbuf,
               vbuf,
               lbuf,
               &llul,
               &inloc,
               0xFFFFFFFF, //beforebyte
               0xFFFFFFFF,  //beforetag
               &soloc,
               &solen,
               &soidx,
               &siloc,
               &silen,
               &stloc,
               &stlen,
               &stidx,
               &siidx,
               &sitot
          )) I("%s", "dicm2dckv OK");
      else E("%s", "dicm2dckv error");
   }
   return exitOK;
}
