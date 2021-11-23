#ifndef HISTOGRAMMER_H
#define HISTOGRAMMER_H

#include <string>
#include <vector>
#include <THashTable.h>
#include "ChannelMap.h"


class Histogrammer
{
public:
	Histogrammer(const std::string& channelfile);
	~Histogrammer();
	void Run(const std::string& inname, const std::string& outname);

private:
	void MyFill(THashTable* table, const std::string& name, const std::string& title, int bins, double min, double max, double valuex);
	void MyFill(THashTable* table, const std::string& name, const std::string& title, int binsx, double minx, double maxx, double valuex, int binsy, double miny, double maxy, double valuey);

	ChannelMap chanmap;
};

#endif
