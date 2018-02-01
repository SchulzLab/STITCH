#ifndef BINSELECTION_H
#define BINSELECTION_H

#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <map>
#include <tuple>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>


//IMPORTANT: The bigwig lib uses half open intervalls, e.g. [A,B)

class BinSelection{
	public:
	BinSelection(const std::string& path,std::map<std::string,double>& expressionMap);
	void computeMeanSignal(std::string& chrom, const std::vector<std::pair<unsigned int, unsigned int> >& segments);
	std::vector<std::vector<double> >& getMeanSignal();
	std::vector<std::string>& getSampleNames();
	std::vector<std::pair<double, double> > computePearsonCorrelation();
	std::vector<std::pair<double, double> > computeSpearmanCorrelation();
	std::vector<double> getExpressionVectorByNames();
	
	std::vector<double> getSignalVectorBySegment(unsigned int segID);
          friend std::ostream& operator<<(std::ostream& os,const BinSelection& r);




	private:
	const std::string path_;
	std::map<std::string,double>& expressionMap_;
	const std::vector<std::pair<unsigned int, unsigned int> > segmentation;
	std::vector<std::string> sampleNames_;
	std::vector<std::vector<double> > meanSignal_;	
};

#endif
