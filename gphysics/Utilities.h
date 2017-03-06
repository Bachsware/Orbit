//
// Created by christian on 06/03/17.
//

#ifndef ORBIT_UTILITIES_H
#define ORBIT_UTILITIES_H

#include <armadillo>

using namespace arma;

template<unsigned int paramSize>
mat interpDesign(const mat & reducedDesign, vec& tt){
    int N_interpPoints = tt.n_rows;
    mat design{paramSize,N_interpPoints,fill::zeros};
    vec t   = linspace(0,1,reducedDesign.n_cols);
    for (int i = 0; i < paramSize; ++i) {
        vec tempVec{N_interpPoints,fill::zeros};
        //        cout  << "rdc/tr: " << reducedDesign.n_cols << "/" << t.n_rows << endl;
        //        cout  << "tvr/ttc: " << tempVec.n_rows << "/" << tt.n_rows << endl;
        interp1(t,reducedDesign.row(i).t(),tt,tempVec,"*nearest");

        design.row(i)= tempVec.t();
    }
    return design;
}

#endif //ORBIT_UTILITIES_H
