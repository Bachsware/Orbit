#include <iostream>
#include <sstream>
#include <armadillo>

#include "gphysics/Universe.h"
#include "gphysics/Mission.h"
#include "gphysics/Utilities.h"

#include "Optimization/Cmaes.h"

using namespace arma;

std::vector<Planet> planets;
std::vector<Satellite> satellites;

int main(int argc, char* argv[]) {
    // Choose mission:
    namespace MISSION = MARSMISSION;

    planets.push_back(MISSION::sun);
    planets.push_back(MISSION::earth);
    planets.push_back(MISSION::moon);
    planets.push_back(MISSION::mars);

    satellites.push_back(MISSION::spacecraft);

    double dt = 315.0; // [s]


    // Cost function for Mars mission with landing survivable
    auto distanceToMarsWithLanding = [=](vec x)->double{
        //x/=max(x)*50*dt;
        // Convert design vector into matrix format ~ interpolate over ~ 4 months
        vec tt  = linspace(0,1,33000);
        mat plan = interpDesign<3>(x,tt);

        // Create universe
        Universe<false> universe(planets,satellites,plan,dt);

        // evolve universe
        universe.evolve();

        // Fetch end result ~ relative position and velocity
        double distance = sqrt(norm(universe.getPlanets().at(3).getPosition()-universe.getSatellites().at(0).getPosition()));
        double relativeVelocity = sqrt(norm(universe.getPlanets().at(3).getSpeed()-universe.getSatellites().at(0).getSpeed()));
        double fuelUsage = sum(x);
        // return distance to Martian surface + relative velocity as the cost to be minimzed
        double r_mars = 3390; // [km]

        return (std::abs(distance - r_mars)+relativeVelocity);
    };

    // initialguess
      vec initialguess = vec{128,fill::zeros};
      //initialguess = MISSION::initialguess;
    // trial
      double distance = distanceToMarsWithLanding(initialguess);
      cout << "Cost of design: \t" << distance << endl << endl;

    // Optimization of thrust plan

      // Optimization parameters
      int     N_iterations     = 1e4;
      double  tolerance        = 1e-3; // [m]
      double  initialStepSize  = 30.0; // [m/s/dt]

      // The actual optimization
      Member solution = Cmaes(distanceToMarsWithLanding,initialguess,N_iterations,tolerance,initialStepSize);

      // Result from optimization
      cout << "Optimized cost: \t" << solution.cost << endl;
      cout << "Optimized design: \t" << endl << solution.design.t() << endl;

      //set initialparams
      vec tt  = linspace(0,1,33000);
      mat plan = interpDesign<3>(solution.design,tt);

      // Creating new universe ~ and getfile
      Universe<true> universe(planets,satellites,plan,dt,"Data/OptimizedSpacecraft4.dat");
      // evolve universe
      universe.evolve();

    return 0;
}
