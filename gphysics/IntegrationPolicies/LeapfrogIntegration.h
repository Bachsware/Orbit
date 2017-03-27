#pragma once
#include "../Planet.h"
class LeapfrogIntegration{
public:
  static void applyGravity(std::vector<Planet>& planets,vec & a, vec& r, vec& v,double dt){

    vec aj = {0,0,0};
    for (int k = 0; k < planets.size(); ++k) {
        Planet pi = planets.at(k);
        vec relativePosition = pi.getPosition()-r; // x_i
        double distance = norm(relativePosition);
        if (distance>0.0){ // Do not accelerate on self-interaction
            aj += pi.getMu()/pow(distance,2)*normalise(relativePosition);
        }
    }
    r += v*dt + 0.5*a*dt*dt;
    v += 0.5*(a+aj)*dt;
    a = aj;
  }
};
