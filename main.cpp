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

    vec x = vec(128,fill::zeros);
    vec tt  = linspace(0,1,33000);
    double dt = 21859200/33000;
    mat plan = interpDesign<3>(x,tt);
    //satellites.at(0).setSpeed(satellites.at(0).getSpeed());
    Universe<true> universe(planets,satellites,plan,dt,"Data/MissionAt" + std::to_string(0) + ".dat");
    universe.evolve();

    std::vector<Member> sweeps;
    auto optimizeForDuration = [=,&sweeps](double T, int index)->void{
      // Cost function for Mars mission with landing survivable
      double dt = T/33000.0;

      satellites.at(0).setSpeed(satellites.at(0).getSpeed()/dt);
      auto distanceToMarsWithLandingAndFuel = [=](vec x)->double{
          //x/=max(x)*50*dt;
          // Convert design vector into matrix format ~ interpolate over ~ 4 months
          vec tt  = linspace(0,1,33000);
          mat plan = interpDesign<3>(x,tt);


          // Create universe
          Universe<false> universe(planets,satellites,plan,dt);

          // evolve universe
          universe.evolve();

          // Fetch end result ~ relative position and velocity
          Satellite sat = universe.getSatellites().at(0);
          double distance           = sqrt(norm(universe.getPlanets().at(3).getPosition()-sat.getPosition()));
          double relativeVelocity   = sqrt(norm(universe.getPlanets().at(3).getSpeed()-sat.getSpeed()));
          double fuelUsage          = 1e1*sum(abs(x));
          // return distance to Martian surface + relative velocity as the cost to be minimzed
          double r_mars = 3390; // [km]
          double sunPenalty = 0;
          if(sat.hasBeenCloser) sunPenalty = 1e20;
          return (std::abs(distance - r_mars)+relativeVelocity) + fuelUsage + sunPenalty;
      };

      // initialguess
        vec initialguess = vec{128,fill::zeros};

        //initialguess = optDesign;
        //initialguess = MISSION::initialguess;

      // trial
        double distance = distanceToMarsWithLandingAndFuel(initialguess);
        cout << "Cost of design: \t" << distance << endl << endl;

      // Optimization parameters
        int     N_iterations     = 0.6*1e4;
        double  tolerance        = 1e-3; // [m]
        double  initialStepSize  = 30.0; // [m/s/dt]

      // The actual optimization
        Member solution = Cmaes(distanceToMarsWithLandingAndFuel,initialguess,N_iterations,tolerance,initialStepSize);
        sweeps.push_back(solution);

      // Result from optimization
        cout << "Optimized cost: \t" << solution.cost << endl;
        cout << "Optimized design: \t" << endl;
        cout << "vec optDesign = {";
        size_t i;
        for (i = 0; i < solution.design.n_rows-1; i++) {
          cout << solution.design.at(i) << ",";
        }
        cout << solution.design.at(i)<<"};" << endl;

        //set initialparams
        vec tt  = linspace(0,1,33000);
        mat plan = interpDesign<3>(solution.design,tt);

        // Creating new universe ~ and getfile
        Universe<true> universe(planets,satellites,plan,dt,"Data/MissionAt" + std::to_string(index) + ".dat");
        // evolve universe

        universe.evolve();
    };

    double T1 = 10395000; // 120 days
    double T2 = 21859200; // 253 days ~ duration of mission
    vec times = linspace(T1,T2,100);

    for (size_t i = 0; i < times.n_rows; i++) {
      cout << "Main: optimizing for duration: "<< times(i) << ", at index: " << i+1 << " of " << times.n_rows << endl;
      optimizeForDuration(times(i),i+1);
    }

    return 0;
}
