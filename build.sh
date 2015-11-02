# creates a cmake file, builds the app and the runs it.
cd build 
cmake -G Unix\ Makefiles ../src
make
./DBI $1 $2