#IF(POLICY CMP0048)
#  CMAKE_POLICY(SET CMP0048 OLD)
#ENDIF(POLICY CMP0048)

path=`pwd`
tar -xzvf lemon-1.3.1.tar.gz 
cd lemon-1.3.1/
sed -i "s/CMAKE_POLICY(SET CMP0048 OLD)//" CMakeLists.txt 
cmake -DLEMON_ENABLE_SOPLEX=NO -DLEMON_ENABLE_COIN=NO -DLEMON_ENABLE_ILOG=NO -DCMAKE_INSTALL_PREFIX=${path}/library/
make
make install
cd ..

