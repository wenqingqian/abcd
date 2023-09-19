rm -r server/build
cd server \
	&& mkdir build\
	&& cd build\
	&& cmake ..\
	&& make