#include "shuffle.h"
#include "toLower.h"
#include "split.h"
#include "getStat.h"
#include "common.h"

using namespace std;

void printHelp_main();

int main(int argc, char* argv[])
{
	if(argc < 2)
	{
		printHelp_main();
	}
	else
	{
		initCommon();
		string com = argv[1];
		if(com == "shuffle")
		{
			return program_shuffle(argc, argv);
		}
		else if(com == "toLower")
		{
			return program_toLower(argc, argv);
		}
		else if(com == "split")
		{
			return program_split(argc, argv);
		}
		else if(com == "getStat")
		{
			return program_getStat(argc, argv);
		}
		else
		{
			printHelp_main();
		}
	}
	return 0;
}

void printHelp_main()
{
	cerr<< endl;
	cerr<< "Usage: fastUtils <command> [options]" << endl;
	cerr<< endl;
	cerr<< "command: getStat      prints general statistics of fasta/q files" << endl;
	cerr<< "         shuffle      interleave/interlace fasta/q files" << endl;
	cerr<< "         toLower      convert fasta/q reads to lowercase" << endl;
	cerr<< "         split        split a fasta/q file into smaller files" << endl;
	cerr<< endl;
}
