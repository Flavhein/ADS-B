#include "Recepteur_PM.hpp"
#include "Emetteur_PM.hpp"
#include "Canal.hpp"
#include "DetectCRC.hpp"
#include "Bit2Register.hpp"
#include "Decimation_PM.hpp"
#include "FIRFilter.hpp"
#include "AbsolueCarre.hpp"
#include "Decision_PM.hpp"
#include "CorrSelect.hpp"
#include "Extract.hpp"
#include "Redirig.hpp"
#include "DecodCoord.hpp"
#include "DecodNom.hpp"
#include <pybind11/pybind11.h>
#include <streampu.hpp>
#include <pybind11/numpy.h>



namespace py = pybind11;
using namespace py::literals;

// Create a	python module using PYBIND11
PYBIND11_MODULE(ads_b, m)
{
    auto pyspu_stateful = (py::object)py::module_::import("streampu").attr("Stateful");

    py::class_<RecepteurPM, spu::module::Stateful>(m, "RecepteurPM")
        .def(py::init<const int, double, double>(), "n_elmts"_a, "Fe"_a, "Ts"_a);

    py::class_<EmetteurPM, spu::module::Stateful>(m, "EmetteurPM")
        .def(py::init<const int, double, double>(), "n_elmts"_a, "Fe"_a, "Ts"_a);

    py::class_<Canal, spu::module::Stateful>(m, "Canal")
        .def(py::init<const int, const double, const int>(), "n_elmts"_a, "SNR"_a, "len_p"_a);

    py::class_<DetectCRC, spu::module::Stateful>(m, "DetectCRC")
        .def(py::init<const int>(), "n_elmts"_a);

    py::class_<Register>(m, "Register")
        .def(py::init<>())
        .def_static("recast",[](py::array& arr){
            py::buffer_info buffer = arr.request();
            return static_cast<Register*>(buffer.ptr);
        })
        .def_readwrite("adresse", &Register::adresse)
        .def_readwrite("format", &Register::format)
        .def_readwrite("type", &Register::type)
        .def_readwrite("nom", &Register::nom)
        .def_readwrite("altitude", &Register::altitude)
        .def_readwrite("timeFlag", &Register::timeFlag)
        .def_readwrite("cprFlag", &Register::cprFlag)
        .def_readwrite("latitude", &Register::latitude)
        .def_readwrite("longitude", &Register::longitude);

    py::class_<Bit2Register, spu::module::Stateful>(m, "Bit2Register")
        .def(py::init<const int>(), "n_elmts"_a);

    py::class_<DecimationPM, spu::module::Stateful>(m, "DecimationPM")
        .def(py::init<const int, int>(), "n_elmts"_a, "isComplex"_a);

    py::class_<FIRFilter, spu::module::Stateful>(m, "FIRFilter")
        .def(py::init<const int, const std::vector<double>&, int>(), "n_elmts"_a, "b"_a, "buffersize"_a);

    py::class_<AbsolueCarre, spu::module::Stateful>(m, "AbsolueCarre")
        .def(py::init<const int>(), "n_elmts"_a);

    py::class_<DecisionPM, spu::module::Stateful>(m, "DecisionPM")
        .def(py::init<const int, double>(), "n_elmts"_a, "v0"_a);

    py::class_<CorrSelect, spu::module::Stateful>(m, "CorrSelect")
        .def(py::init<const int>(), "n_elmts"_a);

    py::class_<Extract, spu::module::Stateful>(m, "Extract")
        .def(py::init<const int, const int, const int, const double>(), "n_elmts"_a, "rows"_a, "cols"_a, "seuil"_a);

    py::class_<Redirig, spu::module::Stateful>(m, "Redirig")
        .def(py::init<const int>(), "n_elmts"_a);

    py::class_<DecodCoord, spu::module::Stateful>(m, "DecodCoord")
        .def(py::init<const int>(), "n_elmts"_a);

    py::class_<DecodNom, spu::module::Stateful>(m, "DecodNom")
        .def(py::init<const int>(), "n_elmts"_a);
}