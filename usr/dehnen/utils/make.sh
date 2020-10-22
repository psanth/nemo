#  -*- makefile -*-
#
#  make variables for the install of falcON
#  this file is controlled by NEMO's configure since Version 4.2 (dec 2019)


# if you want --enable-native it should say "NO_ARCH_NATIVE=0"
NO_ARCH_NATIVE=1

# if you want --fopenmp it should say "OPENMP=1"
noOPENMP=1

# if you get "undefined symbol: GOMP_parallel"
# may need to add -lgomp as NEMO needed for the body/pot plugins 
