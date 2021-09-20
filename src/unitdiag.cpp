#include <linconGaussR.h>

//' scale covariance and precision in place
//' This routine will scale the covariance matrix to have unit diagonal in place and also properly scale the precision.
//' @param Sigma a matrix that is the covariance matrix, will be scaled to have unit diagonal
//' @param Omega should be the inverse of Sigma
//' @return no return, scaling will be done in place
// [[Rcpp::export]]
void unitdiag(arma::mat & Sigma, arma::mat &Omega){
    linconGaussR::unitdiag(Sigma, Omega);
    return;
}