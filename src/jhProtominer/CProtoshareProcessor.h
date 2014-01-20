/*
 * CProtosharePoocessor.h
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

#ifndef CPROTOSHAREPOOCESSOR_H_
#define CPROTOSHAREPOOCESSOR_H_
#include "AbstractMomentum.h"
#include "global.h"

class CProtoshareProcessorGPU {
public:
	CProtoshareProcessorGPU(GPUALGO gpu_ver,
			unsigned int _collisionTableBits,
			unsigned int _thread_id,
			unsigned int _device_num);
	virtual ~CProtoshareProcessorGPU();
	virtual void protoshares_process(minerProtosharesBlock_t* block);
private:
	unsigned int collisionTableBits;
	unsigned int thread_id;
	unsigned int device_num;
	AbstractMomentum * M1;
	collision_struct * collisions;
};

#endif /* CPROTOSHAREPOOCESSOR_H_ */
