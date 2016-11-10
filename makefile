FlowMake : transmitter.cpp T1_rx_v05.cpp
	g++ -pthread -o receiver T1_rx_v05.cpp
	g++ -pthread -o transmitter transmitter.cpp
