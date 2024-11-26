// project: dicm2dckv
// file: main.c
// created by jacquesfauquex on 2024-04-04.

#include "dicm2dckv.h"

/*
 main controls dicm2dckv.
 dicm2dckv may be used by any other app controlling dicm2dckv
 all fread get processed by dckvapi.m
 */

int main(int argc, const char * argv[]) {
   /* args:
   0 command name defined by target
   1 loglevel [ D | I | W | E | F ] ( Debug, Info, Warning, Error, Fault )
   2 outdir
   3 (opcional) infile
   */

#pragma mark home dir
   struct stat statparam={0};//for directory creation
   if (stat(argv[2], &statparam)==-1)
   {
      I("create %s",argv[2]);
      if (mkdir(argv[2], 0777)==-1)
      {
         E("%s","failed");
         exit(1);
      }
   }
  
   if (argc < 3){
      return dckvErrorArgs;
   }
   if (!loglevel(argv[1])) exit(dckvErrorLogLevel);
   
   uint8_t *kbuf = malloc(0xFF);//max use 16 bytes x 10 encapsulation levels
   uint8_t *vbuf = malloc(0xFFFE);//max size of vl attribute values
   uint8_t *lbuf = malloc(0x4);//vll bytes
   u32 vlen;//size of large attributes
   u64 inloc;//inputstream index
   u64 soloc,siloc,stloc;//so=sopClass, si=sopInstance, st=sopTransfer
   u16 solen,silen,stlen;
   u16 soidx,stidx;
   
   //file or stdin
   s16 siidx=1;//instances count

   FILE *inFile = NULL;
   if (argc==4) //file specified in args
   {
      inFile=freopen(argv[3],"rb",stdin);
      if (inFile==NULL) return dckvErrorIn;
      siidx=-1;
   }
   else freopen(NULL, "rb", stdin); //from stdin
   setvbuf(stdin, NULL, _IOFBF, 0xFFFE);// | O_NONBLOCK  buffer binario largo 0xFFFE
      
   while (siidx)//if file, siidx==0 after first pass
   {
      if (!dicmuptosopts(
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
          ) return dckvSOPinstanceRejected;
      
      if (!dicm2dckvInstance(
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
          ) return dckvErrorParsing;
   }
 
   if (inFile!=NULL) fclose(inFile);
   free(kbuf);
   free(vbuf);
   free(lbuf);
   return dckvExitOK;
}
