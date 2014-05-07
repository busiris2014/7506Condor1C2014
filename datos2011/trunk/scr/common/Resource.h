#ifndef RESOURCE_H_
#define RESOURCE_H_

#include "Properties.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>

using namespace std;

class Resource {
	private:
		static Properties configProperties;

	public:
		Resource();
		virtual ~Resource();

		static Properties* getConfigProperties();
};

#endif /* RESOURCE_H_ */
