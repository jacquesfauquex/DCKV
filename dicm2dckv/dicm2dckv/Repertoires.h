//
//  Repertoires.h
//  dicm2mdbx
//
//  Created by jacquesfauquex on 2024-02-28.
//

#ifndef Repertoires_h
#define Repertoires_h

enum repertoireenum{
   
   REPERTOIRE_GL,
   ISO_IR100,
   ISO_IR101,
   ISO_IR109,
   ISO_IR110,
   ISO_IR148,
   ISO_IR126,
   ISO_IR127,
   
   ISO_IR192,
   RFC3986,
   ISO_IR13,
   ISO_IR144,
   ISO_IR138,
   ISO_IR166,
   GB18030,
   GBK,
   
   ISO2022IR6,
   ISO2022IR100,
   ISO2022IR101,
   ISO2022IR109,
   ISO2022IR110,
   ISO2022IR148,
   ISO2022IR126,
   ISO2022IR127,
   
   ISO2022IR87,
   ISO2022IR159,
   ISO2022IR13,
   ISO2022IR144,
   ISO2022IR138,
   ISO2022IR166,
   ISO2022IR149,
   ISO2022IR58
};


uint32 repertoireidx( uint8_t *valbytes, uint16 vallength );
NSString *repertoirename( uint16 repertoireidx);

#endif /* Repertoires_h */
