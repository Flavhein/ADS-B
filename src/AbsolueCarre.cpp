#include "AbsolueCarre.hpp"


AbsolueCarre::AbsolueCarre(const int n_elmts, int isComplex) //isCompelx == 1 yes, isComplex = 0 no
    : Stateful()
    , n_elmts(n_elmts)
    , isComplex(isComplex) {

    const std::string name = "AbsolueCarre";
    this->set_name(name);

    auto& p = this->create_task("process");                                                  // Create the task
    size_t ps_input = this->template create_socket_in<double>(p, "input", this->n_elmts);    // Create the input socket
    size_t ps_output = this->template create_socket_out<double>(p, "output", this->n_elmts / (1+isComplex));   // Create the output socket

    // create the codelet
    this->create_codelet(
      p,
      [ps_input, ps_output](spu::module::Module& m, spu::runtime::Task& t, const size_t frame_id) -> int
      {
          // Recover the Module and Sockets in the codelet
          auto& absolueCarre = static_cast<AbsolueCarre&>(m);
          double* input = (double*)(t[ps_input].get_dataptr());
          double* output = (double*)(t[ps_output].get_dataptr());

          // Process the data
          absolueCarre.process(input, output);
          return spu::runtime::status_t::SUCCESS;
      });
}

void AbsolueCarre::process(const double* input, double* output) {
    if (isComplex == 1) {
        for (size_t i = 0; i < n_elmts/2; ++i) {
            output[i]=(input[i*2]*input[i*2])+(input[i*2 +1]*input[i*2 +1]);
        }
    } else {
        for (size_t i = 0; i < n_elmts; ++i) {
            output[i]=(input[i]*input[i]);
        }
    }
}