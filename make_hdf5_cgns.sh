#!/bin/bash

#Use git tags
hdf5ver="1_10_10"
cgnsver="v3.3.1"
jobs=24

hdf5dir=$PWD/hdf5/hdf5_$hdf5ver
cgnsdir=$PWD/CGNS/cgns_$cgnsver

libdir="cgnsConverter/lib_cgns"$cgnsver"_hdf"$hdf5ver

make_hdf5()
{
	#git clone https://github.com/HDFGroup/hdf5.git
	cd hdf5
	make distclean
	export CC='cc -w -m64'
	export CFLAGS='-fPIC'
	git checkout hdf5-$hdf5ver && \
	./configure \
		--prefix=$hdf5dir  \
		--enable-shared    \
		--enable-zlib      \
		--disable-fortran  \
		--disable-hl       \
		--enable-tests     \
		--disable-tools    \
		--disable-parallel \
	&& make -j$jobs && make install || exit 1

	echo "HDF5 lib is build in $hdf5dir"

	cd ..
}

make_cgns() 
{
	#git clone https://github.com/CGNS/CGNS.git
	cd CGNS/src
	make distclean
	
	export CFLAGS='-fPIC'
	git checkout $cgnsver && \	
	./configure \
		--prefix=$cgnsdir    \
		--enable-shared=all  \
		--enable-64bit       \
		--with-zlib=lz       \
		--with-hdf5=$hdf5dir \
		--with-fortran=no    \
	&& make -j12 && make install || exit 1
	
	echo "CGNS lib is build in $cgnsdir"
	cd ../.. 
}

make_hdf5 && make_cgns

mkdir $libdir
cp $cgnsdir/lib/libcgns.so $libdir
cp $hdf5dir/lib/libhdf5.so $libdir


