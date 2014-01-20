/*
 * CProtosharePoocessor.cpp
 *
 *  Created on: 20/12/2013
 *      Author: girino
 *
 * Copyright (c) 2014 Girino Vey.
 *
 * All code in this file is copyrighted to me, Girino Vey, and licensed under Girino's
 * Anarchist License, available at http://girino.org/license and is available on this
 * repository as the file girino_license.txt
 *
 */

#include "CProtoshareProcessor.h"
#include "OpenCLMomentumV3.h"
#include "OpenCLMomentumV4.h"
#include "OpenCLMomentumV5.h"
#include "OpenCLMomentum2.h"
#include "global.h"
#include <assert.h>

volatile uint32 totalCollisionCount = 0;
volatile uint32 totalShareCount = 0;

bool protoshares_revalidateCollision(minerProtosharesBlock_t* block, uint8* midHash, uint32 indexA, uint32 indexB, uint64_t birthdayB)
{
	//if( indexA > MAX_MOMENTUM_NONCE )
	//	printf("indexA out of range\n");
	//if( indexB > MAX_MOMENTUM_NONCE )
	//	printf("indexB out of range\n");
	//if( indexA == indexB )
	//	printf("indexA == indexB");
	sha512_ctx c512;
	uint8 tempHash[32+4];
	uint64 resultHash[8];
	memcpy(tempHash+4, midHash, 32);
	// get birthday B
	if (birthdayB == 0) {
		*(uint32*)tempHash = indexB&~7;
		sha512_init(&c512);
		sha512_update(&c512, tempHash, 32+4);
		sha512_final(&c512, (unsigned char*)resultHash);
		birthdayB = resultHash[indexB&7] >> (64ULL-SEARCH_SPACE_BITS);
	}
	// get birthday A
	*(uint32*)tempHash = indexA&~7;
	sha512_init(&c512);
	sha512_update(&c512, tempHash, 32+4);
	sha512_final(&c512, (unsigned char*)resultHash);
	uint64 birthdayA = 0;
	uint32_t nonceA = indexA&~7;
	for (int i =0; i < BIRTHDAYS_PER_HASH; i++) {
		birthdayA = resultHash[i] >> (64ULL-SEARCH_SPACE_BITS);
		if (birthdayA == birthdayB) {
			indexA = nonceA+i;
			break;
		}
	}
	if( birthdayA != birthdayB )
	{
		return false; // invalid collision
	}
	// birthday collision found
	totalCollisionCount += 2; // we can use every collision twice -> A B and B A
	//printf("Collision found %8d = %8d | num: %d\n", indexA, indexB, totalCollisionCount);
	// get full block hash (for A B)
	block->birthdayA = indexA;
	block->birthdayB = indexB;
	uint8 proofOfWorkHash[32];
	sha256_ctx c256;
	sha256_init(&c256);
	sha256_update(&c256, (unsigned char*)block, 80+8);
	sha256_final(&c256, proofOfWorkHash);
	sha256_init(&c256);
	sha256_update(&c256, (unsigned char*)proofOfWorkHash, 32);
	sha256_final(&c256, proofOfWorkHash);
	bool hashMeetsTarget = true;
	uint32* generatedHash32 = (uint32*)proofOfWorkHash;
	uint32* targetHash32 = (uint32*)block->targetShare;
	for(sint32 hc=7; hc>=0; hc--)
	{
		if( generatedHash32[hc] < targetHash32[hc] )
		{
			hashMeetsTarget = true;
			break;
		}
		else if( generatedHash32[hc] > targetHash32[hc] )
		{
			hashMeetsTarget = false;
			break;
		}
	}
	if( hashMeetsTarget )
	{
		totalShareCount++;
		jhProtominer_submitShare(block);
	}
	// get full block hash (for B A)
	block->birthdayA = indexB;
	block->birthdayB = indexA;
	sha256_init(&c256);
	sha256_update(&c256, (unsigned char*)block, 80+8);
	sha256_final(&c256, proofOfWorkHash);
	sha256_init(&c256);
	sha256_update(&c256, (unsigned char*)proofOfWorkHash, 32);
	sha256_final(&c256, proofOfWorkHash);
	hashMeetsTarget = true;
	generatedHash32 = (uint32*)proofOfWorkHash;
	targetHash32 = (uint32*)block->targetShare;
	for(sint32 hc=7; hc>=0; hc--)
	{
		if( generatedHash32[hc] < targetHash32[hc] )
		{
			hashMeetsTarget = true;
			break;
		}
		else if( generatedHash32[hc] > targetHash32[hc] )
		{
			hashMeetsTarget = false;
			break;
		}
	}
	if( hashMeetsTarget )
	{
		totalShareCount++;
		jhProtominer_submitShare(block);
	}
	return true;
}

CProtoshareProcessorGPU::CProtoshareProcessorGPU(GPUALGO gpu_algorithm, unsigned int _collisionTableBits,
		unsigned int _thread_id, unsigned int _device_num) {
	if (gpu_algorithm == GPUV2) {
		M1 = new OpenCLMomentum2(_collisionTableBits, _device_num);
	} else if (gpu_algorithm == GPUV3) {
		M1 = new OpenCLMomentumV3(_collisionTableBits, _device_num);
	} else if (gpu_algorithm == GPUV4) {
		M1 = new OpenCLMomentumV4(_collisionTableBits, _device_num);
	} else if (gpu_algorithm == GPUV5) {
		M1 = new OpenCLMomentumV5(_collisionTableBits, _device_num);
	} else {
		assert(gpu_algorithm <= 4 && gpu_algorithm >= 2);
	}
	this->collisionTableBits = _collisionTableBits;
	this->thread_id = _thread_id;
	this->device_num = _device_num;
	this->collisions = new collision_struct[M1->getCollisionCeiling()];
}

CProtoshareProcessorGPU::~CProtoshareProcessorGPU() {

	delete M1;
	delete collisions;
}

void CProtoshareProcessorGPU::protoshares_process(minerProtosharesBlock_t* block) {

	size_t count_collisions = 0;

	// generate mid hash using sha256 (header hash)
	uint8_t midHash[32];

	{
		sha256_ctx c256;
		sha256_init(&c256);
		sha256_update(&c256, (unsigned char*)block, 80);
		sha256_final(&c256, midHash);
		sha256_init(&c256);
		sha256_update(&c256, (unsigned char*)midHash, 32);
		sha256_final(&c256, midHash);
	}

	M1->find_collisions(midHash, collisions, &count_collisions);

	if (count_collisions > M1->getCollisionCeiling()) {
		std::cerr
				<< "Warning: found more candidate collisions than storage space available"
				<< std::endl;
		count_collisions = M1->getCollisionCeiling();
	}

	for (int i = 0; i < count_collisions; i++) {
		protoshares_revalidateCollision(block, midHash, collisions[i].nonce_a,
				collisions[i].nonce_b, collisions[i].birthday);
	}
	//printf("DEBUG: collisions = %d\n", count_collisions);

}
