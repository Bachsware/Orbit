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
    vec design;
    double cost;
};

template<class Function>
auto Cmaes(const Function& fin,arma::vec x0, int maxFunEval, double targetCost = 1e-3)->Member{
    using namespace arma;
    unsigned int nFunEvals = 0;
    auto f = [&fin, &nFunEvals](const vec& x)->double {++nFunEvals; return fin(x); };

    double sigma = 0.3;
    const unsigned int N = x0.n_rows;

    vec pc = zeros(N, 1);
    vec ps = zeros(N, 1);                           // evolution paths for C and sigma
    mat B = eye(N, N);                              // B defines the coordinate system
    vec D = ones(N, 1);                             // diagonal D defines the scaling
    mat C = B * diagmat(arma::pow(D, 2)) * B.t();   // covariance matrix C
    mat invsqrtC = zeros(N,N);
    vec xmean = x0;

    const unsigned lambda = 30;
    const unsigned mu = lambda / 2;
    double eigeneval = 0;

    vec weightsm{ mu, arma::fill::zeros };
    weightsm = log(mu + 0.5) - arma::log(arma::linspace(1, mu, mu));
    weightsm /= arma::sum(weightsm);
    const vec weights = weightsm;

    const double mueff = as_scalar(std::pow(arma::sum(weights), 2) / arma::sum(arma::pow(weights, 2)));

    // adaption parameters
    const double cc = (4 + mueff / N) / (N + 4 + 2 * mueff / N); // time constant for cumulation for C
    const double cs = (mueff + 2) / (N + mueff + 5);  // t - const for cumulation for sigma control
    const double c1 = 2 / (std::pow(N + 1.3, 2) + mueff);    // learning rate for rank - one update of C
    const double cmu = std::min(1 - c1, 2 * (mueff - 2 + 1 / mueff) / (std::pow(N + 2, 2) + mueff));  // and for rank - mu update
    const double damps = 1 + 2 * std::max(0.0, std::sqrt((mueff - 1) / (N + 1)) - 1) + cs; // damping for sigma
    const double chiN = std::sqrt(N)*(1 - 1.0 / (4 * N) + 1.0 / (21 * std::pow(N, 2)));  // expectation of || N(0, I) || == norm(randn(N, 1))


    std::vector<Member> oldPopulation;
    auto funcType = []()->void{};
    while(nFunEvals<maxFunEval){
        std::vector<Member> population;
        auto driver = ParallelDriver{};
        std::mutex m;
        // Creating population
        for (int i = 0; i < lambda; ++i) {
            driver.addTask([&m,i, N,xmean,sigma,B,D,f,&population]()->void{

                // Construct member
                Member child;
                arma_rng::set_seed_random();
                vec randVec = arma::randn(N);
                randVec /= arma::norm(randVec);
                child.design = xmean + sigma*B*(D%randVec);

                // Test member and push to population
                m.lock();
                child.cost = f(child.design);
                population.push_back(child);
                m.unlock();
            });
        }

        driver.run();
        // Sorting members of population
        std::sort(population.begin(),population.end(), [](const Member a,const Member b)->bool{
            return b.cost > a.cost;
        });

        // Check fitness
        if(population.front().cost <= targetCost){
            oldPopulation = population;
            break;
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
        arma::mat artmp(N, mu, arma::fill::zeros);
        for (auto i = 0u; i < mu; ++i)
        {
            artmp.col(i) = 1 / sigma * (population.at(i).design - xold);
        }

        arma::mat rank1 = pc * pc.t(); // + (1 - hsig) * cc*(2 - cc) * C;
        if(!hsig) rank1 += cc*(2 - cc) * C;

        arma::mat rankMu = artmp * diagmat(weights) * artmp.t();

        C = (1 - c1 - cmu)*C;
        C += c1*rank1;
        C += cmu*rankMu;

        // Update sigma
        sigma *= std::exp((cs / damps)*(arma::norm(ps) / chiN - 1));

        // Diagonalize C
        if (nFunEvals - eigeneval > lambda / (c1 + cmu) / N / 10) // to achieve O(N ^ 2)
        {
            eigeneval = nFunEvals;
            auto C2 = (C + C.t()) / 2; // enforce symmetry (Maybe check if this is important ~ Christian Bach)

            arma::eig_sym(D, B, C2);// eigen decomposition, B == normalized eigenvectors

            D = sqrt(D);        // D contains standard deviations now

        }

        std:: cout << "cmaes: " << population.front().cost <<  endl;
        oldPopulation = population;
    };
    return oldPopulation.front();
}