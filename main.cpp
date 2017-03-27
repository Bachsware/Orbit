#include <iostream>
#include <sstream>
#include <armadillo>

#include "gphysics/Universe.h"
#include "gphysics/Mission.h"
#include "gphysics/Utilities.h"
#include "gphysics/IntegrationPolicies/LeapfrogIntegration.h"
#include "gphysics/IntegrationPolicies/EulerMethodIntegration.h"
#include "Optimization/Cmaes.h"

using namespace arma;

std::vector<Planet> planets;
std::vector<Satellite> satellites;
void LaunchGeostationary(){
  // Choose mission:
  namespace MISSION = MARSMISSION;

  planets.push_back(MISSION::sun);
  planets.push_back(MISSION::earth);
  planets.push_back(MISSION::moon);
  planets.push_back(MISSION::mars);
  satellites.push_back(MISSION::spacecraft);
  satellites.at(0).speed *=(3.4945+2.6024+0.5); // Hohmann initial burn + GeoStationarySpeed

  vec x = vec(128,fill::zeros);
  vec tt  = linspace(0,1,33000);
  //double T2 = 21859200; // 253 days ~ duration of mission

  double dt = 24*3600*256/33000;


  mat plan = interpDesign<3>(x,tt);
  std::vector<Planet> pl2 = planets;
  std::vector<Satellite> st2 = satellites;

  Universe<EulerMethodIntegration,true> eulerUniverse(planets,satellites,plan,dt,"Data/EulerGeoStationary.dat");
  eulerUniverse.evolve();
  Universe<LeapfrogIntegration,true> frogUniverse(pl2,st2,plan,dt,"Data/LeapGeoStationary.dat");
  frogUniverse.evolve();
}

Member optimizeForDuration(std::vector<Satellite> satellites, std::vector<Planet> planets,vec initialguess, double T, int index){
  // Cost function for Mars mission with landing survivable
  double dt = T/33000.0;

  auto distanceToMarsWithLandingAndFuel = [=](vec x)->double{
      // Convert design vector into matrix format
      vec tt  = linspace(0,1,33000); mat plan = interpDesign<3>(x*50.0,tt);

      // Create universe
      Universe<EulerMethodIntegration,false> universe(planets,satellites,plan,dt);

      // evolve universe
      universe.evolve();

      // Fetch end result ~ relative position and velocity
      Satellite sat                = universe.getSatellites().at(0);
      Planet    mars               = universe.getPlanets().at(3);
      double    r_mars             = 3390; // [km]

      double    distance           = std::abs(sqrt(norm(mars.getPosition()-sat.getPosition())) - r_mars);
      double    relativeVelocity   = sqrt(norm(mars.getSpeed()-sat.getSpeed()));
      double    fuelUsage          = 1e2*sum(abs(x));

      return distance + relativeVelocity + fuelUsage;
  };

  // Optimization parameters
    int     N_iterations     = 0.6*1e4;
    double  tolerance        = 1e-3; // [m]
    double  initialStepSize  = 30; // [m/s/dt]

  // The actual optimization
    Member solution = Cmaes(distanceToMarsWithLandingAndFuel,initialguess,N_iterations,tolerance,initialStepSize);

    vec tt  = linspace(0,1,33000); mat plan = interpDesign<3>(solution.design*50.0,tt);
    Universe<EulerMethodIntegration,true> eulerUniverse(planets,satellites,plan,dt,"Data/mission_" + std::to_string(index) + ".dat");
    eulerUniverse.evolve();
    return solution;
}
int main(int argc, char* argv[]) {
    //LaunchGeostationary();
    //return 0;
    namespace MISSION = MARSMISSION;

    planets.push_back(MISSION::sun);
    planets.push_back(MISSION::earth);
    planets.push_back(MISSION::moon);
    planets.push_back(MISSION::mars);
    satellites.push_back(MISSION::spacecraft);
    satellites.at(0).speed *= (3.4945+2.6024+0.5);

    vec initialguess = vec{128,fill::zeros};


    double T1 = 10395000; // 120 days
    double T2 = 21859200; // 253 days ~ duration of mission
    vec times = linspace(T2,T1,3);

    Member solution;
    solution.design = initialguess;
    solution.cost   = 1e90;

    for (size_t i = 0; i < times.n_rows; i++) {
      cout << "Main: optimizing for duration: "<< times(i) << ", at index: " << i+1 << " of " << times.n_rows << endl;
      solution = optimizeForDuration(satellites,planets,solution.design,times(i),i+1);
    }

    return 0;
}
