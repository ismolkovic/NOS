#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
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
#include <fstream>
#include <sstream>
#include <string>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include <vector>
#include <sys/wait.h>

using namespace std;

#define MAX_MSG 50

string int_2_string(int number) {
  stringstream ss;
  ss << number;
  return ss.str(); 
}

typedef struct _msgBuf {
	int mtype;
	char mtext[200];
} msgBuffer;
