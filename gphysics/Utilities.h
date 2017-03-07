//
// Created by christian on 06/03/17.
//

#ifndef ORBIT_UTILITIES_H
#define ORBIT_UTILITIES_H

#include <armadillo>

using namespace arma;

template<unsigned int paramSize>
mat interpDesign(const vec & x, vec& tt){
    mat plan = reshape(x,3,x.n_rows/3);
    int N_interpPoints = tt.n_rows;
    mat design{paramSize,N_interpPoints,fill::zeros};
    vec t   = linspace(0,1,plan.n_cols);
    for (int i = 0; i < paramSize; ++i) {
        vec tempVec{N_interpPoints,fill::zeros};
        interp1(t,plan.row(i).t(),tt,tempVec,"*nearest");
        design.row(i)= tempVec.t();
    }
    return design;
}

vec thrustMatToDesign(const mat & thrustPlan){
  int N = thrustPlan.n_cols*thrustPlan.n_rows;
  vec design{N,fill::zeros};
  int j = 0;
  for (size_t i = 0; i < N; i+=3) {
    design(i) = thrustPlan(i,j);
    design(i+1) = thrustPlan(i+1,j);
    design(i+2) = thrustPlan(i+2,j);
    j++;
  }
}

#endif //ORBIT_UTILITIES_H
