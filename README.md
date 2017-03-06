# Orbit
0. Orbit is a simulation tool that enables the client to plan missions to other planets launched from earth.
 - This is done in the scope of optimal control theory using optimization algorithms to determine the thruster activities of the spacecraft
 - The software is cross-platform and is able to run on clusters as well.
 - For optimization the default procedure is CMAES (Covariance Matrix Adaption Evolutionary Strategy)

1. Setup Dependencies:
 - sudo apt-get install libarmadillo-dev

2. Build
 - cd into rep
 - cmake .
 - make Orbit
 - ./Orbit

3. Look at data
  - run matlab-script: plotSimple.m
