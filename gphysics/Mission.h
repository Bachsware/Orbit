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

    // Create satellites and planets
    vec r_sat = {8567.0,0.0,0.0};
    vec v_sat = {0.0,6.846,3.717};
    Satellite spacecraft{r_sat*1000.0,v_sat*1000.0,1.0,"Bach"};

    /* EARTH at mission launch
     *  we use geocentric system
     */
    vec r_earth = {0.0,0.0,0.0};
    vec v_earth = {0.0,0.0,0.0};
    Planet earth{r_earth*positionScale,v_earth*positionScale,mu_earth};

    /* SUN at mission launch
        -6.511244251882641E+07 -1.325155825916022E+08  4.038490822128952E+03
        2.723334501912125E+01 -1.302770707957643E+01  1.520335159217900E-03
    */
    vec r_sun = {-6.511244251882641E+07, -1.325155825916022E+08,  4.038490822128952E+03};
    vec v_sun = {2.723334501912125E+01, -1.302770707957643E+01,  1.520335159217900E-03};
    Planet sun{r_sun*positionScale,v_sun*positionScale,mu_sun};

    /* MARS at mission launch
      -1.877516144214960E+08  7.879417504795945E+07  7.442923429344438E+06
       7.195907630755880E+00 -2.312904115480940E+01  2.818634773147277E-01
     */
    vec r_mars = {-1.877516144214960E+08,  7.879417504795945E+07,  7.442923429344438E+06};
    vec v_mars = {7.195907630755880E+00, -2.312904115480940E+01,  2.818634773147277E-01};
    Planet mars{r_mars*positionScale,v_mars*positionScale,mu_mars};

    /* MOON at mission launch
      -4.731121882357229E+04 -3.626666887314772E+05  5.003714921506587E+03
       1.056246398805415E+00 -1.876016846394916E-01  9.874168442103273E-02
     */
    vec r_moon = {-4.731121882357229E+04, -3.626666887314772E+05, 5.003714921506587E+03};
    vec v_moon = {1.056246398805415E+00, -1.876016846394916E-01,  9.874168442103273E-02};
    Planet moon{r_moon*positionScale,v_moon*positionScale,mu_moon};

}

#endif //ORBIT_MISSION_H
