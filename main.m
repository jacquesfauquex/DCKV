#import <Foundation/Foundation.h>

NSString *tagstring(uint32 tag) {
   return [NSString stringWithFormat:@"%08X",
                     ((tag & 0xff000000)>>16)
                    +((tag & 0x00ff0000)>>16)
                    +((tag & 0x0000ff00)<<16)
                    +((tag & 0x000000ff)<<16)];
}

NSString *key(NSString *prefix, uint32 tag, uint16 vr) {
   return [NSString stringWithFormat:@"%@_%@-%c%c",prefix,tagstring(tag),vr & 0xff, vr >> 8];
}

void parse(
           NSData *data,
           uint16 *duobytes,
           uint32 *offset,
           uint64 length,
           uint32 beforetag,
           NSString *keyprefix,
           NSMutableDictionary *hashmap,
           NSError *error)
{
   uint64 itemmaxoffset = length - 4 + *offset;
   while ( *offset < itemmaxoffset ) {
     uint32 tag = duobytes[*offset] + (duobytes[*offset+1] << 16);
     if ( tag < beforetag ) {
       uint16 vr = duobytes[*offset + 2];
       uint16 vl = duobytes[*offset + 3];
       switch (vr) {
         case 0x5341: //AS 4 chars (one value only)
         case 0x4144: //DA 8 chars (one value only)
         case 0x4541: //AE variable length (eventually ended with 0x20)
         case 0x5444: //DT variable length (eventually ended with 0x20)
         case 0x4d54: //TM variable length (eventually ended with 0x20)
         case 0x5343: //CS variable length (eventually ended with 0x20)
         case 0x4853: //SH variable length (eventually ended with 0x20) charset - CR (0D) LF (0A) TAB (09) FF (0B)
         case 0x4f4c: //LO variable length (eventually ended with 0x20) charset + CR (0D) LF (0A) TAB (09) FF (0B)
         case 0x544c: //LT variable length (eventually ended with 0x20) charset + CR (0D) LF (0A) TAB (09) FF (0B)
         case 0x5453: //ST variable length (eventually ended with 0x20) charset + CR (0D) LF (0A) TAB (09) FF (0B)
         case 0x4e50: //PN variable length (eventually ended with 0x20) charset
         case 0x5349: //IS variable length (eventually ended with 0x20)
         case 0x5344: //DS variable length (eventually ended with 0x20)
         case 0x4C53: //SL Signed Long
         case 0x4C55: //UL Unsigned Long
         case 0x5353: //SS Signed Short
         case 0x5355: //US Unsigned Short
         case 0x4C46: //FL Float
         case 0x4446: //DL Double
         case 0x5441: //AT hexBinary 4 bytes
            {
               [hashmap setObject:[NSString stringWithFormat:@"%d+8 : %d",*offset * 2,vl] forKey:key(keyprefix,tag,vr)];
               *offset+=4+vl/2;
            }
            break;
             
         case 0x4955:; //UI eventual padding 0x00 cuidado con end C string ! (opción to optimize rep....)
            {
               [hashmap setObject:[NSString stringWithFormat:@"%d+8 : %d",*offset * 2,vl] forKey:key(keyprefix,tag,vr)];
               *offset+=4+vl/2;
            }
            break;

         case 0x4355: //UC
         case 0x5455: //UT contains one or more paragraphs and Control Characters, CR, LF, FF, and ESC.
                 //may be padded with trailing spaces, which may be ignored, but leading spaces are considered to be significant.
                 //Data Elements with this VR shall not be multi-valued
                 //and therefore character code 5CH (the BACKSLASH "\" in ISO-IR 6) may be used.
         case 0x5255: //UR (URI/URL) always UTF-8
         case 0x5653: //Signed 64-bit Very Long
         case 0x5655: //Unsigned 64-bit Very Long
         case 0x424F: //OB
                      // An octet-stream where the encoding of the contents is specified by the negotiated Transfer Syntax.
                      // OB is a VR that is insensitive to byte ordering
                      // see Section 7.3
                      // The octet-stream shall be padded with a single trailing NULL byte value (00H) when necessary
         case 0x444F: //OD
                      // A stream of 64-bit IEEE 754:1985 floating point words.
                      // OD is a VR that requires byte swapping within each 64-bit word when changing byte ordering
                      // see Section 7.3
         case 0x464F: //OF
                 // A stream of 32-bit IEEE 754:1985 floating point words.
                      // OF is a VR that requires byte swapping within each 32-bit word when changing byte ordering
                      // see Section 7.3
         case 0x4C4F: //OL
                 // A stream of 32-bit words where the encoding of the contents is specified by the negotiated Transfer Syntax.
                      // OL is a VR that requires byte swapping within each word when changing byte ordering
                      // see Section 7.3
         case 0x564F: //OV
                 // A stream of 64-bit words where the encoding of the contents is specified by the negotiated Transfer Syntax.
                      // OV is a VR that requires byte swapping within each word when changing byte ordering
                      // see Section 7.3
         case 0x574F: //OW
                      // A stream of 16-bit words where the encoding of the contents is specified by the negotiated Transfer Syntax.
                      // OW is a VR that requires byte swapping within each word when changing byte ordering
                      // see Section 7.3
         case 0x4E55: //UN
                      // undefined
             {
                 uint32 vll = duobytes[*offset+4] + ( duobytes[*offset+5] << 16 );
                 [hashmap setObject:[NSString stringWithFormat:@"%d+12 : %d",*offset * 2,vll] forKey:key(keyprefix,tag,vr)];
                 *offset+=6+(vll>>1);
             }
             break;
               
         case 0x5153: //SQ
             {
            uint32 itemnumber=1;
            NSString *keySQprefix=[NSString stringWithFormat:@"%@_%@",keyprefix,tagstring(tag)];
            uint32 vll = duobytes[*offset+4] + ( duobytes[*offset+5] << 16 );
            [hashmap setObject:[NSString stringWithFormat:@"%d+12 : %d",*offset * 2,vll] forKey:key(keyprefix,tag,vr)];
            *offset+=6;
            if (vll!=0x0) //not empty defined SQ length sequence
            {
               tag = duobytes[*offset] + (duobytes[*offset+1] << 16);
               while (tag==0xE000FFFE) //itemstart (si no, será end tag o atributo siguiente del nivel superior)
               {
                  uint32 vllitem = duobytes[*offset+2] + ( duobytes[*offset+3] << 16 );
                  NSString *keyitemprefix=[NSString stringWithFormat:@"%@.%08X",keySQprefix,itemnumber];
                  [hashmap setObject:[NSString stringWithFormat:@"%d+8 : %d",*offset * 2,vllitem]
                              forKey:[NSString stringWithFormat:@"%@_00000000-IQ",keyitemprefix]
                  ];//FFFEE000FFFFFFFF
                  *offset+=4;
                  if (vllitem==0xFFFFFFFF) parse(data,duobytes,offset,vll,0xE00DFFFE,keyitemprefix,hashmap,error);
                  else                     parse(data,duobytes,offset,vllitem,0xE00DFFFE,keyitemprefix,hashmap,error);
                  tag = duobytes[*offset] + (duobytes[*offset+1] << 16);
                  if (tag==0xE00DFFFE)
                  {
                     [hashmap setObject:[NSString stringWithFormat:@"%d+8",*offset * 2]  forKey:[NSString stringWithFormat:@"%@_FFFEE00D-IZ",keyitemprefix]];//FFFEE00D00000000
                     *offset+=4;
                     tag = duobytes[*offset] + (duobytes[*offset+1] << 16);
                  }
                  else [hashmap setObject:@""  forKey:[NSString stringWithFormat:@"%@.FFFEE00D-IZ",keyitemprefix]];//FFFEE00D00000000
                  itemnumber++;
               }
            }
            if ( tag==0xE0DDFFFE )
            {
               [hashmap setObject:[NSString stringWithFormat:@"%d+8",*offset * 2] forKey:[NSString stringWithFormat:@"%@.FFFEE0DD_00000000-SZ",keySQprefix]];
               *offset+=4;
               tag = duobytes[*offset] + (duobytes[*offset+1] << 16);
            }
            else [hashmap setObject:@"" forKey:[NSString stringWithFormat:@"%@.FFFEE0DD_00000000-SZ",keySQprefix]];
            }
            break;
       }
     }
     else break;
   }
}

