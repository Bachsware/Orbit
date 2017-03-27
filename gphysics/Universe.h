//
// Created by christian on 13/02/17.
//

#ifndef ORBIT_UNIVERSE_H
#define ORBIT_UNIVERSE_H

#include <armadillo>
#include <iostream>
#include <fstream>

#include "Planet.h"
#include "Satellite.h"

using namespace arma;
template <class Integrator, bool collect = false>
class Universe {
public:
    Universe(std::vector<Planet> planets, std::vector<Satellite> satellites,mat thrustPlan , double dt, std::string fileName = "Data/universe.dat")
            : planets(planets),
              satellites(satellites),
              dt(dt*3.0/4.0),
              thrustPlan(thrustPlan),
              fileName(fileName)
    {

    }
    void evolve(){
        double time = 0, counter = 0.0;

        int dataIndex = 0;
        // na=3/4*N, 3/4
        double n_timeSteps = thrustPlan.n_cols*(4.0/3.0);
        double Nd_data = 2000;
        if (Nd_data>n_timeSteps) Nd_data=n_timeSteps;
        int N_data = int(Nd_data);
        std::string dataContainer[N_data];
        std::string dataPoint;
        double writingRate = n_timeSteps/N_data;
        double progress;
        bool timeToWrite;
        if (collect) {cout << "-- Collecting data" << endl;}
        for (int n = 0; n < n_timeSteps; ++n) {

            if(collect){ // Determined on compile time
              progress = floor(double(n+1)/double(n_timeSteps)*1000.0)/10.0;
              timeToWrite = writingRate<=counter || n==0;
              if (timeToWrite) dataPoint = std::to_string(time+dt) + " ";
            }

            // Update satellites
            for (int j = 0; j < satellites.size(); ++j) {
                Satellite si = satellites.at(j);

                // Old coordinates
                vec r = si.getPosition();
                vec v = si.getSpeed();
                vec a = si.getAcceleration();

                //si.dontIkaroz(planets.at(0).getPosition(),planets.at(1).getPosition());
                if (collect && timeToWrite) dataPoint += std::to_string(r(0))+" "+std::to_string(r(1))+" "+std::to_string(r(2))+" ";
                Integrator::applyGravity(planets,a,r,v,dt);

                // New coordinates ~ adjust after half time
                vec immediateThrust = {0,0,0};
                int N_adjust = floor(n_timeSteps/4.0);
                if (n>=N_adjust){immediateThrust = thrustPlan.col(n-N_adjust);}

                si.setSpeed(v+immediateThrust);
                si.setPosition(r);
                si.setAcceleration(a);

                satellites.at(j) = si;
            }

            // Update planets
            for (int j = 0; j < planets.size(); ++j) {
                Planet pi = planets.at(j);

                // Old coordinates
                vec r = pi.getPosition();
                vec v = pi.getSpeed();
                vec a = pi.getAcceleration();

                // Collect data ~ position
                if (collect && timeToWrite) dataPoint += std::to_string(r(0))+" "+std::to_string(r(1))+" "+std::to_string(r(2))+" ";

                // Integrate
                Integrator::applyGravity(planets,a,r,v,dt);

                // New coordinates + thrust
                pi.setSpeed(v);
                pi.setPosition(r);
                pi.setAcceleration(a);
                planets.at(j) = pi;
            }
            time += dt;
            if (collect && timeToWrite){
                //cout << "Progress: "<< progress << " %, index: " << dataIndex+1 <<  endl;
                dataContainer[dataIndex]=dataPoint;
              counter = 1; dataIndex++;
            } else if(collect){
                counter++;
            }
        }
        if(collect){
          data.open(fileName);
          for (int i = 0; i < N_data; ++i) {
              data << dataContainer[i] << endl;
          }
          data.close();
        }
    }
    std::vector<Satellite> getSatellites(){
        return satellites;
    }

    std::vector<Planet> getPlanets(){
        return planets;
    }

    std::ofstream data;
private:
    std::string fileName;
    mat thrustPlan;
    std::vector<Planet> planets;
    std::vector<Satellite> satellites;
    double dt = 1.0;
};

#endif //ORBIT_UNIVERSE_H
