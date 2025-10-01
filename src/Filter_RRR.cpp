#include "Filter_RRR.hpp"
#include <cmath>

Filter_RRR::Filter_RRR(const int n_elmts,std::vector<double>& h)
    : Stateful()
    , n_elmts(n_elmts)
    , buffer(h.size() - 1, 0.0)
	, b(h.size(), double(0)) {

    for (size_t i = 0; i < h.size(); i++)
		this->b[i] = h[h.size() - 1 - i];

    const std::string name = "Filter_RRR";
    this->set_name(name);

    auto& p = this->create_task("process");                                                  // Create the task
    size_t ps_input = this->template create_socket_in<double>(p, "input", this->n_elmts);    // Create the input socket
    size_t ps_output = this->template create_socket_out<double>(p, "output", this->n_elmts);   // Create the output socket

    // create the codelet
    this->create_codelet(
      p,
      [ps_input, ps_output](spu::module::Module& m, spu::runtime::Task& t, const size_t frame_id) -> int
      {
          // Recover the Module and Sockets in the codelet
          auto& filter_RRR = static_cast<Filter_RRR&>(m);
          double* input = (double*)(t[ps_input].get_dataptr());
          double* output = (double*)(t[ps_output].get_dataptr());

          // Process the data
          filter_RRR.process(input, output);
          return spu::runtime::status_t::SUCCESS;
      });
}

void Filter_RRR::process(const double* input, double* output) {

    for (size_t i = 0; i < n_elmts; ++i) {
        double sum = 0.0;
        for (size_t k = 0; k < b.size(); ++k) {
            if ((i+k)<buffer.size()) {
                sum += buffer[i+k] * b[k];
            } else {
                sum += input[i+k-buffer.size()] * b[k];
            }
        }
        output[i] = sum;
    }

    for (size_t i = 0; i < buffer.size(); ++i) {
        buffer[i] = input[n_elmts - buffer.size() + i];
    }

}
