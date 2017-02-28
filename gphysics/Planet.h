//
// Created by christian on 10/02/17.
//

#ifndef ORBIT_PLANET_H
#define ORBIT_PLANET_H


#include <armadillo>

using namespace arma;

class Planet {
public:
    Planet(vec p0, vec v0, double mu0):
        position(p0),
        speed(v0),
        mu(mu0)
    {
    }
    vec getPosition(){
        return position;
    }
    vec getSpeed(){return  speed;}
    void setSpeed(vec boost){
        speed = boost;
    }

    void setPosition(vec newPosition){
        position = newPosition;
    }
    double getMu(){ return mu; }

private:
    vec position{3};
    vec speed{3};
    double mu = 1.0;
};


#endif //ORBIT_PLANET_H
