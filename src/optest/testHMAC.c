//
//  testHMAC.c
//  C4
//
//  Created by vincent Moscaritolo on 11/2/15.
//  Copyright © 2015 4th-A Technologies, LLC. All rights reserved.
//


#include <stdio.h>
#include <string.h>
#include "c4.h"
#include "optest.h"


/*
 FIPS140 state Run HMAC known answer self test
 */

typedef struct  {
    char	*msg;
    int		keyLen;
    uint8_t	*key;
    uint8_t sha1[20];
    uint8_t sha244[28];
    uint8_t sha256[32];
    uint8_t sha384[48];
    uint8_t sha512[64];
    uint8_t sha512_256[32];
    uint8_t skein256[32];
    uint8_t skein512[64];
 
} katvector;



typedef struct  {
    char	*msg;
    int		keyLen;
    uint8_t	*key;
    char* label;
    char* context;
    uint8_t sha1[20];
    uint8_t sha244[28];
    uint8_t sha256[32];
    uint8_t sha384[48];
    uint8_t sha512[64];
    uint8_t sha512_256[32];
    uint8_t skein256[32];
    uint8_t skein512[64];
} kdfvector;



static uint8_t* kat_for_algorithm(HASH_Algorithm algor, katvector* kat_vector)
{
    uint8_t *kat = NULL;
    
    switch(algor)
    {
        case kHASH_Algorithm_SHA1:    kat = kat_vector->sha1; break;
        case kHASH_Algorithm_SHA224: kat = kat_vector->sha244; break;
        case kHASH_Algorithm_SHA384: kat = kat_vector->sha384; break;
        case kHASH_Algorithm_SHA256: kat = kat_vector->sha256; break;
        case kHASH_Algorithm_SHA512: kat = kat_vector->sha512; break;
        case kHASH_Algorithm_SHA512_256: kat = kat_vector->sha512_256; break;
        case kHASH_Algorithm_SKEIN256: kat = kat_vector->skein256; break;
        case kHASH_Algorithm_SKEIN512: kat = kat_vector->skein512; break;
        default: kat = NULL;
    }
    return kat;
}



static uint8_t* kdf_for_algorithm(HASH_Algorithm algor, kdfvector* kdf_vector)
{
    uint8_t *kat = NULL;
    
    switch(algor)
    {
        case kHASH_Algorithm_SHA1:    kat = kdf_vector->sha1; break;
        case kHASH_Algorithm_SHA224: kat = kdf_vector->sha244; break;
        case kHASH_Algorithm_SHA384: kat = kdf_vector->sha384; break;
        case kHASH_Algorithm_SHA256: kat = kdf_vector->sha256; break;
        case kHASH_Algorithm_SHA512: kat = kdf_vector->sha512; break;
        case kHASH_Algorithm_SHA512_256: kat = kdf_vector->sha512_256; break;
        case kHASH_Algorithm_SKEIN256: kat = kdf_vector->skein256; break;
        case kHASH_Algorithm_SKEIN512: kat = kdf_vector->skein512; break;
        default: kat = NULL;
    }
    return kat;
}



static MAC_Algorithm mac_for_algorithm(HASH_Algorithm algor)
{
    MAC_Algorithm      mac = kMAC_Algorithm_Invalid;
    
    switch(algor)
    {
         case kHASH_Algorithm_SHA1:
        case kHASH_Algorithm_SHA224:
        case kHASH_Algorithm_SHA384:
        case kHASH_Algorithm_SHA256:
        case kHASH_Algorithm_SHA512:
        case kHASH_Algorithm_SHA512_256:
            mac = kMAC_Algorithm_HMAC; break;

        case kHASH_Algorithm_SKEIN256:
        case kHASH_Algorithm_SKEIN512:
            mac = kMAC_Algorithm_SKEIN; break;
           
        default: mac = kMAC_Algorithm_Invalid;
    }
    return mac;
}


/*
 Run a specific HMAC test and compare against known answer.
 */

