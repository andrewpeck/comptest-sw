CC=g++

default: controller fitter

controller:
	$(CC) --std=c++11 `root-config --cflags` histo_writer.cpp serial.cpp test_controller.cpp -o test_controller `root-config --glibs`

fitter:
	$(CC) --std=c++11 `root-config --cflags` fit_raws.C -o fit_raws  `root-config --glibs`

#clean:
   #  rm dls
   #  rm libdls.so

# install:
#	cp dls /usr/bin/dls
#	cp libdls.so /usr/lib/libdls.so
#	cp dls.hpp /usr/include/dls.hpp
