//
// Created by christian on 02/03/17.
//

#ifndef ORBIT_CMAES_H
#define ORBIT_CMAES_H

#endif //ORBIT_CMAES_H

#include <armadillo>
#include <mutex>
#include "ParallelDriver.h"


struct Member{
    arma::vec design;
    double cost;
    bool operator<(const Member& other){
        return cost < other.cost;
    }

    bool operator>(const Member& other){
        return cost > other.cost;
    }
};

template<class Function>
auto Cmaes(const Function& fin,arma::vec x0, unsigned int maxFunEval = 1e4, double targetCost = 1e-3, double initialStepSize = 0.3)->Member{
    using namespace arma;
    unsigned int nFunEvals = 0;


    double sigma = initialStepSize;                 // Global step size
    const unsigned int N = x0.n_rows;               // Dimensionality of optimization

    vec pc = zeros(N, 1);
    vec ps = zeros(N, 1);                           // evolution paths for C and sigma
    mat B = eye(N, N);                              // B defines the coordinate system
    vec D = ones(N, 1);                             // diagonal D defines the scaling
    mat C = B * diagmat(arma::pow(D, 2)) * B.t();   // covariance matrix C
    mat invsqrtC = zeros(N,N);                      // = (sqrt(C)).inv()
    vec xmean = x0;                                 // Input guess

    const unsigned lambda = 16; //30;                     // No. of Offsprings
    const unsigned mu = lambda / 2;                 // No. of selected offsprings for Covariance update
    double eigeneval = 0;                           // No. of eigendecompositions

    vec weightsm{ mu, arma::fill::zeros };
    weightsm = log(mu + 0.5) - arma::log(arma::linspace(1, mu, mu));
    weightsm /= arma::sum(weightsm);
    const vec weights = weightsm;                   // weights for creating C update

    const double mueff = as_scalar(std::pow(arma::sum(weights), 2) / arma::sum(arma::pow(weights, 2)));

    // adaption parameters
    const double cc = (4 + mueff / N) / (N + 4 + 2 * mueff / N);  // time constant for cumulation for C
    const double cs = (mueff + 2) / (N + mueff + 5);              // t - const for cumulation for sigma control
    const double c1 = 2 / (std::pow(N + 1.3, 2) + mueff);         // learning rate for rank - one update of C
    const double cmu = std::min(1 - c1, 2 * (mueff - 2 + 1 / mueff) / (std::pow(N + 2, 2) + mueff));  // and for rank - mu update
    const double damps = 1 + 2 * std::max(0.0, std::sqrt((mueff - 1) / (N + 1)) - 1) + cs;  // damping for sigma
    const double chiN = std::sqrt(N)*(1 - 1.0 / (4 * N) + 1.0 / (21 * std::pow(N, 2)));     // expectation of || N(0, I) || == norm(randn(N, 1))

    std::vector<Member> population(lambda);
    Member bestMember;
    bestMember.cost = 1e90;

    while(nFunEvals<maxFunEval){
        auto driver = ParallelDriver{};
        std::mutex m;


        // Creating population
        for (unsigned  int i = 0; i < lambda; ++i) {
            driver.addTask([&m,i, N,xmean,sigma,B,D,fin,&nFunEvals,&population]()->void{
              // Create child for the new population
              int nFunEvalsThread = 0;
              Member child;
              auto f = [fin,&nFunEvalsThread](const vec& x)->double {++nFunEvalsThread; return fin(x); };

              if(nFunEvals!=0){ // Sample from distribution
                arma_rng::set_seed_random();
                vec randVec = arma::randn(N);
                randVec/=norm(randVec);
                randVec = xmean + sigma*B*(D%randVec);

                child.design = randVec;
                child.cost = f(child.design);

              } else{ // insert best from last generation
                if (nFunEvals!=0) {
                  child = population.front();
                } else{ // insert initialguess
                  child.design = xmean;
                  child.cost = f(child.design);
                }
              }
              // Push child to population and update nFunEvals
              m.lock();
                  //std:: cout << "--- cost=" << child.cost << endl;
                  nFunEvals += nFunEvalsThread;
                  population.at(i)=child;
              m.unlock();
            });
        }

        cout << "CMAES: leaderCost = "<< population.front().cost <<"\tsigma = "<<sigma << endl;
        driver.run();

        // Sorting members of population
        std::sort(population.begin(),population.end()); // This one may contain -nan FIX THIS

        // Check fitness
        if(population.front().cost <= targetCost){break;}
        if(population.front().cost < bestMember.cost){
          bestMember = population.front();
        }

        // Update mean
        vec xold = xmean;
        xmean = vec{ N, arma::fill::zeros };
        for (auto i = 0u; i < mu; ++i) {
            xmean += population.at(i).design*weights.at(i);
        }

        // Update evolution path for sigma
        invsqrtC = B * arma::diagmat(arma::pow(D, -1)) * B.t();
        ps = (1 - cs) * ps + sqrt(cs*(2 - cs)*mueff) * invsqrtC * (xmean - xold) / sigma;

        // Update evolution path fir C1 update
        double x = arma::as_scalar(arma::sum(arma::pow(ps, 2)));
        bool hsig = x / (1 - std::pow(1 - cs, 2 * nFunEvals / lambda)) / N < 2 + 4 / (N + 1);
        pc = (1 - cc) * pc;
        if(hsig) pc += sqrt(cc*(2 - cc)*mueff) * (xmean - xold) / sigma;

        // Update Covariance matrix
            // Rank my update
            arma::mat artmp(N, mu, arma::fill::zeros);
            for (auto i = 0u; i < mu; ++i)
            {
                artmp.col(i) = 1 / sigma * (population.at(i).design - xold);
            }

            // Rank 1 update
            arma::mat rank1 = pc * pc.t(); // + (1 - hsig) * cc*(2 - cc) * C;
            if(!hsig) rank1 += cc*(2 - cc) * C;

            arma::mat rankMu = artmp * diagmat(weights) * artmp.t();

        C = (1 - c1 - cmu)*C;
        C += c1*rank1;              // Rank 1 update
        C += cmu*rankMu;            // Rank mu update

        // Update sigma
        sigma *= std::exp((cs / damps)*(arma::norm(ps) / chiN - 1));

        // Diagonalize C
        if (nFunEvals - eigeneval > lambda / (c1 + cmu) / N / 10) // to achieve O(N ^ 2)
        {
            eigeneval = nFunEvals;  //
            arma::eig_sym(D, B, C); // eigen decomposition, B == normalized eigenvectors
            D = sqrt(D);            // D contains standard deviations now
        }

    };
    return bestMember;
}
