# CgnsConverter
Converts CGNS/HDF5 file versions

CGNS and HDF5 libs lack backward and forward compability in spite of used features in a file. Converter is a workaround.

## Usage:

1. Build sources using `cmake`
2. Build CGNS & HDF5 pairs of required versions using script `make_hdf5_cgns.sh`. The versions are defined by `hdf5ver` & `cgnsver` variables in the script.
3. Put lib directories with executable
4. Run `cgns-convert` from executable dir
