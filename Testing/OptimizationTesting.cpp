//
// Created by christian on 03/03/17.
//

#include "../Optimization/Cmaes.h"

int main(int argc, char* argv[]) {
    using namespace arma;
    auto fun1 = [](vec x)->double {
        double res=0;
        for (int i = 0; i < x.n_rows; ++i) {
            res += (x(i)-double(i))*(x(i)-double(i));
        }
        return res;
    };
    Member solution = Cmaes(fun1,{0,0,0,0,0,0,0,0,0,0,0,0,0},1e5,1e-12);

    cout << "solution cost: " << endl << solution.cost << endl;
    cout << "solution design: " << solution.design << endl;
}