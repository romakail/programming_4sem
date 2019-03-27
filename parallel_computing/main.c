#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <sched.h>

#define PRINT(args...) 		    \
	do 							\
	{							\
		printf(args);           \
		fflush (stdout);        \
	}while (0);


#define CHECK(what, message)										\
	do 																\
	{																\
		if (what == -1)												\
		{															\
			printf ("Error, line = %d", __LINE__);					\
			perror (message);										\
			exit(-1);												\
		}															\
	}while (0);

const int SUCCESS_RET =  0;
const int FAIL_RET    = -1;

const double START_VAL  = 1.0;
const double FINISH_VAL = 10.0;
const double STEP       = 5E-9;

struct threadInfo_t
{
	double startValue;
	double finishValue;
	double step;
	double integralPart;
};

int     parseNprocesses (int argc, char** argv);
unsigned int nMachineProcs ();

int calcIntegralFewThreads  (int nRealProcs, int nThreads, double* integral);
int calcIntegralManyThreads (                int nThreads, double* integral);

void*   computeIntegral (void* arg);
void*   emptyRoutine    (void* arg);

int main (int argc, char** argv)
{
	int nThreads = parseNprocesses (argc, argv);
	int nProcs   = nMachineProcs   ();
	if ((nProcs <= 0) || (nProcs % 2 == 1))
	{
		printf ("nProcs = %d, and it is very weird\n", nProcs);
		exit (FAIL_RET);
	}

	double integral = 0.0;
	if ((nThreads > 0) && (nThreads <= nProcs / 2))
	{
		calcIntegralFewThreads  (nProcs/2, nThreads, &integral);
	}
	else if (nThreads > nProcs / 2)
	{
		calcIntegralManyThreads (        nThreads, &integral);
	}

	printf ("result = %lg\n", integral);
	return 0;
}

//------------------------------------------------------------------------------

int calcIntegralFewThreads  (int nRealProcs, int nThreads, double* integral)
{
	// initializing threads
	pthread_t* threads = (pthread_t*) calloc (nRealProcs, sizeof (*threads));
	if (threads == 0)
	{
		perror ("Calloc has broken\n");
		exit (FAIL_RET);
	}

	// initializing information for threads
	struct threadInfo_t* threadsInformation = (struct threadInfo_t*) calloc (nThreads, sizeof(*threadsInformation));
	if (threadsInformation == 0)
	{
		perror ("Calloc has broken\n");
		exit (FAIL_RET);
	}
	for (int i = 0; i < nThreads; i++)
	{
		threadsInformation[i].startValue   = START_VAL + (FINISH_VAL - START_VAL) / nThreads * i;
		threadsInformation[i].finishValue  = START_VAL + (FINISH_VAL - START_VAL) / nThreads * (i + 1);
		threadsInformation[i].step         = STEP;
		threadsInformation[i].integralPart = 0.0;
	}

	// initializing thread attributes
	pthread_attr_t threadAttributes;
	int initRet = pthread_attr_init (&threadAttributes);
	if (initRet != 0)
	{
		perror("Problem with pthread_attr_init\n");
		exit (FAIL_RET);
	}

	cpu_set_t cpu_set;
	CPU_ZERO (&cpu_set);

	for (int i = 0; i < nRealProcs; i++)
	{
		CPU_SET (i, &cpu_set);
	}
	int setaffinity_ret = pthread_attr_setaffinity_np(&threadAttributes, sizeof(pthread_attr_t), &cpu_set);
	if (setaffinity_ret != 0)
	{
		perror ("some error with set_affinity occured");
		exit (FAIL_RET);
	}

	// computing
	for (int i = 0; i < nRealProcs; i++)
	{
		// printf ("i = %d\n", i);
		if (i < nThreads)
		{
			pthread_create (&threads[i], &threadAttributes, computeIntegral, &threadsInformation[i]);
		}
		else
		{
			pthread_create (&threads[i], &threadAttributes, emptyRoutine, NULL);
		}
	}

	for (int i = 0; i < nThreads; i++)
	{
		pthread_join (threads[i], 0);
		*integral += threadsInformation[i].integralPart;
		printf ("[%d] %lg\n", i, threadsInformation[i].integralPart);
	}

	free (threads);
	free (threadsInformation);

	return 0;
}

//------------------------------------------------------------------------------

int calcIntegralManyThreads (int nThreads, double* integral)
{
	// initializing threads
	pthread_t* threads = (pthread_t*) calloc (nThreads, sizeof (*threads));
	if (threads == 0)
	{
		perror ("Calloc has broken\n");
		exit (FAIL_RET);
	}

	// initializing information for thrads
	struct threadInfo_t* threadsInformation = (struct threadInfo_t*) calloc (nThreads, sizeof(*threadsInformation));
	if (threadsInformation == 0)
	{
		perror ("Calloc has broken\n");
		exit (FAIL_RET);
	}
	for (int i = 0; i < nThreads; i++)
	{
		threadsInformation[i].startValue   = START_VAL + (FINISH_VAL - START_VAL) / nThreads * i;
		threadsInformation[i].finishValue  = START_VAL + (FINISH_VAL - START_VAL) / nThreads * (i + 1);
		threadsInformation[i].step         = STEP;
		threadsInformation[i].integralPart = 0.0;
	}

	// initializing trhead attributes
	pthread_attr_t threadAttributes;
	int initRet = pthread_attr_init (&threadAttributes);
	if (initRet != 0)
	{
		perror("Problem with pthread_attr_init\n");
		exit (FAIL_RET);
	}

	// computing
	for (int i = 0; i < nThreads; i++)
	{
		// printf ("i = %d\n", i);
		pthread_create (&threads[i], &threadAttributes, computeIntegral, &threadsInformation[i]);
	}

	for (int i = 0; i < nThreads; i++)
	{
		pthread_join (threads[i], 0);
		*integral += threadsInformation[i].integralPart;
		printf ("[%d] %lg\n", i, threadsInformation[i].integralPart);
	}

	free (threads);
	free (threadsInformation);

	return 0;
}

//------------------------------------------------------------------------------

unsigned int nMachineProcs ()
{
	return 8;
}

//------------------------------------------------------------------------------

void* emptyRoutine (void* arg)
{
	// printf ("I am empty\n");
	while (1);
	pthread_exit (0);
}

//------------------------------------------------------------------------------

void* computeIntegral (void* arg)
{
	// printf ("I am working\n");
	struct threadInfo_t* thInfo = (struct threadInfo_t*) arg;
	double curPosition = thInfo->startValue;

	while (curPosition < thInfo->finishValue)
	{
		thInfo->integralPart += (curPosition) / (1 + curPosition) * thInfo->step;
		curPosition += thInfo->step;
	}

	pthread_exit (0);
}

//------------------------------------------------------------------------------

int parseNprocesses (int argc, char** argv)
{
	if (argc != 2)
	{
		printf ("wrong number of arguments\n");
		exit (-1);
	}

	char* endptr = 0;
	int val = strtol (argv[1], &endptr, 10);

	if (errno == ERANGE)
	{
		perror ("overflow of long int\n");
		exit (FAIL_RET);
	}
	else if (errno == EINVAL)
	{
		perror ("no digits seen\n");
		exit (FAIL_RET);
	}
	else if (val <= 0)
	{
		printf ("less or equal to zero\n");
		exit (FAIL_RET);
	}
	else if (*endptr != '\0')
	{
		printf ("I found not a digit\n");
		exit (FAIL_RET);
	}

	return val;
}

















//
