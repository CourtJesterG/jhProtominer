#ifndef __jhprotominer_global_h__
#define __jhprotominer_global_h__

#if defined(__WIN32__) || defined(__CYGWIN__)
#pragma comment(lib,"Ws2_32.lib")
#include<Winsock2.h>
#include<ws2tcpip.h>
#elif __CYGWIN__
#include"win.h" // port from windows
#else
#include"win.h" // port from windows
#endif

#define SWAP64(n) \
            (((n) << 56)                      \
          | (((n) & 0xff00) << 40)            \
          | (((n) & 0xff0000) << 24)          \
          | (((n) & 0xff000000) << 8)         \
          | (((n) >> 8) & 0xff000000)         \
          | (((n) >> 24) & 0xff0000)          \
          | (((n) >> 40) & 0xff00)            \
          | ((n) >> 56))

#define max(a, b) \
    ((a)>(b)?(a):(b))

#define min(a, b) \
    ((a)<(b)?(a):(b))

#include<stdio.h>
#include<time.h>
#include<stdlib.h>

#include"jhlib.h" // slim version of jh library


// connection info for xpt
typedef struct  
{
	char* ip;
	uint16 port;
	char* authUser;
	char* authPass;
}generalRequestTarget_t;

#include"xptServer.h"
#include"xptClient.h"

#include"sha2.h"

#include"transaction.h"

// global settings for miner
typedef struct  
{
	generalRequestTarget_t requestTarget;
	uint32 protoshareMemoryMode;
}minerSettings_t;

extern minerSettings_t minerSettings;

#define PROTOSHARE_MEM_4096		(30)
#define PROTOSHARE_MEM_2048		(29)
#define PROTOSHARE_MEM_1024		(28)
#define PROTOSHARE_MEM_512		(27)
#define PROTOSHARE_MEM_256		(26)
#define PROTOSHARE_MEM_128		(25)
#define PROTOSHARE_MEM_64		(24)
#define PROTOSHARE_MEM_32		(23)
#define PROTOSHARE_MEM_16		(22)
#define PROTOSHARE_MEM_8		(21)

// block data struct

typedef struct  
{
	// block header data (relevant for midhash)
	uint32	version;
	uint8	prevBlockHash[32];
	uint8	merkleRoot[32];
	uint32	nTime;
	uint32	nBits;
	uint32	nonce;
	// birthday collision
	uint32	birthdayA;
	uint32	birthdayB;
	uint32	uniqueMerkleSeed;

	uint32	height;
	uint8	merkleRootOriginal[32]; // used to identify work
	uint8	target[32];
	uint8	targetShare[32];
}minerProtosharesBlock_t;

void jhProtominer_submitShare(minerProtosharesBlock_t* block);

// stats
extern volatile uint32 totalCollisionCount;
extern volatile uint32 totalShareCount;

extern volatile uint32 monitorCurrentBlockHeight;

#endif //__jhprotominer_global_h__
