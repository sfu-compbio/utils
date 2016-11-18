#include "revComp.h"
#include "common.h"
#include "kseq.h"
#include <iostream>
#include <fstream>
#include <getopt.h>
#include <zlib.h>

using namespace std;

string 		_revComp_IN_FILE;
string 		_revComp_OUT_FILE;
bool 		_revComp_isFastq = false;
ofstream 	_revComp_fout;
ostream 	*_revComp_pout;

int parseCommandLine_revComp(int argc, char *argv[]);
void printHelp_revComp();

KSEQ_INIT(gzFile, gzread)

int program_revComp(int argc, char* argv[])
{
	if(argc < 3)
	{
		printHelp_revComp();
		return 1;
	}
	else if(!parseCommandLine_revComp(argc, argv))
	{
		return 1;
	}
	ostream &outObj = *_revComp_pout;

	gzFile readFile;
    kseq_t *readSeq;
	readFile = gzopen(_revComp_IN_FILE.c_str(), "r");
	if(readFile==NULL)
	{
		cerr << "Cannot open file: " << _revComp_IN_FILE << endl;
		exit(1);
	}

	string readSeq_rev;
	string readQual_rev;
	
    readSeq = kseq_init(readFile);
	while (kseq_read(readSeq) >= 0)
	{
		if(_revComp_isFastq && readSeq->qual.l>0)
		{
			outObj<< "@" << readSeq->name.s << "\n";
			revComplement(readSeq_rev, readSeq->seq.s);
			outObj<< readSeq_rev << "\n";
			outObj<< "+" << "\n";
			revString(readQual_rev, readSeq->qual.s);
			outObj<< readQual_rev << "\n";
		}
		else
		{
			outObj<< ">" << readSeq->name.s << "\n";
			revComplement(readSeq_rev, readSeq->seq.s);
			outObj<< readSeq_rev << "\n";
		}
	}
    kseq_destroy(readSeq);
    gzclose(readFile);

	if(_revComp_OUT_FILE != "")
	{
		_revComp_fout.close();
	}
	return 0;
}

int parseCommandLine_revComp(int argc, char *argv[])
{
	int index, c;

	static struct option longOptions[] =
	{
		{"file", 		required_argument, 		0, 		'f'	},
		{"out", 		required_argument, 		0, 		'o'	},
		{"fastq", 		no_argument, 			0, 		'q'	},
		{"help", 		no_argument, 			0, 		'h'	},
		{"version", 	no_argument, 			0, 		'v'	},
		{0,0,0,0}
	};

	while ( (c = getopt_long ( argc, argv, "f:o:qhv", longOptions, &index))!= -1 )
	{
		switch (c)
		{
			case 'f':
				_revComp_IN_FILE = optarg;
				break;
			case 'o':
				_revComp_OUT_FILE = optarg;
				break;
			case 'q':
				_revComp_isFastq = true;
				break;
			case 'h':
				printHelp_revComp();
				return 0;
				break;
			case 'v':
				cout << "Version: " << FASTUTILS_VERSION << endl;
				cout << "Build Date: " << BUILD_DATE << endl;
				return 0;
				break;
			default:
				cerr << "[ERROR] run \"fastUtils shuffle -h\" to get a list of acceptable arguments." << endl;
				cerr << endl;
				return 0;
		}
	}

	if(_revComp_IN_FILE == "")
	{
		cerr << endl;
		cerr<< "[ERROR] option -f is required" << endl;
		cerr << endl;
		return 0;
	}

	if(_revComp_OUT_FILE == "")
	{
		_revComp_pout = &cout;
	}
	else
	{
		_revComp_fout.open(_revComp_OUT_FILE.c_str());
		if(_revComp_fout.is_open()==false)
		{
			cerr<< "[ERROR] could not open file: " << _revComp_OUT_FILE << endl;
			return 0;
		}
		_revComp_pout = &_revComp_fout;
	}

	return 1;
}

void printHelp_revComp()
{
	cerr << endl;
	cerr << "USAGE: fastUtils toLower -f in.fq [options]" << endl;
	cerr << endl;
	cerr << "Required options:" << endl;
	cerr << "         -f STR        fasta/q file containing the reads" << endl;
	cerr << "More options:" << endl;
	cerr << "         -o STR        output reads in STR file [stdout]" << endl;
	cerr << "         -q            output reads in fastq format if possible" << endl;
	cerr << "         -v            print version" << endl;
	cerr << "         -h            print this help" << endl;
	cerr << endl;
}