int main(int argc, const char * argv[]) {
   @autoreleasepool {
      NSError *error=nil;
      NSMutableDictionary *hashmap=[NSMutableDictionary dictionary];
//img
      NSData *data1=[NSData dataWithContentsOfFile:@"/Users/jacquesfauquex/dcvvparser/dcvvparser/img.dcm"];
      uint16 *duobytes1=(uint16*)[data1 bytes];
      uint32 duobyte1=72;//after preamble
      uint32 *offset1=&duobyte1;
      parse(
            data1,
            duobytes1,
            offset1,
            (data1.length / 2) - *offset1 ,
            0xFFFFFFFF,
            @"00000001",
            hashmap,
            error
            );
//sr
      NSData *data2=[NSData dataWithContentsOfFile:@"/Users/jacquesfauquex/dcvvparser/dcvvparser/sr.dcm"];
      uint16 *duobytes2=(uint16*)[data2 bytes];
      uint32 duobyte2=72;//after preamble
      uint32 *offset2=&duobyte2;
      parse(
            data2,
            duobytes2,
            offset2,
            (data2.length / 2) - *offset2 ,
            0xFFFFFFFF,
            @"00000002",
            hashmap,
            error
            );

      if (error) NSLog(@"%@",error.description);
      NSLog(@"%@",hashmap.description);
      
   }
   return 0;
}
