#ifndef FILTER_RRR_HPP
#define FILTER_RRR_HPP

#include <streampu.hpp>

class Filter_RRR : public spu::module::Stateful {
protected:
    int n_elmts;

public:

    Filter_RRR(const int n_elmts,std::vector<double>& h);
    virtual ~Filter_RRR() = default;
    void process(const double* input, double* output);

private:
    std::vector<double> b;
    std::vector<double> buffer;

};

#endif // FILTER_RRR_HPP