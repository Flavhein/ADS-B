#include "Select.hpp"


Select::Select(const int n_elmts, double energie_preamb)
    : Stateful()
    , n_elmts(n_elmts)
    , energie_preamb(energie_preamb) {

    const std::string name = "Select";
    this->set_name(name);

    auto& p = this->create_task("process");                                                  // Create the task
    size_t ps_num = this->template create_socket_in<double>(p, "num1", this->n_elmts);    // Create the input socket
    size_t ps_denum = this->template create_socket_in<double>(p, "denum1", this->n_elmts);
    size_t ps_decalage = this->template create_socket_out<int>(p, "decalage", 1);  // Create the output socket
    size_t ps_max = this->template create_socket_out<double>(p, "max", 1);
    size_t ps_intercorr = this->template create_socket_out<double>(p, "intercorr", this->n_elmts);

    // create the codelet
    this->create_codelet(
      p,
      [ps_num, ps_denum, ps_decalage, ps_max, ps_intercorr](spu::module::Module& m, spu::runtime::Task& t, const size_t frame_id) -> int
      {
          // Recover the Module and Sockets in the codelet
          auto& select = static_cast<Select&>(m);

          double* num = (double*)(t[ps_num].get_dataptr());
          double* denum = (double*)(t[ps_denum].get_dataptr());
          int* decalage = (int*)(t[ps_decalage].get_dataptr());
          double* max = (double*)(t[ps_max].get_dataptr());
          double* intercorr = (double*)(t[ps_intercorr].get_dataptr());

          // Process the data
          select.process(num,denum,decalage,max, intercorr);
          return spu::runtime::status_t::SUCCESS;
      });
}

void Select::process(const double* num, const double* denum, int* decalage, double* max, double* intercorr) {

    //std::cerr<<" enegie preamb "<<this->energie_preamb<<std::endl;

    max[0]=0.0;
    decalage[0]=0;

    for (int i=0; i<n_elmts; i++){
        if (denum[i]==0){ //attention <0 ?
            intercorr[i]=0.0;
        } else {
            intercorr[i]=num[i]/(denum[i]*this->energie_preamb);
            if (intercorr[i]>max[0]){
                max[0]=intercorr[i];
                decalage[0]=i;
            }
        }
    }

    //std::cout<<std::endl;

    //double* p_max = std::max_element(intercorr, intercorr + n_elmts);
    //decalage[0] = (p_max - intercorr);
    //max[0] = *p_max;
    //std::cerr<<"decalage : "<<decalage[0]<<" max : "<<max[0]<<std::endl;
    //throw spu::tools::runtime_error();
}