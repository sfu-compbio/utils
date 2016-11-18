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
				cnt[ seq->seq.s[j] ]++;
			}
		}
		kseq_destroy(seq);
		gzclose(fp);
	}
	cout<< "# of reads: " << num << endl;
	cout<< "# of bases: " << sLen << endl;
	cout<< fixed << setprecision(2);
	cout<< "# of n/N's: " << cnt['n']+cnt['N'] << " (" << (double)(cnt['n']+cnt['N'])/sLen*100 << "%)" << endl;
	cout<< "       n's: " << cnt['n'] << " (" << (double)cnt['n']/sLen*100 << "%)" << endl;
	cout<< "       N's: " << cnt['N'] << " (" << (double)cnt['N']/sLen*100 << "%)" << endl;
	cout<< "# of a/A's: " << cnt['a']+cnt['A'] << " (" << (double)(cnt['a']+cnt['A'])/sLen*100 << "%)" << endl;
	cout<< "       a's: " << cnt['a'] << " (" << (double)cnt['a']/sLen*100 << "%)" << endl;
	cout<< "       A's: " << cnt['A'] << " (" << (double)cnt['A']/sLen*100 << "%)" << endl;
	cout<< "# of c/C's: " << cnt['c']+cnt['C'] << " (" << (double)(cnt['c']+cnt['C'])/sLen*100 << "%)" << endl;
	cout<< "       c's: " << cnt['c'] << " (" << (double)cnt['c']/sLen*100 << "%)" << endl;
	cout<< "       C's: " << cnt['C'] << " (" << (double)cnt['C']/sLen*100 << "%)" << endl;
	cout<< "# of g/G's: " << cnt['g']+cnt['G'] << " (" << (double)(cnt['g']+cnt['G'])/sLen*100 << "%)" << endl;
	cout<< "       g's: " << cnt['g'] << " (" << (double)cnt['g']/sLen*100 << "%)" << endl;
	cout<< "       G's: " << cnt['G'] << " (" << (double)cnt['G']/sLen*100 << "%)" << endl;
	cout<< "# of t/T's: " << cnt['t']+cnt['T'] << " (" << (double)(cnt['t']+cnt['T'])/sLen*100 << "%)" << endl;
	cout<< "       t's: " << cnt['t'] << " (" << (double)cnt['t']/sLen*100 << "%)" << endl;
	cout<< "       T's: " << cnt['T'] << " (" << (double)cnt['T']/sLen*100 << "%)" << endl;

	return 0;
}

void printHelp_getStat()
{
	cerr << endl;
	cerr << "USAGE: fastUtils getStat a.fq [b.fq ...]" << endl;
	cerr << endl;
}