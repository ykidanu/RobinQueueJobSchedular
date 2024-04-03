run : RATCH
	./RATCH
RATCH : project3.cpp
	g++ -std=c++11 project3.cpp -o RATCH
