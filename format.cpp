#include "format.h"
#include "common.h"
#include "kseq.h"
#include <iostream>
#include <fstream>
#include <getopt.h>
#include <zlib.h>

using namespace std;

string 		_format_IN_FILE;
string 		_format_OUT_FILE;
int 		_format_LINE_WIDTH = 0;
bool 		_format_isFastq = false;
ofstream 	_format_fout;
ostream 	*_format_pout;

int parseCommandLine_format(int argc, char *argv[]);
void printHelp_format();

KSEQ_INIT(gzFile, gzread)

int program_format(int argc, char* argv[])
{
	if(argc < 3)
	{
		printHelp_format();
		return 1;
	}
	else if(!parseCommandLine_format(argc, argv))
	{
		return 1;
	}
	ostream &outObj = *_format_pout;

	gzFile readFile;
    kseq_t *readSeq;
	readFile = gzopen(_format_IN_FILE.c_str(), "r");
	if(readFile==NULL)
	{
		cerr << "Cannot open file: " << _format_IN_FILE << endl;
		exit(1);
	}
	
    readSeq = kseq_init(readFile);
    int tmpPos;
    string tmpStr;
	while (kseq_read(readSeq) >= 0)
	{
		if(_format_isFastq && readSeq->qual.l>0)
		{
			outObj<< "@" << readSeq->name.s << "\n";
			outObj<< readSeq->seq.s << "\n";
			outObj<< "+" << "\n";
			outObj<< readSeq->qual.s << "\n";
		}
		else
		{
			outObj<< ">" << readSeq->name.s << "\n";
			if(_format_LINE_WIDTH == 0)
			{
				outObj<< readSeq->seq.s << "\n";
			}
			else
			{
				tmpPos = 0;
				tmpStr = readSeq->seq.s;
				while(readSeq->seq.l - tmpPos > _format_LINE_WIDTH)
				{
					outObj<< tmpStr.substr(tmpPos, _format_LINE_WIDTH) << "\n";
					tmpPos += _format_LINE_WIDTH;
				}
				outObj<< tmpStr.substr(tmpPos) << "\n";
			}
		}
	}
    kseq_destroy(readSeq);
    gzclose(readFile);

	if(_format_OUT_FILE != "")
	{
		_format_fout.close();
	}
	return 0;
}

int parseCommandLine_format(int argc, char *argv[])
{
	int index, c;

	static struct option longOptions[] =
	{
		{"file", 		required_argument, 		0, 		'f'	},
		{"out", 		required_argument, 		0, 		'o'	},
		{"lineWidth", 	required_argument, 		0, 		'w'	},
		{"fastq", 		no_argument, 			0, 		'q'	},
		{"help", 		no_argument, 			0, 		'h'	},
		{"version", 	no_argument, 			0, 		'v'	},
		{0,0,0,0}
	};

	while ( (c = getopt_long ( argc, argv, "f:o:w:qhv", longOptions, &index))!= -1 )
	{
		switch (c)
		{
			case 'f':
				_format_IN_FILE = optarg;
				break;
			case 'o':
				_format_OUT_FILE = optarg;
				break;
			case 'w':
				_format_LINE_WIDTH = str2type<int>(optarg);
				break;
			case 'q':
				_format_isFastq = true;
				break;
			case 'h':
				printHelp_format();
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

	if(_format_IN_FILE == "")
	{
		cerr << endl;
		cerr<< "[ERROR] option -f is required" << endl;
		cerr << endl;
		return 0;
	}
	if(_format_LINE_WIDTH < 0)
	{
		_format_LINE_WIDTH = 0;
	}

	if(_format_OUT_FILE == "")
	{
		_format_pout = &cout;
	}
	else
	{
		_format_fout.open(_format_OUT_FILE.c_str());
		if(_format_fout.is_open()==false)
		{
			cerr<< "[ERROR] could not open file: " << _format_OUT_FILE << endl;
			return 0;
		}
		_format_pout = &_format_fout;
	}

	return 1;
}

void printHelp_format()
{
	cerr << endl;
	cerr << "USAGE: fastUtils format -f in.fq [options]" << endl;
	cerr << endl;
	cerr << "Required options:" << endl;
	cerr << "         -f STR        fasta/q file containing the reads" << endl;
	cerr << "More options:" << endl;
	cerr << "         -o STR        output reads in STR file [stdout]" << endl;
	cerr << "         -w INT        wrap lines in fasta output, 0 means no wrapping [0]" << endl;
	cerr << "         -q            output reads in fastq format if possible" << endl;
	cerr << "         -v            print version" << endl;
	cerr << "         -h            print this help" << endl;
	cerr << endl;
}