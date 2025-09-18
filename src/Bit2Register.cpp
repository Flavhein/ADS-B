#include "Bit2Register.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <iterator> // Pour std::next


Bit2Register::Bit2Register(const int n_elmts)
    : Stateful()
    ,n_elmts(n_elmts) {

        const std::string name = "Bit2Register";
        this->set_name(name);

        auto& p = this->create_task("process");                                                  // Create the task
        size_t ps_input = this->template create_socket_in<double>(p, "input", this->n_elmts);    // Create the input socket

        size_t ps_adresse = this->template create_socket_out<int8_t>(p, "adresse", 6*sizeof(int8_t)); // Create the output socket
        size_t ps_format = this->template create_socket_out<int>(p, "format", 1);
        size_t ps_type = this->template create_socket_out<int>(p, "type", 1);
        size_t ps_nom = this->template create_socket_out<int8_t>(p, "nom", 6*sizeof(int8_t));
        size_t ps_altitude = this->template create_socket_out<int>(p, "altitude", 1);
        size_t ps_timeFlag = this->template create_socket_out<int>(p, "timeFlag", 1);
        size_t ps_cprFlag = this->template create_socket_out<int>(p, "cprFlag", 1);
        size_t ps_latitude = this->template create_socket_out<double>(p, "latitude", 1);
        size_t ps_longitude = this->template create_socket_out<double>(p, "longitude", 1);

        // create the codelet
        this->create_codelet(
          p,
          [ps_input,
            ps_adresse,
            ps_format,
            ps_type,
            ps_nom,
            ps_altitude,
            ps_timeFlag,
            ps_cprFlag,
            ps_latitude,
            ps_longitude
            ](spu::module::Module& m, spu::runtime::Task& t, const size_t frame_id) -> int
          {
              // Recover the Module and Sockets in the codelet
              auto& bit2register = static_cast<Bit2Register&>(m);
              double* input = (double*)(t[ps_input].get_dataptr()); //un void* par défaut d'où le cast

              char* adresse = (char*)(t[ps_adresse].get_dataptr());
              int* format = (int*)(t[ps_format].get_dataptr());
              int* type = (int*)(t[ps_type].get_dataptr());
              char* nom = (char*)(t[ps_nom].get_dataptr());
              int* altitude = (int*)(t[ps_altitude].get_dataptr());
              int* timeFlag = (int*)(t[ps_timeFlag].get_dataptr());
              int* cprFlag = (int*)(t[ps_cprFlag].get_dataptr());
              double* latitude = (double*)(t[ps_latitude].get_dataptr());
              double* longitude = (double*)(t[ps_longitude].get_dataptr());

              // Process the data
              bit2register.process(input, adresse, format, type, nom, altitude, timeFlag, cprFlag, latitude, longitude);
              return spu::runtime::status_t::SUCCESS;
          });

}

void Bit2Register::process(double * input,
    char adresse[1024],
    int* format,
    int* type,
    char nom[1024],
    int* altitude,
    int* timeFlag,
    int* cprFlag,
    double* latitude,
    double* longitude) {

    std::vector<double> vectbin(input, input + n_elmts);

    auto formatBits = getRange(vectbin, 0, 5);
    auto adresseBits = getRange(vectbin, 8, 32);
    auto typeBits = getRange(vectbin, 32, 37);

    *format = bin2dec(formatBits);
    bin2hex(adresseBits, adresse);
    *type = bin2dec(typeBits);
    //std::cout << adresse << std::endl;

    if ((*type >= 9 && *type <= 18) || (*type >= 20 && *type <= 22)) {
        auto altitudeBits = getRange(vectbin, 40, 47);
        auto tempVect = getRange(vectbin, 48, 52);
        altitudeBits.insert(altitudeBits.end(), tempVect.begin(), tempVect.end());
        *altitude = bin2dec(altitudeBits) * 25 - 1000;
        *timeFlag = vectbin[52];
        *cprFlag = vectbin[53];
        int LAT = bin2dec(getRange(vectbin, 54, 71));
        int LON = bin2dec(getRange(vectbin, 71, 88));
        *latitude = calcLAT(*cprFlag, LAT, 44.806884);
        *longitude = calcLON(*cprFlag, LON, -0.606629, *latitude);
    }
    if (*type >= 1 && *type <= 4) {
        //std::string nom = "";
        for (int i = 0; i < 8; ++i) {
            auto charBits = getRange(vectbin, 40 + i * 6, 46 + i * 6);
            nom[i] = bin2carid(charBits);
        }
        //nom = nom;
        //std::cout << nom << std::endl;
    }

    //std::cout<<"indicateur fin de process"<<std::endl;
    //return registre;
}



