/**
* @file: test.cpp
* @author: Liu Hongbin
* @brief:
* @date:   2019-10-09 11:04:42
* @last Modified by:   lenovo
* @last Modified time: 2019-11-26 17:23:58
*/
#include <iostream>
#include <fstream>
#include <string>
#include <yaml-cpp/yaml.h>
#include <assert.h>
#include <unistd.h>
#include "cstdlib"
#include "mpi.h"
#include "utilities.hpp"
#include "loadBalancer.hpp"
#include "parameter.hpp"
#include "cgnslib.h"
#define OUT std::cout
#define IN std::cin
#define ENDL std::endl
#define String std::string

using namespace HSF;

// #define DEBUG_YAML

// void loadRegionTopologyFromYAML(String filePtr, Array<scalar> &s,
// 	ArrayArray<label> &nei, label procNum);
// void operator >> (const YAML::Node& node, Array<scalar>& s);
// void operator >> (const YAML::Node& node, Array<Array<label> >& nei);
// void operator >> (const YAML::Node& node, Array<label>& regionIdx);
// void hdf5ToAdf(char* filePtr, char* desFilePtr);

int main(int argc, char** argv)
{
	LoadBalancer *lb = new LoadBalancer();
	OUT<<"hello world!"<<ENDL;

	Parameter para;

	/// initialize MPI environment
	printf("initialize MPI environment ......\n");
	int numproces, rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numproces);
	printf("This is process %d, %d processes are launched\n", rank, numproces);

	// if(rank==0) std::cout<<"Solver: "<<para.getDomain("dom").getEquation("P").getSolver()<<std::endl;
	// if(rank==0) std::cout<<"Path: "<<para.getDomain("dom").getRegion("region2").getPath()<<std::endl;
	/// construct mesh block topology
	// Array<scalar> s;
	// ArrayArray<label> nei;
	// // label procNum = 4;
	// loadRegionTopologyFromYAML("regionTopo.yaml",s, nei, numproces);

	// LoadBalancer *lb = new LoadBalancer(s, nei, procNum);

	// lb->LoadBalancer_2(s, nei, procNum);

	/// evaluate the result of load balancer
	// ArrayArray<label> procId = lb->getProcId();
	// OUT<<"Item: region (processes ID)"<<ENDL;
	// procId.display();
	// ArrayArray<scalar> procLoad = lb->getProcLoad();
	// OUT<<"Item: region (measurement)"<<ENDL;
	// procLoad.display();
	// scalar* procLoadSum = lb->getProcLoadSum();
	// OUT<<"Item: process (measurement)"<<ENDL;
	// for (int i = 0; i < procNum; ++i)
	// {
	// 	OUT<<"Item: "<<i<<" ("<<procLoadSum[i]<<")"<<ENDL;
	// }
	char* meshFile = "/home/export/online1/amd_dev1/liuhb/unstructured_frame/data/hexa_20.cgns";
	char* resultFile = "/home/export/online1/amd_dev1/liuhb/unstructured_frame/data/hexa_result.cgns";

	/// read CGNS file
	Array<Region> regs(1);
	regs[0].initBeforeBalance(meshFile);

	// /// only one region for test.
	// Region reg;
	// // reg.getMesh().readMesh("data/40W.cgns");
	// // printf("writing HDF5 cgns file: ......\n");
	// // reg.getMesh().writeMesh("data/hdf5.cgns");
	// printf("reading HDF5 cgns file: ......\n");
	// // reg.getMesh().initMesh("data/hdf5.cgns");
	// // MPI_Barrier(MPI_COMM_WORLD);
	// // reg.getMesh().readMesh("data/hdf5.cgns");
	// reg.getMesh().readMesh("/home/export/online1/amd_dev1/liuhb/unstructured_frame/data/hdf5.cgns");
	// MPI_Barrier(MPI_COMM_WORLD);
	// // MPI_Barrier(MPI_COMM_WORLD);

	// regs.push_back(reg);

	/// load balance in region
	lb->LoadBalancer_3(regs);

	regs[0].initAfterBalance();
	// regs[0].getMesh().getTopology().constructTopology();
	// MPI_Barrier(MPI_COMM_WORLD);

	// regs[0].getBoundary().readMesh("/home/export/online1/amd_dev1/liuhb/unstructured_frame/data/hdf5.cgns");
	// Topology innerTopo = regs[0].getMesh().getTopology();
	// regs[0].getBoundary().exchangeBoundaryElements(innerTopo);

	// regs[0].getMesh().writeMesh(meshFile, parts);
	regs[0].writeMesh(resultFile);

	MPI_Finalize();
	return 0;
}

