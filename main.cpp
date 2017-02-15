#include <iostream>
#include <sstream>
#include <armadillo>
#include "Universe.h"
using namespace arma;

std::vector<Planet> planets;
std::vector<Satellite> satellites;
long double PI = 3.14159265359;

int main(int argc, char* argv[]) {
    int iterations = 0;
    if(argc >= 2){

        std::istringstream iss{argv[1]};
        int n;
        if(iss>>n){
            iterations = n;
        }
    } else {
        iterations = 5;
    }
    double mu_earth = 398600441800000.0;
    double mu_sun   = 1.327124*1e20;
    double mu_mars   = mu_earth*0.107;
    double mu_moon  = 4.902798882*1e12;

    // Create satellites and planets
    vec r_sat = {8567.0,0.0,0.0};
    vec v_sat = {0.0,6.846,3.717};
    //v_sat = {-100.0,5500.0,3500.0};
    Satellite spacecraft{r_sat*1000.0,v_sat*1000.0,1.0,"Bach"};
    satellites.push_back(spacecraft);

    vec r_sc2 = {9267.0,0.0,300.0};
    vec v_sc2 = {0.0,-5.846,3.717};
    Satellite sc2{r_sc2*1000,v_sc2*1000,1.0,"Katrine"};
    satellites.push_back(sc2);

    vec r_earth = {0.0,20000.0,0.0};
    vec v_earth = {0.0,0.0,0.0};
    Planet earth{r_earth,v_earth,mu_earth};
    planets.push_back(earth);

    Planet moon{{380000000,0,0},{0,1000.0,0},mu_moon};
    planets.push_back(moon);

    // Create universe and do simulation
    Universe universe{planets,satellites,3.0};
    universe.evolve(iterations);


    return 0;
}