int Bit2Register::bin2dec(const std::vector<double>& bits) {
    int value = 0;
    for (double bit : bits) {
        value = (value << 1) | static_cast<int>(bit);
    }
    return value;
}

void Bit2Register::bin2hex(const std::vector<double>& bits, char* out) {
    int value = bin2dec(bits);
    std::stringstream ss;
    ss << std::hex << std::setw(6) << std::setfill('0') << value;
    auto longu = ss.str().length();
    std::string inter = ss.str();

    for (int i=0; i<longu; i++){
        out[i] = inter[i];
    }


    //return new std::string(ss.str());
}

char Bit2Register::bin2carid(const std::vector<double>& bits) {

    if (bits.size() == 6) {
        const std::string majA_O = "ABCDEFGHIJKLMNO";
        const std::string majP_Z = "PQRSTUVWXYZ     ";

        std::vector<double> flipped_vect = bits;
        std::reverse(flipped_vect.begin(), flipped_vect.end());
        std::vector<double> extract = getRange(flipped_vect,0,4);
        std::reverse(extract.begin(), extract.end());

        int value = bin2dec(extract);
        //std::cout<< "valeur : "<<value<<std::endl;

        if (flipped_vect[5] == 0) {
            if (flipped_vect[4] == 0) {
                return majA_O[value-1];
            } else {
                return majP_Z[value];
            }
        } else {
            if (flipped_vect[4] == 1) {
                return std::to_string(value)[0];
            } else {
                return ' ';
            }
        }
    } else {
        throw std::invalid_argument("La suite de bits n'est pas de taille 6");
    }
}

double Bit2Register::NL(double x, int Nz) {
    if (x == 0) {
        return 59;
    } else if (x == 87) {
        return 2;
    } else if (std::abs(x) > 87) {
        return 1;
    } else {
        //return std::floor(2 * M_PI *
        //                  std::pow(std::acos(1 - (1 - std::cos(M_PI / (2 * Nz))) /
        //                                     std::pow(std::cos(M_PI * std::abs(x) / 180), 2)), -1));
        return std::floor(2*M_PI / (std::acos(1- ( (1-std::cos(M_PI/(2*Nz))) / (std::pow(std::cos(M_PI*x/180),2)) ))));
    }
}

double Bit2Register::calcLAT(int cprFlag, int LAT, double latref) {
    const int Nz = 15; // Nombre de latitudes géographiques considérées entre l'équateur et un pôle
    const double Dlati = 360.0 / (4 * Nz - cprFlag);
    const int Nb = 17; // Nombre de bits constituant le registre de latitude


    // Calcul de l'indice j
    double j = std::floor(latref / Dlati) +
               std::floor(0.5 + std::fmod(latref, Dlati) / Dlati - static_cast<double>(LAT) / (1 << Nb));

    // Calcul de la latitude
    double lat = Dlati * (j + static_cast<double>(LAT) / (1 << Nb));

    return lat;
}

double Bit2Register::calcLON(int cprFlag, int LON, double lonref, double lat) {
    const int Nb = 17; // Nombre de bits utilisés pour représenter la longitude
    const int Nz = 15; // Nombre de latitudes géographiques considérées entre l'équateur et un pôle

    // Calcul de Dloni
    double Dloni;
    if (NL(lat, Nz) - cprFlag > 0) {
        Dloni = 360.0 / (NL(lat, Nz)); //NL(lat,Nz) - cprFlag
    } else {
        Dloni = 360.0; // degrés
    }

    // Calcul de l'indice m
    double m = std::floor(lonref / Dloni) +
               std::floor(0.5 + std::fmod(lonref, Dloni) / Dloni - static_cast<double>(LON) / (1 << Nb));

    // Calcul de la longitude
    double lon = Dloni * (m + static_cast<double>(LON) / (1 << Nb));

    return lon;
}

std::vector<double> Bit2Register::getRange(std::vector<double>& vect, int start, int end) {
    return std::vector<double>(vect.begin() + start, vect.begin() + end);
}
