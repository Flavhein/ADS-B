#ifndef AbsolueCarre_HPP
#define AbsolueCarre_HPP

#include <streampu.hpp>

class AbsolueCarre : public spu::module::Stateful {
protected:
    int n_elmts;

public:

AbsolueCarre(const int n_elmts,int isComplex);
    virtual ~AbsolueCarre() = default;
    void process(const double* input, double* output);

private:
    int isComplex;

};

#endif // AbsolueCarre_HPP