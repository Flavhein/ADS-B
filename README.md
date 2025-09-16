# ADS-B

This project is a custom module for PyStreamPU

Implementation of PPM or PM modulation use in the aircraft's ADS-B system to transmit information


Install pystreampu and pybind11 in order to use it

https://github.com/aff3ct/pystreampu


```
pip install pybind11
```

### How to run it :

After activation of your python environnement, create the build directory to run this command in order to generate the Makefile :

```
CMAKE_PREFIX_PATH=$(python -c "import streampu; print(streampu.get_cmake_dirs())") cmake ..
```

Then

```
make
```

And

```
cp ads_b*.so $(pip show streampu | grep Location | cut -d' ' -f2)
``` 



An example of use is shown in the chaine_PM.py file


The graph presenting the processing aimed to be implemented :

![Diagramme](simple_double_voie.svg)
