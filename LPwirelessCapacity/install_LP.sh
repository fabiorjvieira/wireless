g++ -D__GXX_EXPERIMENTAL_CXX0X__ -I"library/include" -O3 -g3 -Wall -c -fmessage-length=0 -std=gnu++11 -MMD -MP -MF"main.d" -MT"main.d" -o "main.o" "main.cpp"
g++ -D__GXX_EXPERIMENTAL_CXX0X__ -I"library/include" -O3 -g3 -Wall -c -fmessage-length=0 -std=gnu++11 -MMD -MP -MF"network.d" -MT"network.d" -o "network.o" "network.cpp"
g++ -L"library/lib" -lemon -o "LPwirelessCapacity"  ./main.o ./network.o   -lglpk

