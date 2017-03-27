#pragma once
#include "../Planet.h"
class EulerMethodIntegration{
public:
  static void applyGravity(std::vector<Planet>& planets,vec & a, vec& r, vec& v,double dt){
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
};
