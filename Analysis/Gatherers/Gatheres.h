#include <armadillo>
#include "../../gphysics.h"

class Gatherer{
  void init();
  void collect();
  void closeUp();
}

class writeToTerminalGatherer: public Gatherer{
public:
  void init(int n_timeSteps,std::string fName = "Data/universe.dat"){
    double Nd_data = 2000;
    if (Nd_data>n_timeSteps) Nd_data=n_timeSteps;
    int N_data = int(Nd_data);
    dataContainer = dataContainer[N_data];
    writingRate = n_timeSteps/N_data;
  }
private:
  double writingRate;
  std::string dataContainer[];
  std::string dataPoint;
}
