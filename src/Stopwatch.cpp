#include "uRobotics/utilities/Stopwatch.h"

using namespace Robotics::Utilities;

Stopwatch::~Stopwatch(void){}

#ifdef _WIN32
/************************************************************************/
/* Windows                                                              */
/************************************************************************/

#include <Windows.h>
long long get_wall_time(){
	LARGE_INTEGER time, freq;
	if (!QueryPerformanceFrequency(&freq)){
		//  Handle error
		return 0;
	}
	if (!QueryPerformanceCounter(&time)){
		//  Handle error
		return 0;
	}
	return 1000000 * time.QuadPart / freq.QuadPart;
}

#else
/************************************************************************/
/* UNIX                                                                 */
/************************************************************************/
#include <cstddef>
#include <sys/time.h>
long long get_wall_time(){
	struct timeval time;
	if (gettimeofday(&time, NULL)){
		//  Handle error
		return 0;
	}
	return time.tv_sec * 1000000 + time.tv_usec;
}
#endif

Stopwatch::Stopwatch(void) :
running(false), startTime(0), currentTime(0){
}

void Stopwatch::start(){
	if(running) return;
	startTime = get_wall_time();
	running = true;
}

void Stopwatch::reset(){
	startTime = get_wall_time();
	currentTime = startTime;
}

void Stopwatch::stop(){
	if(!running) return;
	currentTime = get_wall_time();
	running = false;
}

long Stopwatch::elapsedMilliseconds(){
	return (long)(elapsedMicroseconds() / 1000);
}

long long Stopwatch::elapsedMicroseconds(){
	if(running)
		return (currentTime = get_wall_time()) - startTime;
	return currentTime - startTime;
}
