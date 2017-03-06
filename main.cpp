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
    // I/O handling
    int iterations = 0;
    if (argc >= 2) {

        std::istringstream iss{argv[1]};
        int n;
        if (iss >> n) {
            iterations = n;
        }
    } else {
        iterations = 5;
    }

    // Choose mission:
    namespace MISSION = MARSMISSION;

    planets.push_back(MISSION::sun);
    planets.push_back(MISSION::earth);
    planets.push_back(MISSION::moon);
    planets.push_back(MISSION::mars);
    satellites.push_back(MISSION::spacecraft);

    // Cost function for Mars mission
    auto distanceToMars = [=](vec x)->double{
        mat plan = reshape(x,3,x.n_rows/3);
        vec tt  = linspace(0,1,33000);
        plan = interpDesign<3>(plan,tt);
        double dt = 315.0;
        Universe universe(planets,satellites,plan,dt);
        universe.evolve();
        double distance = sqrt(norm(universe.getPlanets().at(3).getPosition()-universe.getSatellites().at(0).getPosition()));
        double r_mars = 3390000;
        return std::abs(distance - r_mars);
    };

    // initialguess
      vec initialguess = MISSION::initialDesign;
      initialguess = vec{128,fill::zeros};

    // trial
      double distance = distanceToMars(initialguess);
      cout << "Distance to Mars: " << distance << endl << endl;

    // Optimization of thrust plan
      int     N_iterations     = 1e4;
      double  tolerance        = 1e-3; // [km]
      double  initialStepSize  = 30.0;

      Member solution = Cmaes(distanceToMars,initialguess,N_iterations,tolerance,initialStepSize);

      cout << "Optimized distance to Mars: " << solution.cost << endl;
      cout << "Optimized design: " << endl << solution.design << endl;

    return 0;
}
