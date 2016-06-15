#include "toLower.h"
#include "common.h"
#include "kseq.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <getopt.h>
#include <zlib.h>

using namespace std;

int parseCommandLine_getStat(int argc, char *argv[]);
void printHelp_getStat();

KSEQ_INIT(gzFile, gzread)

int program_getStat(int argc, char* argv[])
{
	if(argc < 3)
	{
		printHelp_getStat();
		return 1;
	}

	long long cnt[256]={0};

	gzFile fp;
	kseq_t *seq;
	long long num = 0;
	long long sLen = 0;
	for(int i=2; i<argc; i++)
	{
		fp = gzopen(argv[i], "r");
		if(fp==NULL)
		{
			cerr << "Cannot open file: " << argv[i] << endl;
			exit(1);
		}
		seq = kseq_init(fp);
		while (kseq_read(seq) >= 0)
		{
			num++;
			sLen += seq->seq.l;
			for(int j=0; j<seq->seq.l; j++)
			{
				cnt[toupper(seq->seq.s[j])]++;
			}
		}
		kseq_destroy(seq);
		gzclose(fp);
	}
	cout<< "   number of reads: " << num << endl;
	cout<< "total num of bases: " << sLen << endl;
	cout<< fixed << setprecision(2);
	cout<< "  total num of N's: " << cnt['N'] << " (" << (double)cnt['N']/sLen*100 << "%)" << endl;
	cout<< "  total num of A's: " << cnt['A'] << " (" << (double)cnt['A']/sLen*100 << "%)" << endl;
	cout<< "  total num of C's: " << cnt['C'] << " (" << (double)cnt['C']/sLen*100 << "%)" << endl;
	cout<< "  total num of G's: " << cnt['G'] << " (" << (double)cnt['G']/sLen*100 << "%)" << endl;
	cout<< "  total num of T's: " << cnt['T'] << " (" << (double)cnt['T']/sLen*100 << "%)" << endl;

	return 0;
}

void printHelp_getStat()
{
	cerr << endl;
	cerr << "USAGE: fastUtils getStat a.fq [b.fq ...]" << endl;
	cerr << endl;
}