#include "../BigWig/bigWig.h"
#include "BinSelection.h"
#include "CorComp.h"

BinSelection::BinSelection(const std::string& path)
	:path_(path)
{
	meanSignal_.resize(0);

}



//IMPORTANT NOTE: This method transforms the first coordinate to the 0-based index used in bigWig.h. As bigWig.h works with half open intervalls, but SPAN provides closed intervalls, the second coordinate is not changed.
void BinSelection::computeMeanSignal(std::string& chrom, const std::vector<std::pair<unsigned int, unsigned int> >& segments){
	 //Generating per base input for SPAN
	if(bwInit(1<<17) != 0) throw std::runtime_error("Error occured in bwInit.");
	//Iterating through all files in the specified directory
	if (boost::filesystem::is_directory(path_)){
		for(auto& entry : boost::make_iterator_range(boost::filesystem::directory_iterator(path_), {})){
			double *stats = NULL;
			bigWigFile_t *fp = NULL;
			std::string filenameS = entry.path().string();
			char * filename= new char[filenameS.size() +1];
			char * chromC= new char[chrom.size()+1];
			std::copy(filenameS.begin(),filenameS.end(), filename);
			std::copy(chrom.begin(),chrom.end(), chromC);
			filename[filenameS.size()]='\0';
			chromC[chrom.size()]='\0';
			if (entry.path().extension().string() != ".bw") throw std::invalid_argument(filenameS+" is not a biwWig file. Expected file type is .bw");
			//Check for presence of expression data for the current sample, otherwise ignore the sample!
			std::vector<double> currentSample;
			fp = bwOpen(filename, NULL, "r");
			if(!fp) {
				throw std::invalid_argument("An error occured while opening the bw files");
			}
			//Get an example statistic - standard deviation
			//We want ~1 bins in the range
			sampleNames_.push_back(entry.path().stem().string());
			for (auto& seg : segments){
			stats = bwStats(fp, chromC, seg.first-1, seg.second, 1, mean);
			if(stats) {
					currentSample.push_back(stats[0]);
				}
			}
			free(stats);
			bwClose(fp);
			meanSignal_.push_back(currentSample);
		}
	}
	bwCleanup();
}


std::vector<double> BinSelection::getExpressionVectorByNames(std::map<std::string, double>& expressionMap){
	std::vector<double> expVecTemp;
	for (auto& sample : sampleNames_){
		expVecTemp.push_back(expressionMap[sample]);
	}
	return expVecTemp;
}

std::vector<double> BinSelection::getSignalVectorBySegment(unsigned int segID){
	std::vector<double> segmentSignal;
	for (unsigned int sam = 0; sam < meanSignal_.size(); sam++){
		segmentSignal.push_back(meanSignal_[sam][segID]);
	}			
	return segmentSignal;
}

std::vector<double> BinSelection::computeCorrelation(std::map<std::string, double>& expressionMap){
	std::vector<double> correlation;
	unsigned int numSeg = meanSignal_[0].size();
	std::vector<double> expVec=getExpressionVectorByNames(expressionMap);
	for (unsigned int seg = 0; seg < numSeg; seg++){
		std::vector<double> signalVec = getSignalVectorBySegment(seg);
		CorComp cC(expVec,signalVec);
		correlation.push_back(cC.computePearsonCorrelation());
	}
	return correlation;
}

std::vector<std::vector<double> >& BinSelection::getMeanSignal(){
	return meanSignal_;
}

std::vector<std::string>& BinSelection::getSampleNames(){
	return sampleNames_;
}