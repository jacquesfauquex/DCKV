//
//  main.m
//  dicm2dckv
//
//  Created by jacquesfauquex on 2024-04-03.
//
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
      NSFileManager *fileManager=[NSFileManager defaultManager];
      NSProcessInfo *processInfo=[NSProcessInfo processInfo];
      args=[processInfo arguments];
      if (argc!=5)
      {
         E("requires 5 args (command, DIWEF, err, out, in ). args count %d",argc);
         return errorArgs;
      }

#pragma mark input stream
      NSInputStream *stream=nil;
      NSString *source=nil;
      if ([args[4] isEqualToString:@"-"])//stdin
         stream=[NSInputStream inputStreamWithFileAtPath:@"/dev/stdin"];
         /*
         NSFileHandle *readingFileHandle=[NSFileHandle fileHandleWithStandardInput];
         NSData *moreData;
         while ((moreData=[readingFileHandle availableData]) && moreData.length) [inputData appendData:moreData];
         [readingFileHandle closeFile];
          */
      else if ([[args[4] componentsSeparatedByString:@"://"]count]==2)
      {
         //url
         source=args[4];
         stream=[NSInputStream inputStreamWithURL:[NSURL URLWithString:args[4]]];
      }
      else
      {
         //path
         source=args[4];
         stream=[NSInputStream inputStreamWithFileAtPath:[args[4] stringByExpandingTildeInPath]];
      }
      
      if (!stream)
      {
         E("bad in %s",argv[4]);
         return errorIn;
      }
      [stream open];
      if (![stream hasBytesAvailable])
      {
         E("bad source path %s",argv[4]);
         return errorIn;
      }
      
#pragma mark output folder
      BOOL isDir=false;
      if (![fileManager fileExistsAtPath:args[3] isDirectory:&isDir] || !isDir)
      {
         E("bad out folder path %s",argv[3]);
         return errorOutPath;
      }

#pragma mark dicmuptosopts
      
      //https://stackoverflow.com/questions/19165134/correct-portable-way-to-interpret-buffer-as-a-struct
      uint8_t *keybytes = malloc(0xFF);//max use 16 bytes x 10 encapsulation levels
      uint8_t *valbytes = malloc(0xFFFF);//max size of vl attribute values
      uint64 inloc=0;//inputstream index
      uint64 soloc,siloc,stloc;
      uint16 solen,silen,stlen;
      uint16 soidx,stidx;
      
      NSString *sopiuid=dicmuptosopts(
                         source,
                         keybytes,
                         valbytes,
                         stream,
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
      NSString *path=nil;
      if (sopiuid==nil) //not DICM
         path=[NSString stringWithFormat:@"%@/%@.bin",args[3],[[NSUUID UUID]UUIDString]];
      else if (stidx==1) //DICM ile
         path=[NSString stringWithFormat:@"%@/%@.ile.dcm",args[3],sopiuid];
      if (path!=nil)
      {
         FILE *fp;
         fp=freopen([path cStringUsingEncoding:NSASCIIStringEncoding], "a", stdout);
         NSInteger bytesRead=inloc;
         ssize_t bytesWritten=0;
         while (bytesRead > 0)
         {
            bytesWritten=write(1,valbytes,bytesRead);
            if (bytesWritten!=bytesRead)
            {
               E("write %s",[path cStringUsingEncoding:NSASCIIStringEncoding]);
               fclose(fp);
               return errorWrite;
            }
            bytesRead=[stream read:valbytes maxLength:0xFFFF];
            inloc+=bytesRead;
         }
         fclose(fp);
         E("written %llu bytes to %s",inloc,[path cStringUsingEncoding:NSASCIIStringEncoding]);
      }
      else
      {
#pragma mark ele
         if (!createdb(kvDEFAULT)) return errorCreateKV;
         
         const uint64 key00020002=0x0000554900020002;
         if(!appendkv((uint8_t*)&key00020002,0,false,kvUI,source, soloc, solen,nil,valbytes+soloc)) return false;
         const uint64 key00020003=0x0000554900030002;
         if(!appendkv((uint8_t*)&key00020003,0,false,kvUI,source, siloc, silen,nil,valbytes+siloc)) return false;
         const uint64 key00020010=0x0000554900100002;
         if(!appendkv((uint8_t*)&key00020010,0,false,kvUI,source, stloc, stlen,nil,valbytes+stloc)) return false;

         /*
         char keydepth=0;//base level of the DICM dataset
         uint32 beforetag=0x0;
          */
         uint8 keydepth=0;
         uint64 streamindex=stloc+stlen;
         uint64 *loc=&streamindex;
         uint32 beforetag=0xFFFFFFFF;
         if (!dicm2kvdb(
                        source,
                        keybytes,
                        keydepth,
                        true,
                        0,
                        valbytes,
                        stream,
                        loc,
                        0xFFFFFFFF,
                        beforetag
                        )) E("%s", "dicm2dckv error");
      }
      E("elapsed %F",-[startDate timeIntervalSinceNow]);
   }
   return exitOK;
}
