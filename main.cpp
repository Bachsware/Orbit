#include <iostream>
#include <sstream>
#include <armadillo>

#include "gphysics/Universe.h"
#include "gphysics/Mission.h"

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

    mat thrustPlan = MISSION::thrustPlan;

    // Simulate mission:
    Universe universe{planets,satellites,thrustPlan,MISSION::timeScale};
    universe.evolve(iterations);

    return 0;
}