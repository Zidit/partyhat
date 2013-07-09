
#include <iostream>
#include <string>
#include <math.h>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <stdio.h>
#include <string.h>
#include <iomanip>


using namespace std;

#define STORE_WREG_MASK 	0b10000000

#define OPCODE_LOAD_CONST	0x01
#define OPCODE_LOAD_RAM 	0x02
#define OPCODE_STORE_RAM 	0x03
#define OPCODE_ADD			0x04
#define OPCODE_SUB			0x05
#define OPCODE_AND			0x06
#define OPCODE_OR			0x07
#define OPCODE_XOR			0x08
#define OPCODE_INC			0x09
#define OPCODE_DEC			0x0A
#define OPCODE_SHIFT_R		0x0B
#define OPCODE_SHIFT_L		0x0C
#define OPCODE_UPDATE_WREG 	0x10
#define OPCODE_UPDATE_CONST	0x11
#define OPCODE_SIWZ			0x12
#define OPCODE_SIRZ			0x13
#define OPCODE_SICZ			0x14
#define OPCODE_JIWZ			0x15
#define OPCODE_JICZ			0x16
#define OPCODE_JUMP			0x17
#define OPCODE_RST			0x18
#define OPCODE_REL_JUMP		0x19
#define OPCODE_SIWE			0x1A
#define OPCODE_SIWB			0x1B
#define OPCODE_SIWS			0x1C
#define OPCODE_RET			0x1D

typedef struct {
	std::string name;
	std::string replacement;
} define;

typedef struct {
	std::string name;
	std::string address;
} label;

std::vector<define> defines;
std::vector<label> labels;


int operandToOpcode(std::string str, int &args)
{
	args = 0;
	if(str == "SIWZ") return OPCODE_SIWZ;
	if(str == "SICZ") return OPCODE_SICZ;
	if(str == "RET") return OPCODE_RET;
	if(str == "UDW") return OPCODE_UPDATE_WREG;
	
	args = 1;
	if(str == "LDC") return OPCODE_LOAD_CONST;
	if(str == "LDR") return OPCODE_LOAD_RAM;
	if(str == "STR") return OPCODE_STORE_RAM;
	if(str == "ADD:W") return OPCODE_ADD | STORE_WREG_MASK;
	if(str == "ADD:R") return OPCODE_ADD;
	if(str == "SUB:W") return OPCODE_SUB | STORE_WREG_MASK;
	if(str == "SUB:R") return OPCODE_SUB;
	if(str == "AND:W") return OPCODE_AND | STORE_WREG_MASK;
	if(str == "AND:R") return OPCODE_AND;
	if(str == "OR:W") return OPCODE_OR | STORE_WREG_MASK;
	if(str == "OR:R") return OPCODE_OR;
	if(str == "XOR:W") return OPCODE_XOR | STORE_WREG_MASK;
	if(str == "XOR:R") return OPCODE_XOR;
	if(str == "INC") return OPCODE_INC;
	if(str == "DEC") return OPCODE_DEC;
	if(str == "SR") return OPCODE_SHIFT_R;
	if(str == "SL") return OPCODE_SHIFT_L;
	if(str == "UDC") return OPCODE_UPDATE_CONST;
	if(str == "SIRZ") return OPCODE_SIRZ;
	if(str == "JIWZ") return OPCODE_JIWZ;
	if(str == "JICZ") return OPCODE_JICZ;
	if(str == "JMP") return OPCODE_JUMP;
	if(str == "RJMP") return OPCODE_REL_JUMP;
	if(str == "RST") return OPCODE_RST;
	if(str == "SIWE") return OPCODE_SIWE;
	if(str == "SIWB") return OPCODE_SIWB;
	if(str == "SIWS") return OPCODE_SIWS;

	throw string("Instruction not found");
	return 0;
}

int valueToNumber(std::string str)
{

	int i;
	
	try {
		if (str.substr(0,2) == "0b" || str.substr(0,2) == "0B")	
			i =  stoi(str.substr(2),0,2);
		else		
			i = stoi(str,0,0);
	}
	catch (...)
	{
		throw string("Number conversion failed");
	}

	if (i > 255 || i < -128) throw string("Value out of range");
	return i;	
}




int codeToHex(std::vector<std::string> &words)
{
	int i = 0; 
	int args;
	i = operandToOpcode(words[0] , args);
	i &= 0xFF;
	i <<= 8;

	if (args + 1 < words.size()) throw ("Too few arguments");
	if (args + 1 > words.size()) throw ("Too many arguments");

	if(args == 1)
	{
		int v;
		v = valueToNumber(words[1]);
		v &= 0xFF;
		i |= v;

	}
	return i;
}

void removeComments(std::string &str)
{
	size_t p = str.find(";");
	
	if(p != string::npos)
		str.erase(p);

}

void explode(std::string &str, std::vector<std::string> &words)
{

	istringstream iss(str);
	copy(istream_iterator<string>(iss),
		     istream_iterator<string>(),
		     back_inserter<vector<string> >(words));

}

void preProsessing(std::vector<std::string> &words)
{

	if(words[0] == "#define")
	{
		if (words.size() < 3) throw string("Too few arguments");
		define tmp;
		tmp.name = words[1];
		tmp.replacement = words[2];
		defines.push_back(tmp);
		words.clear();
		return;
	}
	else if(words[0][0] == '#')
		throw string("Not a valid preprosessor instruction");

	for(int i = 0; i < defines.size(); i++)
	{
		for(int u = 0; u < words.size(); u++)
		{
			if(defines[i].name == words[u])
			{
				words[u] = defines[i].replacement;

			}
		}
	}

}

