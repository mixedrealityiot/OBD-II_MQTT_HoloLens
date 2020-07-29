#include <sys/time.h>
#include <stdio.h> 
#include "common.h"
#include "common.h"

unsigned long millis()
{
	struct timeval te;
	gettimeofday(&te, NULL);
	long long milliseconds = te.tv_sec * 1000LL + te.tv_usec / 1000;
	return milliseconds;
}

void waitMs(unsigned long t) {
	unsigned long start = millis();
	while (millis() - start < t) {
		__asm("nop");
	}
}