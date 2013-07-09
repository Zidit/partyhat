

#include <fstream>
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


std::fstream serial;


std::vector<uint8_t> buildFrame(std::vector<uint8_t> &data)
{

	std::vector<uint8_t> rxdata;
	rxdata.push_back(0x7E);

	uint16_t len = data.size();
		
	rxdata.push_back((uint8_t)(len >> 8));
	rxdata.push_back((uint8_t)len);

	uint8_t cs = 0xFF;	
	
	for (auto it = data.begin(); it < data.end(); it++)	
	{
		uint8_t i = *it;
		if(i == 0x7E || i == 0x7D || i == 0x11 || i == 0x13)
		{
			rxdata.push_back(0x7D);
			rxdata.push_back(i ^ 0x20);
		}
		else rxdata.push_back(i);
		cs -= i;	
	}
	
	rxdata.push_back(cs);
	return rxdata;
}

std::vector<uint8_t> buildAtCommand(std::string &command, std::string &param, uint8_t id = 0)
{
	std::vector<uint8_t> data;
	data.push_back(0x08);
	data.push_back(id);
	data.push_back(command[0]);
	data.push_back(command[1]);

	for(int i = 0; i < param.size(); i++)
		data.push_back(param[i]);

	return buildFrame(data);

}

std::vector<uint8_t> buildTransmitRequest(std::vector<uint8_t> &data, uint64_t address64, uint8_t id = 0, uint16_t address16 = 0xFFFE, uint8_t radius = 0, bool multicast = false)
{
	std::vector<uint8_t> cmdData;
	cmdData.push_back(0x10);
	cmdData.push_back(id);
	
	cmdData.push_back((uint8_t)(address64 >> (8*7)));
	cmdData.push_back((uint8_t)(address64 >> (8*6)));
	cmdData.push_back((uint8_t)(address64 >> (8*5)));
	cmdData.push_back((uint8_t)(address64 >> (8*4)));
	cmdData.push_back((uint8_t)(address64 >> (8*3)));
	cmdData.push_back((uint8_t)(address64 >> (8*2)));
	cmdData.push_back((uint8_t)(address64 >> (8*1)));
	cmdData.push_back((uint8_t)(address64));

	cmdData.push_back((uint8_t)(address16 >> (8*1)));
	cmdData.push_back((uint8_t)(address16));

	cmdData.push_back(radius);
	if(multicast) cmdData.push_back(0x08);
	else cmdData.push_back(0x00);

	for (auto it = data.begin(); it < data.end(); it++)	
	{
		cmdData.push_back(*it);
	}	

	return buildFrame(cmdData);
}


void sendCommand(uint8_t command, uint64_t address64, std::vector<uint8_t> &payload)
{

		std::vector<uint8_t> d;
		d.push_back(command);
		
		for(int i = 0; i < payload.size(); i++)
		{
			d.push_back(payload[i]);
		}


		std::vector<uint8_t> rxdata = buildTransmitRequest(d, address64 , 0x00);

		for (auto it = rxdata.begin(); it < rxdata.end(); it++)
		{

			serial << *it;
			int a = *it;
			std::cout << std::hex << a << " ";
		}

		serial.flush();
		std::cout << std::endl;
	
}

void sendAnimation(std::vector<uint8_t> &data, uint64_t address64)
{
	int i = 0;
	int offset = 0;

	while(true)
	{
		std::vector<uint8_t> d;
		d.push_back(offset);
		
		while ( i < data.size() && i < 64 * (offset + 1))
			d.push_back(data[i++]);

		sendCommand(0x81, address64, d);
		sleep(1);

		offset++;
		if(64 * offset > data.size()) break;
	}

}





int main ( int argc, const char* argv[] )
{
	std::string port = "/dev/ttyUSB0";
	uint64_t address = 0;

	for(int i = 1; i < argc; i++)
	{
		if(!strcmp(argv[i], "-d"))
		{
			i++;
			if(i == argc) 
			{
				std::cerr << "-d need arguments \n";
				exit(1);  
			}

			port = argv[i];
			
		}
		if(!strcmp(argv[i], "-a"))
		{
			i++;
			if(i == argc) 
			{
				std::cerr << "-a need arguments \n";
				exit(1);  
			}
			address = std::stol(argv[i],0,0);
		}
	}


	serial.open(port.c_str(), std::ios::in | std::ios::out | std::ios::binary);
	if(!serial.is_open())
	{
		std::cerr << "Can't open device " << port << "\n";
		return 1;	
	}

	std::vector<uint8_t> in;	

	while(std::cin.good())
		in.push_back(std::cin.get());


	sendAnimation(in, address);

	std::vector<uint8_t> rxdata;
	sendCommand(0x80, address, rxdata);
	sendCommand(0x82, address, rxdata);

	char c[33];
	serial.read(c, 32);

	//std::string ss(c);
	std::cout << c;
	
	return 0;
}













