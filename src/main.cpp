#include <string>
#include <fstream>
#include <iostream>
#include "Histogrammer.h"

int main(int argc, char** argv)
{
	if(argc!=2)
	{
		std::cerr<<"Anasen Reconstruction requires an input file!"<<std::endl;
		return 1;
	}

	std::ifstream input(argv[1]);
	if(!input.is_open())
	{
		std::cerr<<"Unable to open input file for Anasen Reconstruction."<<std::endl;
		return 1;
	}

	std::string junk, data_file, output_file, mapfile;

	input>>junk>>data_file;
	input>>junk>>output_file;
	input>>junk>>mapfile;
	

	input.close();

	std::cout<<"----------Anasen Reconstruction----------"<<std::endl;
	std::cout<<"Running reconstruction analysis on data from file: "<<data_file<<std::endl;
	std::cout<<"Outputing results to file: "<<output_file<<std::endl;
	std::cout<<"Beginning operation..."<<std::endl;
	Histogrammer grammer(mapfile);
	grammer.Run(data_file, output_file);
	std::cout<<"-----------------------------------------"<<std::endl;
}