static C4Err  testKDFKat(    HASH_Algorithm     hash,
                             uint8_t*           K,
                             unsigned long      Klen,
                             char*              label,
                             uint8_t*           context,
                             unsigned long      contextLen,
                             uint32_t           hashLen,
                             unsigned long      expectedLen,
                             uint8_t *          expected )
{
    C4Err err = kC4Err_NoErr;
 
    MAC_Algorithm mac =  mac_for_algorithm(hash);
    uint8_t kdfBuf[64];
    
     err = MAC_KDF(mac, hash, K, Klen, label, context, contextLen,hashLen,expectedLen, kdfBuf); CKERR;
    
    /* check against know answer */
    err = compareResults( expected, kdfBuf, expectedLen , kResultFormat_Byte, hash_algor_table(hash)); CKERR;
  
done:

    return err;

};


static C4Err TestHMACkat(
            HASH_Algorithm      algor,
            uint8_t *              key,
            size_t				keyLen,
            char *				data,
            uint8_t				dataLen,
            uint8_t *              expected )

{
    C4Err err = kC4Err_NoErr;
    MAC_ContextRef     hmac = kInvalidMAC_ContextRef;
    
    size_t				hashSize = 0;
    size_t				resultLen;
    uint8_t                hmacBuf[64];
    
    err  = MAC_Init(kMAC_Algorithm_HMAC, algor,key, keyLen,  &hmac); CKERR;
    
    err = MAC_HashSize(hmac,  &hashSize);CKERR;
    
    err  = MAC_Update( hmac,  (uint8_t*)data, dataLen);CKERR;
    
    resultLen = hashSize;
    err  = MAC_Final( hmac, hmacBuf, &resultLen);CKERR;
    
    /* check against know answer */
    err = compareResults( expected, hmacBuf, resultLen , kResultFormat_Byte, hash_algor_table(algor)); CKERR;
    
done:
    
    if(!IsNull(hmac))
        MAC_Free(hmac);
    
    return err;
}



