#include "Histogrammer.h"
#include "DataStructs.h"
#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TH2.h>
#include <iostream>

Histogrammer::Histogrammer(const std::string& channelfile) :
	chanmap(channelfile)
{
}

Histogrammer::~Histogrammer() 
{
}

void Histogrammer::MyFill(THashTable* table, const std::string& name, const std::string& title, int bins, double min, double max, double value)
{
	TH1* h = (TH1*) table->FindObject(name.c_str());
	
	if(h == nullptr)
	{
		TH1F* histo = new TH1F(name.c_str(), title.c_str(), bins, min, max);
		histo->Fill(value);
		table->Add(histo);
	}
	else 
	{
		h->Fill(value);
	}
}

void Histogrammer::MyFill(THashTable* table, const std::string& name, const std::string& title, int binsx, double minx, double maxx, double valuex, int binsy, double miny, double maxy, double valuey)
{

	TH2* h = (TH2*) table->FindObject(name.c_str());
	
	if(h == nullptr)
	{
		TH2F* histo = new TH2F(name.c_str(), title.c_str(), binsx, minx, maxx, binsy, miny, maxy);
		histo->Fill(valuex, valuey);
		table->Add(histo);
	}
	else 
	{
		h->Fill(valuex, valuey);
	}
}

void Histogrammer::Run(const std::string& inname, const std::string& outname)
{
	if(!chanmap.IsValid())
	{
		std::cerr<<"Channel map is not properly setup at Histogrammer::Run(). Exiting."<<std::endl;
		return;
	}

	TFile* input = TFile::Open(inname.c_str(), "READ");
	TTree* tree = (TTree*) input->Get("CalTree");

	CalibratedEvent* event = new CalibratedEvent();
	tree->SetBranchAddress("event", &event);
	
	TFile* output = TFile::Open(outname.c_str(),"RECREATE");

	THashTable* table = new THashTable();

	long nentries = tree->GetEntries();
	long flush_val = 0.05*nentries;
	int flush_count=0;
	long count=0;


	double sum_energy;
	double sum_energy_wthresh;
	int count_wthresh;
	int total_count;

	const double si_threshold = 0.6;

	for(long i=0; i<nentries; i++)
	{
		tree->GetEntry(i);
		count++;
		if(count == flush_val)
		{
			flush_count++;
			count = 0;
			std::cout<<"\rPercent of data processed: "<<0.05*flush_count*100.0<<std::flush;
		}

		sum_energy=0;
		sum_energy_wthresh=0;
		count_wthresh=0;
		total_count = event->barrel1.size() + event->barrel2.size() + event->fqqq.size() + event->bqqq.size();
		for(auto& hit : event->barrel1)
		{
			sum_energy += hit.back_energy;
			MyFill(table,"energy_channel","Energy v. Channel;g. channel;Energy(MeV)",544,0,544,hit.back_gchan,1000,0.0,20.0,hit.back_energy);
			if(hit.back_energy > si_threshold)
			{
				sum_energy_wthresh += hit.back_energy;
				MyFill(table,"energy_channel_wThresh","Energy v. Channel with Threshold;g. channel;Energy(MeV)",544,0,544,hit.back_gchan,1000,0.0,20.0,hit.back_energy);
				count_wthresh++;
			}
		}
		for(auto& hit : event->barrel2)
		{
			sum_energy += hit.back_energy;
			MyFill(table,"energy_channel","Energy v. Channel;g. channel;Energy(MeV)",544,0,544,hit.back_gchan,1000,0.0,20.0,hit.back_energy);
			if(hit.back_energy > si_threshold)
			{
				sum_energy_wthresh += hit.back_energy;
				MyFill(table,"energy_channel_wThresh","Energy v. Channel with Threshold;g. channel;Energy(MeV)",544,0,544,hit.back_gchan,1000,0.0,20.0,hit.back_energy);
				count_wthresh++;
			}
		}
		for(auto& hit : event->fqqq)
		{
			sum_energy += hit.wedge_energy;
			MyFill(table,"energy_channel","Energy v. Channel;g. channel;Energy(MeV)",544,0,544,hit.wedge_gchan,1000,0.0,20.0,hit.wedge_energy);
			if(hit.wedge_energy > si_threshold)
			{
				sum_energy_wthresh += hit.wedge_energy;
				MyFill(table,"energy_channel_wThresh","Energy v. Channel with Threshold;g. channel;Energy(MeV)",544,0,544,hit.wedge_gchan,1000,0.0,20.0,hit.wedge_energy);
				count_wthresh++;
			}
		}
		for(auto& hit : event->bqqq)
		{
			sum_energy += hit.wedge_energy;
			MyFill(table,"energy_channel","Energy v. Channel;g. channel;Energy(MeV)",544,0,544,hit.wedge_gchan,1000,0.0,20.0,hit.wedge_energy);
			if(hit.wedge_energy > si_threshold)
			{
				sum_energy_wthresh += hit.wedge_energy;
				MyFill(table,"energy_channel_wThresh","Energy v. Channel with Threshold;g. channel;Energy(MeV)",544,0,544,hit.wedge_gchan,1000,0.0,20.0,hit.wedge_energy);
				count_wthresh++;
			}
		}
		
		MyFill(table, "sum_energy", "Sum of Energies;Energy(MeV);Counts",2000,0.0,60.0,sum_energy);
		MyFill(table, "sume_v_mult","Sum of Energies vs. Multiplicity;Energy(MeV);Multiplicity",1000,0.0,60.0,sum_energy,10,0,10,total_count);
		if(count_wthresh > 0)
		{
			MyFill(table, "sum_energy_wThresh", "Sum of Energies with Threshold;Energy(MeV);Counts",2000,0.0,60.0,sum_energy_wthresh);
			MyFill(table, "sume_v_mult_wThresh","Sum of Energies vs. Multiplicity with Threshold;Energy(MeV);Multiplicity",1000,0.0,60.0,sum_energy_wthresh,
																															10,0,10,count_wthresh);
		}
		if(count_wthresh == 3)
		{
			MyFill(table, "sum_energy_wthresh_3coinc","Sum of ANASEN Energies with Threshold--3 hits;Energy(MeV);Counts",500,0.0,30.0,sum_energy_wthresh);
		}
		
	}
	std::cout<<std::endl;
	input->Close();
	output->cd();
	table->Write();
	output->Close();
}

