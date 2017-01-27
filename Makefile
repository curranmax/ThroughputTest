all: clean receiver transmitter

receiver:
	g++ -Wall -std=c++11 receiver.cpp -o receiver

transmitter:
	g++ -Wall transmitter.cpp -o transmitter

clean:
	rm -rf receiver transmitter
	