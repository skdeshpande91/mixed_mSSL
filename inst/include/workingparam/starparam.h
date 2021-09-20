#ifndef WORKINGPARAM_STARPARAM_H
#define WORKINGPARAM_STARPARAM_H


// [[Rcpp::depends(RcppArmadillo)]]
#include <linconGaussR.h> 
#include "wparam.h"

using namespace Rcpp;
using namespace arma;
using namespace std;
using namespace linconGaussR;

namespace workingparam{

class starWorkingParam : public WorkingParam{
    public:
        starWorkingParam() = default;
        starWorkingParam(arma::mat S, 
            arma::mat R,
            arma::mat tXR,
            arma::mat tXX,
            arma::vec mu,int n): WorkingParam(S, R, tXR, tXX, mu, n){};
        inline void update(const arma::mat &lower,
                    const arma::mat &upper,
                        const arma::mat &X,
                        const arma::vec &mu_t,
                        const arma::mat &B_t,
                        const arma::mat &Sigma_t,
                        int n_rep, int nskp = 5);
};

inline void starWorkingParam::update(const arma::mat &lower,
                        const arma::mat &upper,
                        const arma::mat &X,
                        const arma::vec &mu_t,
                        const arma::mat &B_t,
                        const arma::mat &Sigma_t,
                        int n_rep, int nskp){

    int n = lower.n_rows;
    int q = lower.n_cols;
    arma::mat XB = X * B_t;
    XB.each_row() += mu_t.t();
    arma::mat C = arma::chol(Sigma_t);
    arma::vec x_init(q);
    arma::vec b(2*q);
    arma::mat A(2*q,q);
    //arma::mat A_temp(q,q);
    S.zeros(q,q);
    mu.zeros(q);
    R.zeros(n,q);
    tXR.zeros(q,q);
    s_eval.zeros(q);
    //arma::mat S(q,q,fill::zeros);
    //arma::vec mu(q,fill:zeros);

    for(int i = 0 ; i < n ; i++){
        Rcpp::checkUserInterrupt();
        b.rows(0,q-1) = trans(XB.row(i)-lower.row(i));
        b.rows(q,2*q-1) = trans(upper.row(i)-XB.row(i));
        A.rows(0,q-1) = C.t();
        A.rows(q,2*q-1) = -C.t();
        x_init = trans(lower.row(i)+upper.row(i));
        x_init *= 0.5;
        x_init = arma::solve(C.t(), x_init-arma::trans(XB.row(i)));// this serves as the initial point
        LinearConstraints lincon(A,b,true);
        //Rcout << "      " << i <<"th sample sampling start" << endl;
        EllipticalSliceSampler sampler(n_rep + 1,lincon,nskp,x_init);
        sampler.run();
        //Rcout << "      end" << endl;
        arma::mat resi = sampler.loop_state.samples;
        resi.shed_row(0);// we will remove the initial points
        resi = resi * C;
        R.row(i) = mean(resi);
        S += resi.t() * resi;
        resi.each_row() += mu_t.t();
        mu += arma::trans(arma::mean(resi)); 
    }

    S /= (n*n_rep);
    mu /= n;
    R.each_row() += mu_t.t();
    R.each_row() -= mu.t();
    tXR = X.t() * R;
    tRR = S*n;
    
    s_eval = eig_sym(S);

}


}

#endif