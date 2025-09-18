#include "DetectCRC.hpp"

DetectCRC::DetectCRC(const int n_elmts)
    : Stateful()
    , n_elmts(n_elmts){

    //Polynome_CRC = {1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,0,0,0,0,0,1,0,0,1};

    Polynome_CRC = {
        1, 1,1,1,1, 1,1,1,1, 1,1,1,1,  //FFF
        0,1,0,0,                    // 4
        0,0,0,0,                    // 0
        1,0,0,1                     // 9
    };

    const std::string name = "DetectCRC";
    this->set_name(name);

    auto& p = this->create_task("process");                                                  // Create the task
    size_t ps_input = this->template create_socket_in<double>(p, "input", this->n_elmts);    // Create the input socket
    size_t ps_output = this->template create_socket_out<int>(p, "output", 1); // Create the output socket

    // create the codelet
    this->create_codelet(
      p,
      [ps_input, ps_output](spu::module::Module& m, spu::runtime::Task& t, const size_t frame_id) -> int
      {
          // Recover the Module and Sockets in the codelet
          auto& detectCRC = static_cast<DetectCRC&>(m);
          double* input = (double*)(t[ps_input].get_dataptr());
          int* output = (int*)(t[ps_output].get_dataptr());

          // Process the data
          detectCRC.process(input, output);
          return spu::runtime::status_t::SUCCESS;
      });

}

void DetectCRC::process(double* tram, int* output) {
    std::vector<bool> bits(n_elmts);
    for (size_t i = 0; i < n_elmts; ++i) {
        bits[i] = (tram[i] > 0.5);
    }

    int degree = Polynome_CRC.size()-1;

    if (n_elmts < degree) {
        output[0] = 0;  // too short
        return;
    }

    std::vector<bool> data_bits(bits.begin(), bits.end() - degree);
    std::vector<bool> received_crc(bits.end() - degree, bits.end());


    //std::vector<bool> crc(degree,0);
    std::vector<bool> augmented_message = data_bits;

    /*
    std::cout<< "augmented message : ";
    for (int i =0;i<augmented_message.size();i++){
    	std::cout<<augmented_message[i];
    }
    std::cout<<std::endl;
    */

    augmented_message.insert(augmented_message.end(), degree, 0);

    int taille_reste = augmented_message.size();
    while (taille_reste > degree) {
        /*
    	std::cout<< "augmented message : ";
    	for (int i =0;i<augmented_message.size();i++){
    		std::cout<<augmented_message[i];
    	}
    	std::cout<<std::endl;
    	std::cout<<"taille reste : "<<taille_reste<<std::endl;
        */

    	if (augmented_message[augmented_message.size()-taille_reste]==0){
    		taille_reste--;
    	} else {
    		for (int i = 0; i < Polynome_CRC.size(); i++){
    			augmented_message[augmented_message.size()-taille_reste+i]=augmented_message[augmented_message.size()-taille_reste+i] ^ Polynome_CRC[i];
    		}
    	}
    }

    //std::cout<< "CRC : ";
    bool is_valid = true;
    for (int i =0;i<degree;i++){ //Check
        if (augmented_message[data_bits.size()+i] != received_crc[i]) {
            is_valid = false;
            break;
        }
    	//std::cout<<crc[i];
    }
    //std::cout<<std::endl;


/*
    //print received bit data
    std::cout << "data bits : ";
    for (bool b : data_bits) {
        std::cout << b;
    }
    std::cout << std::endl;
*//*
    // print computed CRC
    std::cout << "computed CRC : ";
    for (bool b : crc) {
        std::cout << b;
    }
    std::cout << std::endl;
*//*
    // print received CRC
    std::cout << "received CRC : ";
    for (bool b : received_crc) {
        std::cout << b;
    }
    std::cout << std::endl;
*/

    output[0] = is_valid ? 1 : 0;
}
