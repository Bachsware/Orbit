//
// Created by christian on 28/02/17.
//

#ifndef ORBIT_MISSION_H
#define ORBIT_MISSION_H

#include <iostream>

#include "Satellite.h"
#include "Planet.h"


namespace MARSMISSION {
    double positionScale = 1000.0;
    double timeScale = 3.0;

    double mu_earth = 398600441800000.0;
    double mu_sun   = 1.327124*1e20;
    double mu_mars   = mu_earth*0.107;
    double mu_moon  = 4.902798882*1e12;

    vec r_earth = {0.0,0.0,0.0};
    vec v_earth = {0.0,0.0,0.0};
    Planet earth{r_earth*positionScale,v_earth*positionScale,mu_earth};

    vec r_mars = {-1.877516144214960E+08,  7.879417504795945E+07,  7.442923429344438E+06};
    vec v_mars = {7.195907630755880E+00, -2.312904115480940E+01,  2.818634773147277E-01};
    Planet mars{r_mars*positionScale,v_mars*positionScale,mu_mars};

    vec unitDirectionEarth2Mars = (r_mars-r_earth)/norm(r_mars-r_earth);
    vec temp = {1.0,1.0,1.0};
    double scale = dot(temp/sqrt(3),unitDirectionEarth2Mars);
    vec scaledTemp =unitDirectionEarth2Mars*scale;
    vec toMars = unitDirectionEarth2Mars;
    vec perpToMars = (temp-scaledTemp)/norm(temp-scaledTemp);
    double velocity = 1.0, distance = 35786.0; //6.9; // 2.7564
    Satellite spacecraft{distance*toMars*positionScale,toMars*velocity*positionScale,12.0,"Bach"};

    vec r_sun = {-6.511244251882641E+07, -1.325155825916022E+08,  4.038490822128952E+03};
    vec v_sun = {2.723334501912125E+01, -1.302770707957643E+01,  1.520335159217900E-03};
    Planet sun{r_sun*positionScale,v_sun*positionScale,mu_sun};

    vec r_moon = {-4.731121882357229E+04, -3.626666887314772E+05, 5.003714921506587E+03};
    vec v_moon = {1.056246398805415E+00, -1.876016846394916E-01,  9.874168442103273E-02};
    Planet moon{r_moon*positionScale,v_moon*positionScale,mu_moon};
}
namespace LaunchToGeostationary {
  using namespace MARSMISSION;
}
#endif //ORBIT_MISSION_H
