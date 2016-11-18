#include "format.h"
#include "common.h"
#include "kseq.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <getopt.h>
#include <limits.h>
#include <zlib.h>

using namespace std;

string 			_format_IN_FILE;
string 			_format_OUT_FILE;
int 			_format_LINE_WIDTH = 0;
long long 		_format_MIN_LEN = 0;
long long 		_format_MAX_LEN = LLONG_MAX;
bool 			_format_isFastq = false;
bool 			_format_filterN = false;
bool 			_format_noComment = true;
bool 			_format_pacbio = false;
ofstream 		_format_fout;
ostream 		*_format_pout;

int parseCommandLine_format(int argc, char *argv[]);
void printHelp_format();

KSEQ_INIT(gzFile, gzread)

void printRead_format(ostream &outObj, kseq_t *readSeq, long long cnt)
{
    int tmpPos;
    string tmpStr;
	if(_format_isFastq && readSeq->qual.l>0)
	{
		outObj<< "@" << readSeq->name.s;
		if(_format_pacbio)
			outObj<< "/" << cnt << "/0_" << readSeq->seq.l;
		if(_format_noComment==false && readSeq->comment.l > 0)
			outObj<< " " << readSeq->comment.s << "\n";
		else
			outObj<< "\n";
		outObj<< readSeq->seq.s << "\n";
		outObj<< "+" << "\n";
		outObj<< readSeq->qual.s << "\n";
	}
	else
	{
		outObj<< ">" << readSeq->name.s;
		if(_format_pacbio)
			outObj<< "/" << cnt << "/0_" << readSeq->seq.l;
		if(_format_noComment==false && readSeq->comment.l > 0)
			outObj<< " " << readSeq->comment.s << "\n";
		else
			outObj<< "\n";
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

bool hasNoN(string r)
{
	return (r.find('n')==string::npos && r.find('N')==string::npos);
}

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
	ostringstream sout;
    kseq_t *readSeq;
	readFile = gzopen(_format_IN_FILE.c_str(), "r");
	if(readFile==NULL)
	{
		cerr << "Cannot open file: " << _format_IN_FILE << endl;
		exit(1);
	}
	
    readSeq = kseq_init(readFile);
    bool flag1, flag2;
    long long cnt = 0;
	while (kseq_read(readSeq) >= 0)
	{
		sout.clear();
		sout.str("");
		flag1 = hasNoN(readSeq->seq.s);
		printRead_format(sout, readSeq, cnt);

		if( ((!_format_filterN) || (_format_filterN && flag1)) &&
			readSeq->seq.l >= _format_MIN_LEN && 
			readSeq->seq.l <= _format_MAX_LEN )
		{
			outObj<< sout.str();
			cnt++;
		}

		// if(kseq_read(readSeq) < 0)
		// 	break;
		// cnt++;
		// flag2 = hasNoN(readSeq->seq.s);
		// printRead_format(sout, readSeq, cnt);
		// if((!_format_filterN) || (_format_filterN && flag1 && flag2))
		// 	outObj<< sout.str();
		// else
		// 	cnt -= 2;
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
		{"minLen", 		required_argument, 		0, 		'm'	},
		{"maxLen", 		required_argument, 		0, 		'M'	},
		{"fastq", 		no_argument, 			0, 		'q'	},
		{"filterN", 	no_argument, 			0, 		'n'	},
		{"noComment", 	no_argument, 			0, 		'c'	},
		{"pacbio", 		no_argument, 			0, 		'p'	},
		{"help", 		no_argument, 			0, 		'h'	},
		{"version", 	no_argument, 			0, 		'v'	},
		{0,0,0,0}
	};

	while ( (c = getopt_long ( argc, argv, "f:o:w:m:M:qncphv", longOptions, &index))!= -1 )
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
			case 'm':
				_format_MIN_LEN = str2type<long long>(optarg);
				break;
			case 'M':
				_format_MAX_LEN = str2type<long long>(optarg);
				break;
			case 'q':
				_format_isFastq = true;
				break;
			case 'n':
				_format_filterN = true;
				break;
			case 'c':
				_format_noComment = false;
				break;
			case 'p':
				_format_pacbio = true;
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
		_format_LINE_WIDTH = 0;
	if(_format_MIN_LEN < 0)
		_format_MIN_LEN = 0;
	if(_format_MAX_LEN < 0)
		_format_MAX_LEN = LLONG_MAX;
	if(_format_MIN_LEN > _format_MAX_LEN)
	{
		cerr << endl;
		cerr<< "[ERROR] minLen cannot be greater than maxLen" << endl;
		cerr << endl;
		return 0;
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
	cerr << "         -m INT        min read length [0]" << endl;
	cerr << "         -M INT        max read length [LLONG_MAX]" << endl;
	cerr << "         -q            output reads in fastq format if possible" << endl;
	cerr << "         -v            print version" << endl;
	cerr << "         -h            print this help" << endl;
	cerr << endl;
}