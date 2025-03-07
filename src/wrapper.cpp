#include "Recepteur_PM.hpp"
#include "Emetteur_PM.hpp"
#include "Canal.hpp"
#include "DetectCRC.hpp"
#include "Bit2Register.hpp"
#include <pybind11/pybind11.h>
#include <streampu.hpp>

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
}