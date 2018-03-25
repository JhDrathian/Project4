all : oss child_process

oss : oss.cpp
	g++ oss.cpp -o oss
child_process : child_process.cpp
	g++ child_process.cpp -o child_process
