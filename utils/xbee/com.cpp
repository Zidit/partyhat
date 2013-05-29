
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <stdint.h>
#include <sstream>

char data[255];
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


int main ( int argc, const char* argv[] )
{
	std::string port = "/dev/ttyUSB0";

	serial.open(port.c_str(), std::ios::in | std::ios::out | std::ios::binary);
	if(!serial.is_open())
	{
		std::cerr << "Can't open device " << port << "\n";
		return 1;	
	}
	

	std::vector<uint8_t> d;
	d.push_back(0x06);	

	rxdata = buildTransmitRequest(d, 0x0013A2004089EDEB, 0x01);
	for (auto it = rxdata.begin(); it < rxdata.end(); it++)
	{
		serial << *it;
		std::cout << *it;
	}
	std::cout << "\n";

	return 0;
}













