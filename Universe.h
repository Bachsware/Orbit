//
// Created by christian on 13/02/17.
//

#ifndef ORBIT_UNIVERSE_H
#define ORBIT_UNIVERSE_H

#include <armadillo>
#include <iostream>

#include <iostream>
#include <fstream>

#include "Planet.h"
#include "Satellite.h"

using namespace arma;

class Universe {
public:
    Universe(std::vector<Planet> planets, std::vector<Satellite> satellites, double dt, std::string fileName = "universe.dat")
            : planets(planets),
              satellites(satellites),
            dt(dt)
    {
        data.open(fileName);
    }
    void evolve(int n_timeSteps){
        double time = 0, counter = 0.0; int dataIndex = 0;
        double Nd_data = 2000;
        if (Nd_data>n_timeSteps) Nd_data=n_timeSteps;
        int N_data = int(Nd_data);
        std::string dataContainer[N_data];
        double writingRate = n_timeSteps/N_data;
        std::string dataPoint;
        for (int n = 0; n < n_timeSteps; ++n) {

            double progress = floor(double(n+1)/double(n_timeSteps)*1000.0)/10.0;
            bool timeToWrite = writingRate<=counter;

            if (timeToWrite) dataPoint = std::to_string(time+dt) + " ";

            // Update satellites
            for (int j = 0; j < satellites.size(); ++j) {
                Satellite si = satellites.at(j);

                // Old coordinates
                vec r = si.getPosition();
                vec v = si.getSpeed();

                applyGravity(r,v,dt);

                // New coordinates
                si.setSpeed(v);
                si.setPosition(r);

                satellites.at(j) = si;
                if (timeToWrite) dataPoint += std::to_string(r(0))+" "+std::to_string(r(1))+" "+std::to_string(r(2))+" ";
            }

            // Update planets
            for (int j = 0; j < planets.size(); ++j) {
                Planet pi = planets.at(j);

                // Old coordinates
                vec r = pi.getPosition();
                vec v = pi.getSpeed();

                applyGravity(r,v,dt);

                // New coordinates
                pi.setSpeed(v);
                pi.setPosition(r);
                planets.at(j) = pi;
                if (timeToWrite) dataPoint += std::to_string(r(0))+" "+std::to_string(r(1))+" "+std::to_string(r(2))+" ";
            }
            time += dt;
            if (timeToWrite){
                cout << "Progress: "<< progress << " %, index: " << dataIndex+1 <<  endl;
                dataContainer[dataIndex]=dataPoint;
                counter = 1; dataIndex++;
            } else{
                counter++;
            }
        }
        for (int i = 0; i < N_data; ++i) {
            data << dataContainer[i] << endl;
        }
        data.close();
    }
    std::vector<Satellite> getSatellites(){
        return satellites;
    }
    std::vector<Planet> getPlanets(){
        return planets;
    }
    std::ofstream data;
private:

    void applyGravity(vec& r, vec& v,double dt){
        vec acceleration = {0,0,0};
        for (int k = 0; k < planets.size(); ++k) {
            Planet pi = planets.at(k);
            vec relativePosition = pi.getPosition()-r; // x_i
            double distance = norm(relativePosition);
            if (distance>0.0){ // Do not accelerate on self-interaction
                acceleration += pi.getMu()/pow(distance,2)*normalise(relativePosition);
            }
        }
        v += acceleration*dt;
        r += v*dt; // dr = v*dt + 1/2*a*dt^2
    }

    std::vector<Planet> planets;
    std::vector<Satellite> satellites;
    double dt = 1.0;
};


#endif //ORBIT_UNIVERSE_H