C4Err TestHMAC()
{
    C4Err err = kC4Err_NoErr;
    
    int i,j;
    /*
     Test vectors for HMAC known answer test
     NOTE: These examples were taken from FIPS-198 doc
     */
    
    katvector kat_vector_array[] =
    {
        { "Sample #2",
            20,
            (uint8_t *)
            "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d\x3e\x3f\x40\x41\x42\x43",
            
            { 0x09, 0x22, 0xd3, 0x40, 0x5f, 0xaa, 0x3d, 0x19, 0x4f, 0x82,
                0xa4, 0x58, 0x30, 0x73, 0x7d, 0x5c, 0xc6, 0xc7, 0x5d, 0x24 },
            
            { 0xdd, 0xef, 0x0a, 0x40, 0xcb, 0x7d, 0x50, 0xfb,
                0x6e, 0xe6, 0xce, 0xa1, 0x20, 0xba, 0x26, 0xaa,
                0x08, 0xf3, 0x07, 0x75, 0x87, 0xb8, 0xad, 0x1b,
                0x8c, 0x8d, 0x12, 0xc7 },
            
            { 0xb8, 0xf2, 0x0d, 0xb5, 0x41, 0xea, 0x43, 0x09,
                0xca, 0x4e, 0xa9, 0x38, 0x0c, 0xd0, 0xe8, 0x34,
                0xf7, 0x1f, 0xbe, 0x91, 0x74, 0xa2, 0x61, 0x38,
                0x0d, 0xc1, 0x7e, 0xae, 0x6a, 0x34, 0x51, 0xd9 },
            
            { 0x08, 0xbc, 0xb0, 0xda, 0x49, 0x1e, 0x87, 0xad,
                0x9a, 0x1d, 0x6a, 0xce, 0x23, 0xc5, 0x0b, 0xf6,
                0xb7, 0x18, 0x06, 0xa5, 0x77, 0xcd, 0x49, 0x04,
                0x89, 0xf1, 0xe6, 0x23, 0x44, 0x51, 0x51, 0x9f,
                0x85, 0x56, 0x80, 0x79, 0x0c, 0xbd, 0x4d, 0x50,
                0xa4, 0x5f, 0x29, 0xe3, 0x93, 0xf0, 0xe8, 0x7f },
            
            { 0x80, 0x9d, 0x44, 0x05, 0x7c, 0x5b, 0x95, 0x41,
                0x05, 0xbd, 0x04, 0x13, 0x16, 0xdb, 0x0f, 0xac,
                0x44, 0xd5, 0xa4, 0xd5, 0xd0, 0x89, 0x2b, 0xd0,
                0x4e, 0x86, 0x64, 0x12, 0xc0, 0x90, 0x77, 0x68,
                0xf1, 0x87, 0xb7, 0x7c, 0x4f, 0xae, 0x2c, 0x2f,
                0x21, 0xa5, 0xb5, 0x65, 0x9a, 0x4f, 0x4b, 0xa7,
                0x47, 0x02, 0xa3, 0xde, 0x9b, 0x51, 0xf1, 0x45,
                0xbd, 0x4f, 0x25, 0x27, 0x42, 0x98, 0x99, 0x05 },
            
            {   0xBB, 0x06, 0xB4, 0x83, 0xE9, 0x21, 0xC0, 0x9C,
                0x2F, 0x27, 0x50, 0x84, 0xCE, 0xC0, 0x89, 0x7B,
                0xBC, 0xA4, 0x95, 0xA8, 0x93, 0x20, 0x5F, 0x69,
                0xB0, 0xA8, 0x6E, 0xAB, 0x4D, 0xC6, 0xE7, 0x30 },
            
            {   0x2B ,0xE6 ,0x39 ,0x71 ,0x79 ,0x0D ,0xAE ,0x39 ,
                0xB9 ,0x37 ,0x2D ,0x0B ,0x01 ,0x84 ,0xB8 ,0x3A ,
                0x33 ,0x92 ,0x33 ,0x89 ,0xC1 ,0x99 ,0x51 ,0x0E ,
                0x4D ,0x38 ,0x93 ,0xF0 ,0xC9 ,0xC4 ,0xD6 ,0x9F },
            
            { 0x22 ,0x89 ,0x56 ,0x4A ,0x01 ,0x58 ,0x51 ,0xDB ,
                0xCD ,0x80 ,0xC5 ,0x82 ,0x92 ,0x6B ,0x5C ,0x79 ,
                0xDB ,0x47 ,0x74 ,0x62 ,0xEC ,0x26 ,0x0A ,0xFA ,
                0x2E ,0x59 ,0xC6 ,0xD7 ,0x3F ,0x4F ,0xE5 ,0xBC ,
                0x34 ,0x57 ,0x2F ,0x88 ,0x83 ,0xE6 ,0x82 ,0x96 ,
                0x42 ,0xAB ,0x2C ,0x56 ,0x24 ,0x6C ,0x74 ,0xE7 ,
                0x0A ,0xA7 ,0x9A ,0xD7 ,0xA8 ,0x64 ,0xEC ,0x95 ,
                0xD6 ,0x89 ,0xF7 ,0xC9 ,0x6A ,0xA9 ,0x44 ,0xFF },

            
        },
        
        { "Sample #1",
            64,
            (uint8_t *)
            "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"
            "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f"
            "\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f"
            "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d\x3e\x3f",
            
            { 0x4f, 0x4c, 0xa3, 0xd5, 0xd6, 0x8b, 0xa7, 0xcc, 0x0a, 0x12,
                0x08, 0xc9, 0xc6, 0x1e, 0x9c, 0x5d, 0xa0, 0x40, 0x3c, 0x0a },
            
            { 0xa6, 0xa3, 0xe2, 0x7f, 0x8b, 0x4c, 0xfa, 0xf2,
                0xbd, 0x15, 0x7c, 0xc5, 0xe8, 0x7c, 0x5b, 0x5d,
                0x7d, 0x7f, 0x22, 0xb0, 0x09, 0xfa, 0x66, 0x44,
                0xfb, 0xbd, 0x38, 0x13 },
            
            { 0x35, 0x19, 0xf0, 0xcd, 0xdf, 0xa0, 0x90, 0xf8,
                0xac, 0xe8, 0x19, 0xd9, 0xae, 0x85, 0x01, 0x57,
                0x8c, 0x46, 0x92, 0x05, 0x02, 0xc6, 0x2b, 0xaa,
                0x47, 0xbf, 0xe6, 0x01, 0x48, 0x64, 0xa9, 0x3a },
            
            { 0x1d, 0x43, 0x71, 0xaa, 0xf0, 0x55, 0x84, 0x3e,
                0xf0, 0xb9, 0x50, 0xf5, 0x4b, 0xd4, 0xb8, 0x90,
                0xd3, 0x5f, 0x5c, 0xd1, 0x18, 0x31, 0x04, 0x35,
                0x0d, 0xe1, 0xcb, 0xe3, 0x1c, 0xfd, 0xd6, 0xb9,
                0x64, 0x32, 0xa2, 0x0d, 0x73, 0x35, 0x6e, 0xb7,
                0x2d, 0xe6, 0xc4, 0x65, 0xc9, 0xa1, 0x50, 0x23 },
            
            { 0xe7, 0xa9, 0xe4, 0x21, 0x95, 0xcf, 0x36, 0xe6,
                0xf2, 0xd1, 0xe1, 0x31, 0x80, 0x1e, 0x7e, 0x8b,
                0x05, 0xb1, 0x29, 0xa0, 0x8d, 0x73, 0xf3, 0x48,
                0x9b, 0x7e, 0xde, 0x72, 0x54, 0xe4, 0xb0, 0xbe,
                0x49, 0x00, 0x16, 0x04, 0x4f, 0xd0, 0x1c, 0x14,
                0xc2, 0x51, 0x22, 0x30, 0xa0, 0x67, 0x5d, 0xe1,
                0x92, 0xcd, 0xf1, 0xaf, 0x78, 0x6a, 0x8a, 0x5c,
                0x0c, 0xe3, 0xe2, 0xaf, 0x13, 0x8f, 0x21, 0xd1 } ,
            
            { 0xB3, 0x2B, 0xDB, 0x27, 0xC9, 0xF8, 0x57, 0x2E,
                0x87, 0xCF, 0xE8, 0x6A, 0xE6, 0xAF, 0x56, 0xFC,
                0x48, 0xBD, 0xCD, 0xD7, 0x0C, 0x14, 0xA9, 0xDF,
                0x8A, 0x4D, 0xBF, 0x67, 0x74, 0xEE, 0x6C, 0x4B},
            
            {   0x52 ,0x5A ,0x14 ,0x3E ,0xFE ,0xD2 ,0x3C ,0x62 ,
                0xA7 ,0x25 ,0x1A ,0xBB ,0x15 ,0x71 ,0xA6 ,0x0E ,
                0xFA ,0x05 ,0xC1 ,0xE5 ,0xD4 ,0x3C ,0x95 ,0xAE ,
                0x7A ,0x20 ,0x03 ,0x1E ,0x9B ,0xB2 ,0x1C ,0x3B  },
            
            { 0xFC ,0xB6 ,0x01 ,0x90 ,0xF2 ,0x6B ,0x2B ,0xF6 ,
                0xEE ,0xC5 ,0xAE ,0x36 ,0xC1 ,0x5F ,0x33 ,0x43 ,
                0xFD ,0x30 ,0x40 ,0x60 ,0xE2 ,0x56 ,0xE9 ,0x42 ,
                0x15 ,0x0E ,0x8D ,0x1B ,0x81 ,0xB5 ,0x0F ,0x1E ,
                0xC1 ,0xC6 ,0xB7 ,0x04 ,0xC1 ,0xDA ,0xEC ,0x7F ,
                0x5E ,0x00 ,0x31 ,0x80 ,0xD6 ,0xC1 ,0x7B ,0x1A ,
                0xC5 ,0x75 ,0x0C ,0x2B ,0xFD ,0xFB ,0xC1 ,0xA8 ,
                0x0A ,0xCC ,0xBC ,0x7C ,0x12 ,0xE3 ,0xC3 ,0x00 },

            
        },
        
        { "Sample #3",
            100,
            (uint8_t *)
            "\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a\x5b\x5c\x5d\x5e\x5f"
            "\x60\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6a\x6b\x6c\x6d\x6e\x6f"
            "\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\x7b\x7c\x7d\x7e\x7f"
            "\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f"
            "\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f"
            "\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf"
            "\xb0\xb1\xb2\xb3",
            
            { 0xbc, 0xf4, 0x1e, 0xab, 0x8b, 0xb2, 0xd8, 0x02, 0xf3, 0xd0,
                0x5c, 0xaf, 0x7c, 0xb0, 0x92, 0xec, 0xf8, 0xd1, 0xa3, 0xaa },
            
            { 0x9c, 0x29, 0x19, 0x91, 0x2e, 0x44, 0x53, 0xd7,
                0x27, 0x29, 0x2c, 0xee, 0x4c, 0x67, 0x8d, 0x3e,
                0x10, 0x5f, 0xe3, 0x16, 0xf1, 0x19, 0x5e, 0xf9,
                0xcf, 0xaf, 0xe9, 0xf0 },
            
            { 0x2d, 0x7d, 0x0d, 0x7f, 0x3e, 0x52, 0xff, 0xe8,
                0x9d, 0x65, 0xc9, 0x78, 0xf3, 0x9d, 0x55, 0x5b,
                0xb4, 0x8b, 0x0b, 0xa4, 0x8d, 0x5b, 0x6e, 0xb4,
                0x04, 0x65, 0x4a, 0xd1, 0xaf, 0xdb, 0x4c, 0xa3 },
            
            { 0x4f, 0x00, 0xad, 0x30, 0x48, 0x4c, 0xe7, 0xd2,
                0x08, 0x9e, 0xe7, 0xf9, 0x44, 0x6c, 0x9e, 0x07,
                0xeb, 0xa2, 0xc8, 0x18, 0x7c, 0x3c, 0x1d, 0x59,
                0x44, 0x2a, 0xa2, 0xc6, 0x15, 0x0e, 0xfe, 0x74,
                0xab, 0x67, 0xdc, 0x6d, 0x41, 0xe6, 0x15, 0x1a,
                0x1b, 0x8d, 0x2d, 0x42, 0x3e, 0xbd, 0x04, 0x78 },
            
            { 0x5b, 0x28, 0x53, 0x80, 0xd8, 0xf5, 0xb6, 0xee,
                0xa7, 0x32, 0x85, 0xae, 0x63, 0x0b, 0xfa, 0x51,
                0x0d, 0x40, 0xa1, 0xb4, 0x7e, 0x8f, 0xf7, 0x58,
                0xe9, 0x0c, 0x56, 0x16, 0xf2, 0x00, 0x99, 0x51,
                0x18, 0xdf, 0x92, 0xae, 0x6a, 0xfd, 0xd2, 0xf3,
                0x5b, 0x32, 0xe6, 0x77, 0x8f, 0xb6, 0x1b, 0x3e,
                0xc9, 0x95, 0xee, 0x52, 0x3d, 0x56, 0xa4, 0x14,
                0xb4, 0x7c, 0x9e, 0x6c, 0x52, 0x61, 0x3a, 0x82 } ,
            
            {   0xAA, 0x21, 0x40, 0xBC, 0x5C, 0x3C, 0x83, 0x36,
                0x89, 0x16, 0x12, 0x62, 0x3F, 0x5C, 0xCB, 0xE7,
                0x13, 0x10, 0xB2, 0x3D, 0xE9, 0x32, 0xDC, 0xE9,
                0x62, 0xAF, 0x8F, 0x98, 0xFB, 0xD3, 0x87, 0x5B },
            
            {   0xB5 ,0x80 ,0xB2 ,0x26 ,0x4C ,0xF2 ,0x0E ,0x1B ,
                0x77 ,0x86 ,0xC8 ,0x41 ,0x80 ,0x08 ,0xCC ,0xC0 ,
                0x75 ,0x7D ,0xC0 ,0x85 ,0xA2 ,0x5A ,0x87 ,0xDD ,
                0x50 ,0x91 ,0xD8 ,0x72 ,0xEA ,0x39 ,0x11 ,0x88},
            
            { 0x5E ,0xBD ,0xCB ,0x31 ,0x88 ,0x6D ,0x56 ,0x42 ,
                0x5B ,0x0A ,0xC3 ,0xE6 ,0x96 ,0x82 ,0xB6 ,0x52 ,
                0x87 ,0x16 ,0x5F ,0x86 ,0x39 ,0x53 ,0x8E ,0x75 ,
                0x4D ,0x96 ,0x2C ,0xAC ,0x4E ,0x85 ,0x50 ,0x90 ,
                0x31 ,0xED ,0x51 ,0x31 ,0x94 ,0xEE ,0x7E ,0x74 ,
                0xD9 ,0xAC ,0x2C ,0x95 ,0x14 ,0x07 ,0x41 ,0xC1 ,
                0x96 ,0x39 ,0x0C ,0xD1 ,0x16 ,0x32 ,0x46 ,0xC6 ,
                0x41 ,0x2F ,0xDC ,0x35 ,0xEC ,0xD3 ,0xB5 ,0x8F },

            
        },
        
    };
    
    
    kdfvector kdf_vector  =
           { "KDF",
            20,
            (uint8_t *)
            "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d\x3e\x3f\x40\x41\x42\x43",
             "label-1",
            "context-1",
               { 0x1A ,0x2E ,0x50 ,0x30 ,0x58 ,0xBC ,0xEC ,0x79 ,
                   0xFE ,0xE9 ,0x77 ,0x71 ,0xE0 ,0xCA ,0xA2 ,0x7C ,
                   0x50 ,0xC0 ,0xC5 ,0x30 },
            
               { 0xFE ,0x92 ,0x7C ,0x8F ,0x9A ,0x09 ,0x94 ,0x9D ,
                   0x49 ,0x55 ,0xFA ,0x5B ,0x35 ,0x46 ,0xD0 ,0x3B ,
                   0x92 ,0xF9 ,0x6D ,0xEC ,0x7A ,0xB3 ,0x50 ,0x03 ,
                   0x84 ,0x03 ,0x8D ,0x21 },
            
               { 0x21 ,0x71 ,0xA7 ,0x58 ,0x44 ,0xD0 ,0x2E ,0xA0 ,
                   0x0B ,0x9D ,0x4E ,0x09 ,0xE4 ,0xFF ,0x1B ,0x35 ,
                   0x47 ,0x2F ,0x0B ,0x58 ,0x5C ,0x49 ,0xE4 ,0x6D ,
                   0xA6 ,0x50 ,0xE0 ,0xE8 ,0xB7 ,0x4C ,0xF8 ,0xDB },
            
               { 0xD7 ,0x1F ,0x5A ,0x15 ,0x72 ,0x86 ,0xC3 ,0x58 ,
                   0xCF ,0x89 ,0x31 ,0x52 ,0xCE ,0x6C ,0x82 ,0xE7 ,
                   0x1E ,0x1E ,0xCF ,0x14 ,0xF8 ,0x70 ,0xEB ,0x26 ,
                   0xAB ,0x16 ,0xAE ,0xAE ,0xDC ,0x51 ,0x46 ,0xFE ,
                   0x8F ,0x14 ,0x42 ,0x66 ,0xA9 ,0x3E ,0x87 ,0xEF ,
                   0xFD ,0x78 ,0xC7 ,0xAF ,0x74 ,0x63 ,0x9B ,0xDC },
            
               { 0x47 ,0x40 ,0xE3 ,0xB3 ,0x02 ,0x02 ,0x01 ,0xED ,
                   0x8D ,0xE8 ,0x47 ,0x76 ,0x9E ,0x41 ,0x65 ,0x3B ,
                   0x4E ,0x59 ,0x51 ,0x61 ,0x97 ,0x05 ,0x45 ,0x70 ,
                   0xB6 ,0x47 ,0xB1 ,0x87 ,0x16 ,0x67 ,0xE4 ,0x61 ,
                   0x9E ,0xE6 ,0xF4 ,0x30 ,0xA7 ,0x9D ,0xE1 ,0x32 ,
                   0xD2 ,0x31 ,0xC1 ,0x8E ,0x01 ,0xB5 ,0x90 ,0xE3 ,
                   0x4F ,0x44 ,0xA3 ,0x64 ,0x24 ,0xDF ,0xCF ,0x2B ,
                   0x4B ,0xEC ,0xD9 ,0x7B ,0x93 ,0xB7 ,0xBB ,0x5B },
            
               {   0x4F ,0xF4 ,0xF2 ,0xC2 ,0x3F ,0xF8 ,0x14 ,0x21 ,
                   0xF2 ,0x53 ,0xA2 ,0xD7 ,0x90 ,0xCD ,0x0E ,0x8E ,
                   0x20 ,0xB6 ,0xD0 ,0xB6 ,0x8B ,0x66 ,0x7D ,0xDF ,
                   0x69 ,0xE8 ,0xED ,0x93 ,0xEA ,0x9E ,0xAD ,0x6C },
               
               {   0xBC ,0x54 ,0xE9 ,0x6A ,0xB8 ,0x5C ,0x4F ,0x73 ,
                   0xFF ,0x96 ,0x9D ,0x12 ,0x21 ,0x1D ,0x5C ,0xDE ,
                   0x0D ,0x20 ,0xA4 ,0x4C ,0xAA ,0x5D ,0x2A ,0xD1 ,
                   0x7B ,0xC4 ,0x7C ,0xE9 ,0xEC ,0x1A ,0xD7 ,0x32 },
               
               { 0x57 ,0xE5 ,0x37 ,0x2F ,0xD6 ,0xD1 ,0xF8 ,0xFE ,
                   0xB6 ,0x7F ,0xED ,0xF8 ,0x50 ,0x57 ,0xF4 ,0x49 ,
                   0xCF ,0x14 ,0x1E ,0x6C ,0x99 ,0x12 ,0x46 ,0x67 ,
                   0x83 ,0x78 ,0x00 ,0xFB ,0x6B ,0xCA ,0xA0 ,0xB4 ,
                   0xC1 ,0x64 ,0x94 ,0xF6 ,0xCA ,0xF4 ,0xBE ,0xE1 ,
                   0xD7 ,0xEF ,0x0E ,0x1A ,0x56 ,0x9E ,0xF7 ,0x46 ,
                   0x1F ,0xE9 ,0xE9 ,0xC7 ,0x05 ,0x28 ,0x3B ,0x57 ,
                   0x77 ,0x90 ,0x9C ,0x87 ,0xAD ,0x9B ,0x4D ,0xFC },
         
           };
    
    
    const HASH_Algorithm hash_algor_list[] =
    {   kHASH_Algorithm_SHA1,
        kHASH_Algorithm_SHA224,
        kHASH_Algorithm_SHA256,
        kHASH_Algorithm_SHA384,
        kHASH_Algorithm_SHA512,
        kHASH_Algorithm_SHA512_256,
        kHASH_Algorithm_SKEIN256,
        kHASH_Algorithm_SKEIN512,
        kHASH_Algorithm_Invalid  };
   
    OPTESTLogInfo("\nTesting HMAC\n");
    
    /* run test array */
    for (i = 0; hash_algor_list[i] != kHASH_Algorithm_Invalid; i++)
    {
        HASH_Algorithm hash = hash_algor_list[i];
        
        OPTESTLogInfo("\t%11s ",  hash_algor_table(hash)  );
        for (j = 0; j < 3; j++)
        {
            katvector* kat = &kat_vector_array[j];
            
            OPTESTLogInfo("%6d", kat->keyLen);
            
            err = TestHMACkat(
                              hash,
                              kat->key,
                              kat->keyLen,
                              kat->msg,
                              strlen( (char *) kat->msg), 
                              kat_for_algorithm(hash, kat)); CKERR;
          }
        
        
        
        OPTESTLogInfo("\tKDF-%s", mac_algor_table(mac_for_algorithm(hash)));
     
        uint32_t expectedLen = hash_algor_bits(hash) >>3;
     
        err = testKDFKat(hash,
                             kdf_vector.key,
                             kdf_vector.keyLen,
                             kdf_vector.label,
                             (uint8_t*)kdf_vector.context, strlen(kdf_vector.context),
                             hash_algor_bits(i),
                              expectedLen,
                             kdf_for_algorithm(hash, &kdf_vector));

        
        OPTESTLogInfo("\n");
    }	
    
    OPTESTLogInfo("\n");
    
    
done:   
    
    return( err );
}

