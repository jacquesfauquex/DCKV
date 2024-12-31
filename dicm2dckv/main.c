// project: dicm2dckv
// file: main.c
// created by jacquesfauquex on 2024-04-04.

#include "dicm2dckv.h"

//defined global
char *DICMbuf=NULL;//accumulator of stream registering original binary DICM. Referred by external declarations everywhere
u64 DICMidx=0;//associated current pointer
s16 siidx=1;//pointer reset count
uint8_t *kbuf=NULL;//buffer (size 0xFF) for the creation of _DKV and EDKV contextual keys. max use 16 bytes x 10 encapsulation levels
/*
 main controls dicm2_dkv.
 dicm2_dkv may be used by any other app controlling dicm2_dkv
 all fread get processed by dckvapi.m
 */
int main(int argc, const char * argv[]) {
   /* args:
   0  command name defined by target
   1+ (opcional alternative to stdin) infile(s)
   */
   
//environment variables
   
#pragma mark _DKVloglevel
   const char* loglevel = getenv("_DKVloglevel");
   if (loglevel==NULL) loglevel="D";
   //[ D | I | W | E | F ] ( Debug, Info, Warning, Error, Fault )
   
#pragma mark _DKVbeforebyte
   u32 beforebyte=0xFFFFFFFF;
   const char* abeforebyte = getenv("_DKVbeforebyte");
   if (abeforebyte!=NULL) beforebyte=(u32)strtoll(abeforebyte, NULL, 16);
   
#pragma mark _DKVbeforetag
   // agradado en dcmtk storescp al final de cada instancia, para delimitarla dentro del stream
   u32 beforetag=0xFFFCFFFC;
   const char* abeforetag = getenv("_DKVbeforetag");
   if (abeforetag!=NULL) beforetag=(u32)strtoll(abeforetag, NULL, 16);
   
#pragma mark _DKVworkingdir
   const char* workingdir = getenv("_DKVworkingdir");
   if (workingdir!=NULL) chdir(workingdir);
   else chdir("/Users/jacquesfauquex/sqlite_edckv/");
   
   char cwd[1024];
   getcwd(cwd, sizeof(cwd));
   D("working dir:  %s", cwd);

#pragma mark DICMrelativepath DICMbuffermegas
   const char* DICMrelativepath = getenv("DICMrelativepath");
   if (DICMrelativepath==NULL) DICMrelativepath="edckv.dcm";
   //in MB, 0=no dicom binary output
   int DICMmegamax=300;
   const char* aDICMmegamax = getenv("DICMmegamax");
   if (aDICMmegamax!=NULL) DICMmegamax=(u32)atoi(aDICMmegamax);

#pragma mark - args (file or stdin)
   FILE *inFile = NULL;
   if (argc==2) //file specified in args
   {
      inFile=freopen(argv[1],"rb",stdin);
      if (inFile==NULL) return dckvErrorIn;
      siidx=-1;
   }
   else freopen(NULL, "rb", stdin); //from stdin
   setvbuf(stdin, NULL, _IOFBF, 0xFFFE);// | O_NONBLOCK  buffer binario largo 0xFFFE

   
   DICMbuf=malloc(DICMmegamax*1024*1024);
   kbuf = malloc(0xFF);
   while (siidx++)//if file, siidx==0 after first pass
   {
      DICMidx=0;
      if (!dicmuptosopts()) return dckvSOPinstanceRejected;
      if (!dicm2dckvInstance(beforebyte,beforetag)) return dckvErrorParsing;
   }
   if (inFile!=NULL) fclose(inFile);
   return (int)siidx;
}
