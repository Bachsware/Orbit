#ifndef ORBIT_SATELLITE_H
#define ORBIT_SATELLITE_H

#include <armadillo>

using namespace arma;

class Satellite {
public:
    Satellite(vec p0, vec v0, double mu0, std::string name0="noName"):
            position(p0),
            speed(v0),
            mu(mu0),
            name(name0)
    {
    }

    vec getPosition(){return position;}
    vec getSpeed(){return  speed;}

    bool hasBeenCloser = false;
    void dontIkaroz(vec rSun, vec rEarth){
        double myDistance = sqrt(norm(position-rSun));
        double earthDistance = sqrt(norm(position-rEarth));
        if(hasBeenCloser==false){
          if(myDistance<earthDistance*0.75){
            hasBeenCloser = true;
          }
        }
    }
    void setSpeed(vec boost){
        speed = boost;
    }

    void setPosition(vec newPosition){
        position = newPosition;
    }
    void toString(){
        cout << "Name: " << name << ", Velocity: "<< norm(getSpeed())<< endl;
    };

private:
    std::string name;
    vec position;
    vec speed;
    double mu = 1.0;
};


#endif //ORBIT_SATELLITE_H
