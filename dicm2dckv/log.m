//
//  log.m
//  dicm2dckv
//
//  Created by jacquesfauquex on 2024-07-31.
//

#import <Foundation/Foundation.h>
#include "log.h"

DIWEFenum DIWEF;
BOOL loglevel(const char * logletter)
{
   switch ((int)*logletter) {
      case 'D':
         DIWEF=D;
         break;
      case 'I':
         DIWEF=I;
         break;
      case 'W':
         DIWEF=W;
         break;
      case 'E':
         DIWEF=E;
         break;
      case 'F':
         DIWEF=F;
         break;
      default:
         return false;
   }
   return true;
}
