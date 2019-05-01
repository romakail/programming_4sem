#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <sched.h>
#include <sys/sysinfo.h>

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
const double STEP       = 1E-9;

struct threadInfo_t
{
	double startValue;
	double finishValue;
	double step;
	double integralPart;
};

int     parseNprocesses    (int argc, char** argv);
unsigned int nMachineProcs (int** realProcs);

int calcIntegralFewThreads  (int nThreads, double* integral, int nRealProcs, int* realProcs);
int calcIntegralManyThreads (int nThreads, double* integral);

void*   computeIntegral (void* arg);
void*   emptyRoutine    (void* arg);

int main (int argc, char** argv)
{
	int nThreads = parseNprocesses (argc, argv);

	int* realProcs = NULL;
	unsigned int nProcs = nMachineProcs(&realProcs);

	if ((nProcs <= 0) || (nProcs % 2 == 1))
	{
		printf ("nProcs = %d, and it is very weird\n", nProcs);
		exit (FAIL_RET);
	}

	double integral = 0.0;
	if ((nThreads > 0) && (nThreads <= nProcs / 2))
	{
		calcIntegralFewThreads  (nThreads, &integral, nProcs, realProcs);
	}
	else if (nThreads > nProcs / 2)
	{
		calcIntegralManyThreads (nThreads, &integral);
	}

	printf ("result = %lg\n", integral);

	free (realProcs);

	return 0;
}

//------------------------------------------------------------------------------

int calcIntegralFewThreads  (int nThreads, double* integral, int nRealProcs, int* realProcs)
{
	int i = 0;
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
	for (i = 0; i < nThreads; i++)
	{
		threadsInformation[i].startValue   = START_VAL + (FINISH_VAL - START_VAL) / nThreads * i;
		threadsInformation[i].finishValue  = START_VAL + (FINISH_VAL - START_VAL) / nThreads * (i + 1);
		threadsInformation[i].step         = STEP;
		threadsInformation[i].integralPart = 0.0;
	}

	// initializing thread attributes
	// pthread_attr_t threadAttributes;
	// int initRet = pthread_attr_init (&threadAttributes);
	pthread_attr_t* threadAttributes = (pthread_attr_t*) calloc (nRealProcs, sizeof (pthread_attr_t));


	cpu_set_t cpu_set;
	int initRet = 0;
	int setaffinity_ret = 0;

	for (i = 0; i < nRealProcs; i++)
	{
		initRet = pthread_attr_init (&threadAttributes[i]);
		if (initRet != 0)
		{
			perror ("Problem with pthread_attr_init\n");
			exit (FAIL_RET);
		}

		CPU_ZERO (&cpu_set);
		CPU_SET  (realProcs[i], &cpu_set);

		setaffinity_ret = pthread_attr_setaffinity_np(&threadAttributes[i], sizeof(cpu_set_t), &cpu_set);
		if (setaffinity_ret != 0)
		{
			perror ("some error with set_affinity occured");
			exit (FAIL_RET);
		}
	}

	//
	// for (int i = 0; i < nRealProcs; i++)
	// {
	// 	CPU_SET (i, &cpu_set);
	// }

	// computing
	for (i = 0; i < nRealProcs; i++)
	{
		if (i < nThreads)
		{
			// printf ("1");
			pthread_create (&threads[i], &threadAttributes[i], computeIntegral, &threadsInformation[i]);
		}
		else
		{
			// printf ("2");
			pthread_create (&threads[i], &threadAttributes[i], emptyRoutine, NULL);
		}
	}

	printf ("\n");
	for (i = 0; i < nThreads; i++)
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
	int i = 0;
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
	for (i = 0; i < nThreads; i++)
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
	for (i = 0; i < nThreads; i++)
	{
		// printf ("i = %d\n", i);
		pthread_create (&threads[i], &threadAttributes, computeIntegral, &threadsInformation[i]);
	}

	for (i = 0; i < nThreads; i++)
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

unsigned int nMachineProcs (int** realProcs)
{
	int i = 0;
	unsigned int nProcs = get_nprocs();
	*realProcs = (int*) malloc (nProcs * sizeof(**realProcs));
	if (*realProcs == NULL)
	{
		perror ("problem with malloc\n");
	}
	for (i = 0; i < nProcs; i++)
	{
		(*realProcs)[i] = -1;
	}

	unsigned int nRealProcs = 0;
	int procId = 0;
	i = 0;
	FILE* cpuIdFile = NULL;
	char fileLocation [] = "/sys/bus/cpu/devices/cpu0/topology/core_id";

	while ((cpuIdFile = fopen(fileLocation, "r")) != NULL)
	{
		fscanf (cpuIdFile, "%d", &procId);
		i = 0;
		while (((*realProcs)[i] != procId) && ((*realProcs)[i] != -1))
			i++;
		if ((*realProcs)[i] == -1)
		{
			nRealProcs++;
			(*realProcs)[i] = procId;
		}

		fileLocation[24]++;
	}
	return nRealProcs;
}

//------------------------------------------------------------------------------

void* emptyRoutine (void* arg)
{
	// printf ("I am empty\n");
	while (1);
	pthread_exit (0);
}

//-----------------------------------------------------------------------------

#pragma GCC optimize ("03")

void* computeIntegral (void* arg)
{
	// printf ("I am working\n");
	struct threadInfo_t* thInfo = (struct threadInfo_t*) arg;
	double curPosition = thInfo->startValue;

	while (curPosition < thInfo->finishValue)
	{
		thInfo->integralPart += curPosition / (1 + curPosition) * thInfo->step;
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
