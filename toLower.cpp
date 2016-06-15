#include "toLower.h"
#include "common.h"
#include "kseq.h"
#include <iostream>
#include <fstream>
#include <getopt.h>
#include <zlib.h>

using namespace std;

string 		_toLower_IN_FILE;
string 		_toLower_OUT_FILE;
bool 		_toLower_isFastq = false;
ofstream 	_toLower_fout;
ostream 	*_toLower_pout;

int parseCommandLine_toLower(int argc, char *argv[]);
void printHelp_toLower();

KSEQ_INIT(gzFile, gzread)

int program_toLower(int argc, char* argv[])
{
	if(argc < 3)
	{
		printHelp_toLower();
		return 1;
	}
	else if(!parseCommandLine_toLower(argc, argv))
	{
		return 1;
	}
	ostream &outObj = *_toLower_pout;

	gzFile readFile;
    kseq_t *readSeq;
	readFile = gzopen(_toLower_IN_FILE.c_str(), "r");
	if(readFile==NULL)
	{
		cerr << "Cannot open file: " << _toLower_IN_FILE << endl;
		exit(1);
	}
	
    readSeq = kseq_init(readFile);
	while (kseq_read(readSeq) >= 0)
	{
		if(_toLower_isFastq && readSeq->qual.l>0)
		{
			outObj<< "@" << readSeq->name.s << "\n";
			outObj<< str2Lower(readSeq->seq.s) << "\n";
			outObj<< "+" << "\n";
			outObj<< readSeq->qual.s << "\n";
		}
		else
		{
			outObj<< ">" << readSeq->name.s << "\n";
			outObj<< str2Lower(readSeq->seq.s) << "\n";
		}
	}
    kseq_destroy(readSeq);
    gzclose(readFile);

	if(_toLower_OUT_FILE != "")
	{
		_toLower_fout.close();
	}
	return 0;
}

int parseCommandLine_toLower(int argc, char *argv[])
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
				_toLower_IN_FILE = optarg;
				break;
			case 'o':
				_toLower_OUT_FILE = optarg;
				break;
			case 'q':
				_toLower_isFastq = true;
				break;
			case 'h':
				printHelp_toLower();
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

	if(_toLower_IN_FILE == "")
	{
		cerr << endl;
		cerr<< "[ERROR] option -f is required" << endl;
		cerr << endl;
		return 0;
	}

	if(_toLower_OUT_FILE == "")
	{
		_toLower_pout = &cout;
	}
	else
	{
		_toLower_fout.open(_toLower_OUT_FILE.c_str());
		if(_toLower_fout.is_open()==false)
		{
			cerr<< "[ERROR] could not open file: " << _toLower_OUT_FILE << endl;
			return 0;
		}
		_toLower_pout = &_toLower_fout;
	}

	return 1;
}

void printHelp_toLower()
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