// project: dicm2dckv
// file: main.c
// created by jacquesfauquex on 2024-04-04.

#include "dicm2dckv.h"

/*
 main controls dicm2_dkv.
 dicm2_dkv may be used by any other app controlling dicm2_dkv
 all fread get processed by dckvapi.m
 */
 
int main(int argc, const char * argv[]) {
   /* args:
   0 command name defined by target
   1 loglevel [ D | I | W | E | F ] ( Debug, Info, Warning, Error, Fault )
   2 _DKVDICMbuffer (en MB, 0=no dicom binary output)
   3 (opcional alternative to stdin) infile (absolute path only)
   */
   
   if (argc < 3) return dckvErrorArgs;
   if (argc > 4) return dckvErrorArgs;//optional, one path only
   if (!loglevel(argv[1])) exit(dckvErrorLogLevel);

//for debug
chdir("/Users/jacquesfauquex/sqlite_edckv/");
   char cwd[1024];
   getcwd(cwd, sizeof(cwd));
   D("working dir:  %s", cwd);

   int dicombinarymaxsize=atoi(argv[2]);
   if (dicombinarymaxsize < 0) return dckvErrorArgs;
   else if (dicombinarymaxsize > 0)
   {
      if (_DKVDICMbuffer(dicombinarymaxsize*1024*1024)) D("dicom buffer: %d MB", dicombinarymaxsize);
      else E("cannot assign %d MB for dicom buffer",dicombinarymaxsize);
   }
   else D("%s", "no dicom buffer");
   initdicm2dckv();
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
      if (!dicmuptosopts(&siidx)) return dckvSOPinstanceRejected;
      
      if (!dicm2dckvInstance(
                              0xFFFFFFFF, //beforebyte
                              0xFFFCFFFC,  //beforetag agradado en dcmtk storescp al final de cada instancia, para delimitarla dentro del stream
                             &siidx
                             )
          ) return dckvErrorParsing;
   }
   cleanupdicm2dckv();
   if (inFile!=NULL) fclose(inFile);
   return dckvExitOK;
}
