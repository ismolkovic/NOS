#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <iostream>
#include <signal.h>
#include <time.h>

using namespace std;

struct msgBuffer {
	long mtype;
	char mtext[300];
};

#define PROCESS_A 2
#define PROCESS_KDC 1
#define PROCESS_A_TO_B 4
#define PROCESS_B 8

