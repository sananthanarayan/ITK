// This is core/vnl/vnl_cost_function.cxx
//:
// \file
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date   23 Oct 1997
//
//-----------------------------------------------------------------------------

#include "vnl_cost_function.h"
#include <cassert>

static bool f_calling_compute;

void
vnl_cost_function::compute(const vnl_vector<double> & x, double * val, vnl_vector<double> * g)
{
  if (val)
    *val = this->f(x);
  if (g)
    this->gradf(x, *g);
}

//: Default implementation of f is compute...
double
vnl_cost_function::f(const vnl_vector<double> & x)
{
  // if we get back here from compute, neither vf was implemented.
  if (f_calling_compute)
    assert(!"vnl_cost_function: RECURSION");
  double val;
  f_calling_compute = true;
  this->compute(x, &val, nullptr);
  f_calling_compute = false;
  return val;
}

//: Default implementation of gradf is to call compute
void
vnl_cost_function::gradf(const vnl_vector<double> & x, vnl_vector<double> & g)
{
  if (f_calling_compute)
    assert(!"vnl_cost_function: RECURSION");
  f_calling_compute = true;
  this->compute(x, nullptr, &g);
  f_calling_compute = false;
}

//: Compute fd gradient
void
vnl_cost_function::fdgradf(const vnl_vector<double> & x, vnl_vector<double> & gradient, double stepsize)
{
  vnl_vector<double> tx = x;
  const double h = stepsize;
  for (int i = 0; i < dim; ++i)
  {
    const double tplus = x[i] + h;
    tx[i] = tplus;
    const double fplus = this->f(tx);

    const double tminus = x[i] - h;
    tx[i] = tminus;
    const double fminus = this->f(tx);

    gradient[i] = (fplus - fminus) / (tplus - tminus);
    tx[i] = x[i];
  }
}

vnl_vector<double>
vnl_cost_function::gradf(const vnl_vector<double> & x)
{
  vnl_vector<double> g(dim);
  this->gradf(x, g);
  return g;
}

vnl_vector<double>
vnl_cost_function::fdgradf(const vnl_vector<double> & x)
{
  vnl_vector<double> g(dim);
  this->fdgradf(x, g);
  return g;
}
