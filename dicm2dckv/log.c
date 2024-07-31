// dicm2dckv
// log.c

#include "log.h"

enum DIWEFenum DIWEF;
bool loglevel(const char * logletter)
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