bool prosessInput(std::string str, std::vector<std::string> &words)
{

	if(str.empty()) return false; 
	
	removeComments(str);
	if(str.empty()) return false; 
	explode(str,words);
	preProsessing(words);
	return !(words.empty());


}

void resolveLabels(std::vector<std::string> &words)
{
	for (auto it : labels)
	{
		if( it.name == words[1])
		{
			words[1] = it.address; 
		}	
	}
}

bool findLables(std::vector<std::string> &words, int lineNumber)
{
	if(words[0][0] == ':')
	{
		label tmp;
		std::string name = words[0].substr(1);
		for (auto it : labels)
			if( it.name == name) 
				throw string("Double definition of lable");

		tmp.name = name;
		tmp.address = std::to_string(lineNumber);
		labels.push_back(tmp);
		return true;
	}
	return false;
}

void getAssemblyCode(std::vector<std::vector<std::string> > &code, std::istream &inStream)
{

	std::string line;
	int lineNbr = 1;
	
	try
	{
		while(std::cin.good()){

			std::getline(inStream, line);
			std::vector<std::string> words;

			if (prosessInput(line, words))
				code.push_back(words);

			lineNbr++;
		}

		lineNbr = 0;

		auto it = code.begin();
		while (it != code.end()) {
			if (findLables(*it, lineNbr)) {
				it = code.erase(it);
			}
			else {
				++it;
				lineNbr++;
			}		
		}

		for( auto it = code.begin(); it < code.end(); it++)
		{
			std::vector<std::string> words = *it;
			resolveLabels(words);
			*it = words;
		}
	}
	catch (string &s)
	{
		cerr << "Error on line " << lineNbr << ": " << s << "\n";
		cerr << line << "\n";
		exit(1);
	}		
}  

void printHelp()
{
	cerr << "Partyhat assember 0.1 \n";
	cerr << "Usage e.g.: cat test.asm | ./assembler -c \n";
	cerr << "Usage e.g.: cat test.asm | ./assembler -r >> /dev/ttyUSB0\n";	
	cerr << " \n";
	cerr << "-a		prints preprosessed assembly code \n";			
	cerr << "-c		prints porgram as c array \n";
	cerr << "-l		list all labels used in program \n";
	cerr << "-H		prints program as list of hex desimals \n";
	cerr << "-h or --help	prints this help \n";
	cerr << "-r		prints code as raw binary data \n";
	cerr << "-s		get instruction count  \n";
	exit(0);
}

int main ( int argc, const char* argv[] )
{

	if(argc == 1) printHelp();
	if(argc == 2)
		if(!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help") )
			printHelp();

	std::vector<std::vector<std::string> > code;

	getAssemblyCode(code, std::cin);

	std::string line;
	std::string output; 
	
	/*for(int a = 0; a < argc; a++)
	{
		cout << argv[a] << "\n";
	}*/

	if(argc == 2)
	{
		if(!strcmp(argv[1], "-a"))
		{
			for( auto it : code)
			{
				line = it[0] + " " + it[1];
				cout << line << "\n";
			}
			return 0;
		}
		if(!strcmp(argv[1], "-l"))
		{
			for( auto it : labels)
			{
				cout << it.name << " " << it.address << "\n";
			}
			return 0;
		}	
		if(!strcmp(argv[1], "-s"))
		{
			cout << code.size() << "\n";
			return 0;
		}	
	}


	std::vector<int> instructions;

	try 
	{
		for( auto it : code)
		{
			line = it[0] + " " + it[1];
			int i = codeToHex(it);
			instructions.push_back(i);
		}
	}
	catch (string &s)
	{
		cerr << "Error: " << s << "\n";
		cerr << line << "\n";
		exit(1);
	}	

	if(argc == 2)
	{
		if(!strcmp(argv[1], "-H"))
		{
			for(int i = 0; i < instructions.size(); i++)
			{	
				stringstream ss;
				ss << std::hex << std::showbase << instructions[i] ;
				string  value;
				ss >> value;
				output += value;	
				output += "\n";	
			}
			cout << output;
			return 0;
		}

		if(!strcmp(argv[1], "-c"))
		{
			output = "uint16_t animation[] = { ";

			for(int i = 0; i < instructions.size(); i++)
			{	
				stringstream ss;
						
			 	ss << std::hex << "0x" <<  std::setw(4) << std::setfill('0') << instructions[i] ;
				//ss << std::hex << std::showbase << instructions[i] ;
				string  value;
				ss >> value;
				output += value;	
				output += ", ";	
			}
			output.erase(output.size()-2);	
			output += "};\n";
			cout << output;
		}	
		if(!strcmp(argv[1], "-r"))
		{
			std::vector<unsigned char> data;
			//data.push_back(instructions.size()*2);

			for(int i = 0; i < instructions.size(); i++)
			{	
				data.push_back((unsigned char)(instructions[i] >> 8));			
				data.push_back((unsigned char)instructions[i]);

			}
			
			fwrite(data.data(),1, data.size(), stdout);
		}	
	}



}


