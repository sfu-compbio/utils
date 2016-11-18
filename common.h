
#ifndef __COMMON__
#define __COMMON__

#include <stdlib.h>
#include <string.h>
#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <deque>
#include <vector>
#include <set>
#include <list>
#include <algorithm>
#include <math.h>
#include <stdint.h>
#include <limits>
#include <zlib.h>

using namespace std;

void 				initCommon();
string 				str2Lower(string str);
string 				str2Upper(string str);
void 				revString(string &str, string &revStr);
void 				revComplement(string &str, string &revStr);
void 				revString(string &revStr, char *str);
int 				parseCommandLine (int argc, char *argv[]);
void 				revComplement(string &revStr, char *str);
void 				printHelp();

template <typename T>
T str2type(string str)
{
	T n;
	istringstream sin(str);
	sin >> n;
	return n;
}

template <typename T>
string type2str(T v)
{
	ostringstream sout;
	sout << v;
	return sout.str();
}

#endif //__COMMON__