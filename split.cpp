#include "toLower.h"
#include "common.h"
#include "kseq.h"
#include <iostream>
#include <fstream>
#include <getopt.h>
#include <zlib.h>

using namespace std;

string 		_split_IN_FILE;
string 		_split_OUT_PRE;
int 		_split_LINE_WIDTH = 0;
bool 		_split_ISFASTQ = false;
ofstream 	_split_fout;
double 		_split_SIZE;

int parseCommandLine_split(int argc, char *argv[]);
void printHelp_split();

KSEQ_INIT(gzFile, gzread)

int program_split(int argc, char* argv[])
{
	if(argc < 3)
	{
		printHelp_split();
		return 1;
	}
	else if(!parseCommandLine_split(argc, argv))
	{
		return 1;
	}
	// ostream &outObj = *_toLower_pout;

	gzFile readFile;
    kseq_t *readSeq;
	readFile = gzopen(_split_IN_FILE.c_str(), "r");
	if(readFile==NULL)
	{
		cerr << "Cannot open file: " << _split_IN_FILE << endl;
		exit(1);
	}

	bool isOpen = false;
	int cnt = 1;
	long long totalBases = 0;
    int tmpPos;
    string tmpStr;
	
    readSeq = kseq_init(readFile);
	while (kseq_read(readSeq) >= 0)
	{
		if(isOpen==false)
		{
			if(_split_ISFASTQ && readSeq->qual.l>0)
			{
				_split_fout.open((_split_OUT_PRE+type2str<int>(cnt)+".fastq").c_str());
				if(_split_fout.is_open()==false)
				{
					cerr<< "[ERROR] could not open file: " << (_split_OUT_PRE+type2str<int>(cnt)+".fastq") << endl;
					return 0;
				}
			}
			else
			{
				_split_fout.open((_split_OUT_PRE+type2str<int>(cnt)+".fasta").c_str());
				if(_split_fout.is_open()==false)
				{
					cerr<< "[ERROR] could not open file: " << (_split_OUT_PRE+type2str<int>(cnt)+".fasta") << endl;
					return 0;
				}
			}
			isOpen = true;
		}
		if(_split_ISFASTQ && readSeq->qual.l>0)
		{
			_split_fout<< "@" << readSeq->name.s << "\n";
			_split_fout<< readSeq->seq.s << "\n";
			_split_fout<< "+" << "\n";
			_split_fout<< readSeq->qual.s << "\n";
		}
		else
		{
			_split_fout<< ">" << readSeq->name.s << "\n";
			if(_split_LINE_WIDTH == 0)
			{
				_split_fout<< readSeq->seq.s << "\n";
			}
			else
			{
				tmpPos = 0;
				tmpStr = readSeq->seq.s;
				while(readSeq->seq.l - tmpPos > _split_LINE_WIDTH)
				{
					_split_fout<< tmpStr.substr(tmpPos, _split_LINE_WIDTH) << "\n";
					tmpPos += _split_LINE_WIDTH;
				}
				_split_fout<< tmpStr.substr(tmpPos) << "\n";
			}
		}
		totalBases += readSeq->seq.l;
		if(totalBases > _split_SIZE)
		{
			_split_fout.close();
			totalBases = 0;
			cnt++;
			if(_split_ISFASTQ && readSeq->qual.l>0)
			{
				_split_fout.open((_split_OUT_PRE+type2str<int>(cnt)+".fastq").c_str());
				if(_split_fout.is_open()==false)
				{
					cerr<< "[ERROR] could not open file: " << (_split_OUT_PRE+type2str<int>(cnt)+".fastq") << endl;
					return 0;
				}
			}
			else
			{
				_split_fout.open((_split_OUT_PRE+type2str<int>(cnt)+".fasta").c_str());
				if(_split_fout.is_open()==false)
				{
					cerr<< "[ERROR] could not open file: " << (_split_OUT_PRE+type2str<int>(cnt)+".fasta") << endl;
					return 0;
				}
			}
		}
	}
    kseq_destroy(readSeq);
    gzclose(readFile);

	return 0;
}

int parseCommandLine_split(int argc, char *argv[])
{
	int index, c;

	static struct option longOptions[] =
	{
		{"file", 		required_argument, 		0, 		'f'	},
		{"prefix", 		required_argument, 		0, 		'p'	},
		{"size", 		required_argument, 		0, 		's'	},
		{"lineWidth", 	required_argument, 		0, 		'w'	},
		{"fastq", 		no_argument, 			0, 		'q'	},
		{"help", 		no_argument, 			0, 		'h'	},
		{"version", 	no_argument, 			0, 		'v'	},
		{0,0,0,0}
	};

	while ( (c = getopt_long( argc, argv, "f:p:s:w:qhv", longOptions, &index))!= -1 )
	{
		switch (c)
		{
			case 'f':
				_split_IN_FILE = optarg;
				break;
			case 'p':
				_split_OUT_PRE = optarg;
				break;
			case 's':
				_split_SIZE = str2type<double>(optarg);
				break;
			case 'w':
				_split_LINE_WIDTH = str2type<int>(optarg);
				break;
			case 'q':
				_split_ISFASTQ = true;
				break;
			case 'h':
				printHelp_split();
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

	if(_split_IN_FILE == "")
	{
		cerr << endl;
		cerr<< "[ERROR] option -f is required" << endl;
		cerr << endl;
		return 0;
	}

	if(_split_OUT_PRE == "")
	{
		cerr << endl;
		cerr<< "[ERROR] option -p is required" << endl;
		cerr << endl;
		return 0;
	}

	// if(_split_SIZE < 1)
	// {
	// 	cerr << endl;
	// 	cerr<< "[ERROR] option -s is required (should be positive integer)" << endl;
	// 	cerr << endl;
	// 	return 0;
	// }
	// else
	// {
		_split_SIZE = _split_SIZE*1000*1000;
	// }

	return 1;
}

void printHelp_split()
{
	cerr << endl;
	cerr << "USAGE: fastUtils split -f in.fq -p prefix -s INT [options]" << endl;
	cerr << endl;
	cerr << "Required options:" << endl;
	cerr << "         -f STR        fasta/q file containing the reads" << endl;
	cerr << "         -p STR        prefix of output files" << endl;
	cerr << "         -s INT        maximum number of Mega bases (Mbp) for each file" << endl;
	cerr << "More options:" << endl;
	cerr << "         -w INT        wrap lines in fasta output, 0 means no wrapping [0]" << endl;
	cerr << "         -q            output reads in fastq format if possible" << endl;
	cerr << "         -v            print version" << endl;
	cerr << "         -h            print this help" << endl;
	cerr << endl;
}