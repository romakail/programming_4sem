#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>

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

int     parseNprocesses (int argc, char** argv);
void*   computeIntegral (void* arg);

int main (int argc, char** argv)
{
	int nCores = parseNprocesses (argc, argv);

	// initializing some stuff
	pthread_t* threads = (pthread_t*) calloc (nCores, sizeof (*threads));
	if (threads == 0)
	{
		perror ("Calloc has broken\n");
		exit (FAIL_RET);
	}

	struct threadInfo_t* threadsInformation = (struct threadInfo_t*) calloc (nCores, sizeof(*threadsInformation));
	if (threads == 0)
	{
		perror ("Calloc has broken\n");
		exit (FAIL_RET);
	}

	pthread_attr_t threadAttributes;
	int initRet = pthread_attr_init (&threadAttributes);
	if (initRet != 0)
	{
		perror("Problem with pthread_attr_init\n");
		exit (FAIL_RET);
	}

	for (int i = 0; i < nCores; i++)
	{
		threadsInformation[i].startValue   = START_VAL + (FINISH_VAL - START_VAL) / nCores * i;
		threadsInformation[i].finishValue  = START_VAL + (FINISH_VAL - START_VAL) / nCores * (i + 1);
		threadsInformation[i].step         = STEP;
		threadsInformation[i].integralPart = 0.0;
	}

	for (int i = 0; i < nCores; i++)
	{
		pthread_create (&threads[i], &threadAttributes, computeIntegral, &threadsInformation[i]);
	}

	double result = 0.0;
	for (int i = 0; i < nCores; i++)
	{
		pthread_join (threads[i], 0);
		result += threadsInformation[i].integralPart;
		printf ("[%d] %lg\n", i, threadsInformation[i].integralPart);
	}

	printf ("result = %lg\n", result);






	return 0;
}






//------------------------------------------------------------------------------s

void* computeIntegral (void* arg)
{
	struct threadInfo_t* thInfo = (struct threadInfo_t*) arg;
	double curPosition = thInfo->startValue;

	while (curPosition < thInfo->finishValue)
	{
		thInfo->integralPart += (curPosition) / (1 + curPosition) * thInfo->step;
		curPosition += thInfo->step;
	}

	// thInfo->integralPart *= thInfo->step;

	// int a = 0;
	// while (1)
	// {
	// 	a++;
	// 	printf ("%d", a);
	// }

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
		exit (0);
	}
	else if (errno == EINVAL)
	{
		perror ("no digits seen\n");
		exit (0);
	}
	else if (val <= 0)
	{
		perror ("less then zero");
		exit (0);
	}
	else if (*endptr != '\0')
	{
		perror ("I found not a digit\n");
		exit (0);
	}

	return val;
}









//
