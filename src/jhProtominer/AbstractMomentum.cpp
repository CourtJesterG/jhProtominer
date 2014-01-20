/*
 * AbstractMomentum.cpp
 *
 *  Created on: 02/01/2014
 *      Author: girino
 *
 * Copyright (c) 2014 Girino Vey.
 *
 * All code in this file is copyrighted to me, Girino Vey, and licensed under Girino's
 * Anarchist License, available at http://girino.org/license and is available on this
 * repository as the file girino_license.txt
 *
 */

#include "AbstractMomentum.h"
#include "global.h"

AbstractMomentum::AbstractMomentum() {
	// TODO Auto-generated constructor stub

}

AbstractMomentum::~AbstractMomentum() {
	// TODO Auto-generated destructor stub
}


int log2(size_t value) {
	int ret = 0;
	while (value > 1) {
		ret++;
		value = value>>1;
	}
	return ret;
}

