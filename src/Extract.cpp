#include "Extract.hpp"


Extract::Extract(const int n_elmts, const int Fse, const double seuil)
    : Stateful()
    , n_elmts(n_elmts)
    , Fse(Fse)
    , seuil(seuil)
     {

    buffer=new double[224];
    size_buffer=0;
    voie=-1;

    const std::string name = "Extract";
    this->set_name(name);

    auto& p = this->create_task("process");
    size_t ps_decalage = this->template create_socket_in<int>(p, "decalage", 1);
    size_t ps_max = this->template create_socket_in<double>(p, "max", 1);
    size_t ps_sigs = this->template create_socket_in<double>(p, "sigs", this->n_elmts);

    size_t ps_tram = this->template create_socket_out<double>(p, "tram", 224);

    // create the codelet
    this->create_codelet(
      p,
      [ps_decalage, ps_max, ps_sigs, ps_tram](spu::module::Module& m, spu::runtime::Task& t, const size_t frame_id) -> int
      {
          // Recover the Module and Sockets in the codelet
          auto& extract = static_cast<Extract&>(m);
          int* decalage = (int*)(t[ps_decalage].get_dataptr());
          double* max = (double*)(t[ps_max].get_dataptr());
          double* sigs = (double*)(t[ps_sigs].get_dataptr());
          double* tram = (double*)(t[ps_tram].get_dataptr());


          // Process the data
          extract.process(decalage, max, sigs, tram);
          return spu::runtime::status_t::SUCCESS;
      });
}

void Extract::process(const int* decalage, const double* max, double* sigs, double* tram) {
    int voie_sig =  (decalage[0]) % (this->Fse /2); // egual à 0 ou 1 (car échantillonnage à Ts/2)

    if (size_buffer > 0 and voie != -1){
        for (int j=0; j< size_buffer; j++){
            tram[j]=buffer[j];
        }

        for (int i=0; i <(224-size_buffer); i++){
            tram[size_buffer+i]=sigs[i*(Fse/2)+voie]; //voie ou voie_sig
        }
    }

    if (max[0] > seuil){
        size_buffer = 224-decalage[0];
        voie = voie_sig;
        for (int k=0; k< size_buffer; k++){
            buffer[k]=sigs[decalage[0] + (k*(Fse/2)+voie_sig)];
        }

    } else{
        size_buffer=0;
        voie=-1;
        throw spu::tools::processing_aborted();
    }

    if (isEmpty(tram,112)){
        throw spu::tools::processing_aborted();
    }

}


bool Extract::isEmpty(double* input, int taille){
    for (int i=0; i<taille; i++){
        if (input[i] != 0.0){
            return false;
        }
    }
    return true;
}
