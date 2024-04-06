//
//  main.h
//  dicm2mdbx
//
//  Created by jacquesfauquex on 2024-04-03.
//

#ifndef main_h
#define main_h

//required 10.11+
#import <os/log.h>
static os_log_t dicm2mdbxLogger;
//https://developer.apple.com/documentation/os/logging/generating_log_messages_from_your_code?language=objc


enum exitValue{
   exitOK=0,
   errorArgs,
   errorIn,
   errorOutPath,
   errorWrite,
   errorCreateKV
};

#endif /* main_h */
