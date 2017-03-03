#include <iostream>
#include <sstream>
#include <armadillo>

#include "gphysics/Universe.h"
#include "gphysics/Mission.h"

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


    // Simulate mission:
    //Universe universe{planets,satellites,MISSION::timeScale};
    //universe.evolve(zeros(3,1000));

    auto distanceToMars = [=](vec x)->double{
        mat plan = reshape(x,3,x.n_rows/3);
        Universe universe(planets,satellites,plan,3.0);
        universe.evolve();
        double distance = norm(universe.getPlanets().at(3).getPosition()-universe.getSatellites().at(0).getPosition());
        return distance;
    };

    double distance = distanceToMars(vec{300000,fill::zeros});
    cout << "Distance to Mars: " << distance << endl;

    Member solution = Cmaes(distanceToMars,vec{30,fill::zeros},1e4,1e6);

    cout << "Optimized distance to Mars: " << solution.cost << endl;

    return 0;
}