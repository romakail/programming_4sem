#ifndef integral_h
#define integral_h

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <sched.h>
#include <sys/sysinfo.h>

struct threadInfo_t
{
	double startValue;
	double finishValue;
	double step;
	double integralPart;
};

double integral (int nThreads, double startVal, double endVal, double step);

int     parseNprocesses    (int argc, char** argv);
unsigned int nMachineProcs (int** realProcs);

int calcIntegralFewThreads  (int nThreads, double* integral, int nRealProcs, int* realProcs, struct threadInfo_t* info);
int calcIntegralManyThreads (int nThreads, double* integral                                , struct threadInfo_t* info);

void*   computeIntegral (void* arg);
void*   emptyRoutine    (void* arg);

#endif /* integral_h */
