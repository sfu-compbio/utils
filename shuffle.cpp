#include "shuffle.h"
#include "kseq.h"
#include <iostream>
#include <fstream>
#include <getopt.h>
#include <zlib.h>

using namespace std;

string 		_shuffle_IN_FILE1;
string 		_shuffle_IN_FILE2;
string 		_shuffle_OUT_FILE;
char 		_shuffle_SEP_CHAR = '.';
bool 		_shuffle_isFastq = false;
ofstream 	_shuffle_fout;
ostream 	*_shuffle_pout;

int parseCommandLine_shuffle(int argc, char *argv[]);
void printHelp_shuffle();

KSEQ_INIT(gzFile, gzread)

int program_shuffle(int argc, char* argv[])
{
	if(argc < 3)
	{
		printHelp_shuffle();		
		return 1;
	}
	else if(!parseCommandLine_shuffle(argc, argv))
	{
		return 1;
	}
	ostream &outObj = *_shuffle_pout;

	gzFile readFile1;
	gzFile readFile2;
    kseq_t *readSeq1;
    kseq_t *readSeq2;
	readFile1 = gzopen(_shuffle_IN_FILE1.c_str(), "r");
	readFile2 = gzopen(_shuffle_IN_FILE2.c_str(), "r");
	if(readFile1==NULL)
	{
		cerr << "Cannot open file: " << _shuffle_IN_FILE1 << endl;
		exit(1);
	}
	if(readFile2==NULL)
	{
		cerr << "Cannot open file: " << _shuffle_IN_FILE2 << endl;
		exit(1);
	}
	
    readSeq1 = kseq_init(readFile1);
    readSeq2 = kseq_init(readFile2);
	while (kseq_read(readSeq1) >= 0)
	{
		kseq_read(readSeq2);
		if(_shuffle_isFastq && readSeq1->qual.l>0)
		{
			outObj<< "@" << readSeq1->name.s << _shuffle_SEP_CHAR << "1" << "\n";
			outObj<< readSeq1->seq.s << "\n";
			outObj<< "+" << "\n";
			outObj<< readSeq1->qual.s << "\n";
			outObj<< "@" << readSeq2->name.s << _shuffle_SEP_CHAR << "2" << "\n";
			outObj<< readSeq2->seq.s << "\n";
			outObj<< "+" << "\n";
			outObj<< readSeq2->qual.s << "\n";
		}
		else
		{
			outObj<< ">" << readSeq1->name.s << _shuffle_SEP_CHAR << "1" << "\n";
			outObj<< readSeq1->seq.s << "\n";
			outObj<< ">" << readSeq2->name.s << _shuffle_SEP_CHAR << "2" << "\n";
			outObj<< readSeq2->seq.s << "\n";
		}
	}
    kseq_destroy(readSeq1);
    kseq_destroy(readSeq2);
    gzclose(readFile1);
    gzclose(readFile2);

	if(_shuffle_OUT_FILE != "")
	{
		_shuffle_fout.close();
	}
	return 0;
}

int parseCommandLine_shuffle(int argc, char *argv[])
{
	int index, c;

	static struct option longOptions[] =
	{
		{"in1", 		required_argument, 		0, 		'1'	},
		{"in2", 		required_argument, 		0, 		'2'	},
		{"out", 		required_argument, 		0, 		'o'	},
		{"separator", 	required_argument, 		0, 		's'	},
		{"fastq", 		no_argument, 			0, 		'q'	},
		{"help", 		no_argument, 			0, 		'h'	},
		{"version", 	no_argument, 			0, 		'v'	},
		{0,0,0,0}
	};

	while ( (c = getopt_long ( argc, argv, "1:2:o:s:qhv", longOptions, &index))!= -1 )
	{
		switch (c)
		{
			case '1':
				_shuffle_IN_FILE1 = optarg;
				break;
			case '2':
				_shuffle_IN_FILE2 = optarg;
				break;
			case 'o':
				_shuffle_OUT_FILE = optarg;
				break;
			case 's':
				_shuffle_SEP_CHAR = optarg[0];
				break;
			case 'q':
				_shuffle_isFastq = true;
				break;
			case 'h':
				printHelp_shuffle();
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

	if(_shuffle_IN_FILE1 == "")
	{
		cerr << endl;
		cerr<< "[ERROR] option -1 is required" << endl;
		cerr << endl;
		return 0;
	}

	if(_shuffle_IN_FILE2 == "")
	{
		cerr << endl;
		cerr<< "[ERROR] option -2 is required" << endl;
		cerr << endl;
		return 0;
	}

	if(_shuffle_OUT_FILE == "")
	{
		_shuffle_pout = &cout;
	}
	else
	{
		_shuffle_fout.open(_shuffle_OUT_FILE.c_str());
		if(_shuffle_fout.is_open()==false)
		{
			cerr<< "[ERROR] could not open file: " << _shuffle_OUT_FILE << endl;
			return 0;
		}
		_shuffle_pout = &_shuffle_fout;
	}

	return 1;
}

void printHelp_shuffle()
{
	cerr << endl;
	cerr << "USAGE: fastUtils shuffle -1 a_1.fq -2 a_2.fq [options]" << endl;
	cerr << endl;
	cerr << "Required options:" << endl;
	cerr << "         -1 STR        fasta/q file containing first mate of the reads" << endl;
	cerr << "         -2 STR        fasta/q file containing second mate of the reads" << endl;
	cerr << "More options:" << endl;
	cerr << "         -o STR        output interlaced reads in STR file [stdout]" << endl;
	cerr << "         -s CHR        separator character [.]" << endl;
	cerr << "         -v            print version" << endl;
	cerr << "         -h            print this help" << endl;
	cerr << endl;
}