// void loadRegionTopologyFromYAML(String filePtr, Array<scalar> &s,
// 	ArrayArray<label> &nei, label procNum)
// {
// 	OUT<<"reading YAML file: "<<filePtr<<" ......"<<ENDL;
// 	std::ifstream fin(filePtr.c_str());
// 	YAML::Parser parse(fin);
// 	YAML::Node doc;
// 	parse.GetNextDocument(doc);
// 	const YAML::Node& measurement = doc["measurement"];
// 	for (int i = 0; i < measurement.size(); ++i)
// 	{
// 		measurement[i] >> s;
// 	}
// 	Array<Array<label> > neiTmp;
// 	Array<label> regIdxTmp;
// 	const YAML::Node& topology = doc["topology"];
// 	for (int i = 0; i < topology.size(); ++i)
// 	{
// 		topology[i] >> neiTmp;
// 		topology[i] >> regIdxTmp;
// 	}

// 	/// transform the vector<vector<int> > to ArrayArray
// 	nei.num = s.size();
// 	nei.startIdx = new label[nei.num+1];
// 	nei.startIdx[0] = 0;
// 	for (int i = 0; i < nei.num; ++i)
// 	{
// 		nei.startIdx[i+1] = nei.startIdx[i]+neiTmp[regIdxTmp[i]].size();
// 	}
// 	nei.data = new label[nei.startIdx[nei.num]];
// 	for (int i = 0; i < nei.num; ++i)
// 	{
// 		label k = 0;
// 		for (int j = nei.startIdx[i]; j < nei.startIdx[i+1]; ++j)
// 		{
// 			nei.data[j] = neiTmp[regIdxTmp[i]][k];
// 			k++;
// 		}
// 	}
// #ifdef DEBUG_YAML
// 	/// check
// 	for (int i = 0; i < nei.num; ++i)
// 	{
// 		for (int j = nei.startIdx[i]; j < nei.startIdx[i+1]; ++j)
// 		{
// 			OUT<<nei.data[j]<<", ";
// 		}
// 	}
// 	OUT<<ENDL;
// #endif
// }

// void operator >> (const YAML::Node& node, Array<scalar>& s)
// {
// 	String mea;
// 	node["mea"] >> mea;
// 	s.push_back(std::atof(mea.c_str()));
// #ifdef DEBUG_YAML
// 	for (int i = 0; i < s.size(); ++i)
// 	{
// 		OUT<<s[i]<<", ";
// 	}
// 	OUT<<ENDL;
// #endif
// }

// void operator >> (const YAML::Node& node, Array<Array<label> >& nei)
// {
// 	// String neighbor;
// 	// node["neighbor"].as<string>() >> neighbor;
// 	Array<label> neiTmp;
// 	int tmp;
// 	const YAML::Node& neighbor = node["neighbor"];
// 	for (int i = 0; i < neighbor.size(); ++i)
// 	{
// 		neighbor[i] >> tmp;
// 		neiTmp.push_back(tmp);
// 	}
// 	nei.push_back(neiTmp);

// #ifdef DEBUG_YAML
// 	for (int i = 0; i < nei.size(); ++i)
// 	{
// 		OUT<<"(";
// 		for (int j = 0; j < nei[i].size(); ++j)
// 		{
// 			OUT<<nei[i][j]<<", ";
// 		}
// 		OUT<<")";
// 	}
// 	OUT<<ENDL;
// #endif
// }

// void operator >> (const YAML::Node& node, Array<label>& regionIdx)
// {
// 	label tmp;
// 	node["regionIdx"] >> tmp;
// 	regionIdx.push_back(tmp);

// #ifdef DEBUG_YAML
// 	for (int i = 0; i < regionIdx.size(); ++i)
// 	{
// 		OUT<<regionIdx[i]<<", ";
// 	}
// 	OUT<<ENDL;
// #endif
// }

// void hdf5ToAdf(char* filePtr, char* desFilePtr)
// {
// 	int iFile;
// 	if(cg_open(filePtr, CG_MODE_MODIFY, &iFile))
// 		Terminate("readCGNSFile", cg_get_error());
// 	if(cg_save_as(iFile,  desFilePtr, CG_FILE_ADF2, 0))
// 		Terminate("transformToADF", cg_get_error());
// }