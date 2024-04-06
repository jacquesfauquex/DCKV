//
//  main.m
//  dicm2mdbx
//
//  Created by jacquesfauquex on 2024-04-03.
//
#include "dicm2dckv.h"
#include "os_log.h"
#include "dckvapi.h"
#include "seriesk8tags.h"
int main(int argc, const char * argv[]) {
   dicm2mdbxLogger = os_log_create("com.opendicom.dicm2mdbx", "dicm2mdbx");
   @autoreleasepool {
      NSDate *startDate=[NSDate date];
      NSFileManager *fileManager=[NSFileManager defaultManager];
      NSProcessInfo *processInfo=[NSProcessInfo processInfo];
      //NSDictionary *environment=processInfo.environment;
      args=[processInfo arguments];
      if (args.count!=3)
      {
         os_log_error(dicm2mdbxLogger,"requires 3 args (command, source, dest). args count %lu",(unsigned long)args.count);
         return errorArgs;
      }

#pragma mark input stream
      NSInputStream *stream=nil;
      if ([args[1] isEqualToString:@"-"])//stdin
         stream=[NSInputStream inputStreamWithFileAtPath:@"/dev/stdin"];
         /*
         NSFileHandle *readingFileHandle=[NSFileHandle fileHandleWithStandardInput];
         NSData *moreData;
         while ((moreData=[readingFileHandle availableData]) && moreData.length) [inputData appendData:moreData];
         [readingFileHandle closeFile];
          */
      else if ([[args[1] componentsSeparatedByString:@"://"]count]==2)//url
         stream=[NSInputStream inputStreamWithURL:args[1]];
      else //path
         stream=[NSInputStream inputStreamWithFileAtPath:[args[1] stringByExpandingTildeInPath]];
      
      if (!stream)
      {
         os_log_error(dicm2mdbxLogger,"bad source path %@",args[1]);
         return errorIn;
      }
      [stream open];
      if (![stream hasBytesAvailable])
      {
         os_log_error(dicm2mdbxLogger,"bad source path %@",args[1]);
         return errorIn;
      }
      
#pragma mark output folder
      BOOL isDir=false;
      if (![fileManager fileExistsAtPath:args[2] isDirectory:&isDir] || !isDir)
      {
         os_log_error(dicm2mdbxLogger,"bad out folder path %@",args[2]);
         return errorOutPath;
      }

#pragma mark dicmuptosopts
      
      //https://stackoverflow.com/questions/19165134/correct-portable-way-to-interpret-buffer-as-a-struct
      uint8_t *keybytes = malloc(0xFF);//max use 16 bytes x 10 encapsulation levels
      uint8_t *valbytes = malloc(0xFFFF);//max size of vl attribute values
      uint64 inloc=0;//inputstream index
      uint64 soloc;
      uint16 solen;
      uint16 soidx;
      uint64 siloc;
      uint16 silen;
      uint8  stidx;
      uint64 stloc;
      uint16 stlen;
      
      NSString *sopiuid=dicmuptosopts(
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
         path=[NSString stringWithFormat:@"%@/%@.bin",args[2],[[NSUUID UUID]UUIDString]];
      else if (stidx==1) //DICM ile
         path=[NSString stringWithFormat:@"%@/%@.ile.dcm",args[2],sopiuid];
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
               os_log_error(dicm2mdbxLogger,"write %@",path);
               fclose(fp);
               return errorWrite;
            }
            bytesRead=[stream read:valbytes maxLength:0xFFFF];
            inloc+=bytesRead;
         }
         fclose(fp);
         os_log_error(dicm2mdbxLogger, "written %llu bytes to %@",inloc,path);
      }
      else
      {
#pragma mark ele
         if (!createdb(kvDEFAULT)) return errorCreateKV;
         appendk8v(kvDEFAULT, tag00020002, valbytes+soloc, solen);
         appendk8v(kvDEFAULT, tag00020003, valbytes+siloc, silen);
         appendk8v(kvDEFAULT, tag00020010, valbytes+stloc, stlen);

         /*
         char keydepth=0;//base level of the DICM dataset
         uint32 beforetag=0x0;
          */
      
      }
      os_log(dicm2mdbxLogger,"elapsed %F",-[startDate timeIntervalSinceNow]);
   }
   return exitOK;
}
