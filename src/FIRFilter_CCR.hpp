#ifndef FIRFILTER_HPP
#define FIRFILTER_HPP

#include <streampu.hpp>
#include "mipp.h"
#include <vector>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>


class FIRFilter : public spu::module::Stateful {
protected:
    int n_elmts;

public:

    FIRFilter(const int n_elmts,const std::vector<double>& b);
    virtual ~FIRFilter() = default;
    void process(const double* input, double* output);
    inline void step(const double* x_elt, double* y_elt);
    void reset();

private:
    std::vector<double> b;
    std::vector<double> buffer;
    int head;
    int size;
    int M;
    int P;

};

void FIRFilter::step(const double* x_elt, double* y_elt)
{
	this->buffer[this->head] = *x_elt;
	this->buffer[this->head + this->size] = *x_elt;

	*y_elt = this->buffer[this->head+1] * this->b[0];
	for (auto i = 1; i < this->size ; i++)
		*y_elt += this->buffer[this->head + 1 + i] * this->b[i];

	this->head++;
	this->head %= this->size;
}


#endif // FIRFILTER_HPP