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
#include "dicm2dckvapi.h"
#include "log.h"
#include "dckvapi.h"
//#include "seriesk8tags.h"

/*
 main administra el control de dicm2dckvapi para procesar el input.
 dicm2dckv puede incorporarse en otra aplicación, que reemplaza la gestión realizada en main por otra gestión propia
 */

int main(int argc, const char * argv[]) {
   @autoreleasepool {
      NSDate *startDate=[NSDate date];      
      if (argc < 5){
         E("requires 5 args (command, DIWEF, err, out, in ). args count %d",argc);
         return errorArgs;
      }

#pragma mark input stream
      NSInputStream *stream=nil;
      const char *source;
      
      if (strcmp(argv[4], "-")  == 0){//stdin
         freopen(NULL, "rb", stdin);
      } else {
#pragma mark TODO
         if (false){// "://" //url
         } else { //path
             source=argv[4];
            if (freopen(argv[4],"rb",stdin)==NULL){
               E("freopen error %d: %s",errno,argv[4]);
               EXIT_FAILURE;
            }

         }
      }
      setvbuf(stdin, NULL, _IOFBF, 0xFFFF);//buffer binario largo 0xFFFF

         
      
#pragma mark dicmuptosopts
      
      //https://stackoverflow.com/questions/19165134/correct-portable-way-to-interpret-buffer-as-a-struct
      uint8_t *keybytes = malloc(0xFF);//max use 16 bytes x 10 encapsulation levels, which is lower than 0xFF
      //use    size_t fread(valbytes, 2, 4, stdin);
      uint8_t *valbytes = malloc(0xFFFF);//max size of vl attribute values
      uint64 inloc=0;//inputstream index
      uint64 soloc,siloc,stloc;
      uint16 solen,silen,stlen;
      uint16 soidx,stidx;
      
      char *sopiuid=dicmuptosopts(
                         keybytes,
                         valbytes,
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
            if (bytesRead) bytesWritten=write(1,valbytes,bytesRead);//1=stdout
            if (bytesWritten!=bytesRead)
            {
               E("error writing %s",path);
               fclose(fp);
               return errorWrite;
            }
            bytesRead=fread(valbytes, 1, 0xFFFF, stdin);
            inloc+=bytesRead;
         }
         fclose(fp);
         W("written %llu bytes to %s",inloc,path );
      }
      else
      {
#pragma mark ele
         if (!createdb(kvDEFAULT)) return errorCreateKV;
         
         const uint64 key00020002=0x0000554902000200;
         if(!appendkv((uint8_t*)&key00020002,0,false,kvUI,source, soloc, solen,false,valbytes+soloc)) return false;
         const uint64 key00020003=0x0000554903000200;
         if(!appendkv((uint8_t*)&key00020003,0,false,kvUI,source, siloc, silen,false,valbytes+siloc)) return false;
         const uint64 key00020010=0x0000554910000200;
         if(!appendkv((uint8_t*)&key00020010,0,false,kvUI,source, stloc, stlen,false,valbytes+stloc)) return false;

         if (!dicm2kvdb(
                        source,
                        keybytes,
                        0,          //keydepth
                        true,       //readfirstattr
                        0,          //keycs
                        valbytes,
                        true,       //fromStdin
                        &inloc,
                        0xFFFFFFFF, //beforebyte
                        0xFFFFFFFF  //beforetag
                        )) E("%s", "dicm2dckv error");
      }
      E("elapsed %F",-[startDate timeIntervalSinceNow]);
   }
   return exitOK;
}
