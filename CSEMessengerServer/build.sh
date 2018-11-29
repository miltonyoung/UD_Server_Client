# build.sh

if [ $# -eq 0 ]; then 
	if [! -f manager.exe ]; then
		rm receive.exe
	fi
	 g++ -o send.exe SocketConnection.cpp udp_send.cpp
else
	if [ "$1" == "clean" ]; then
		echo cleaning
		rm send.exe
	elif [ "$1" == "run" ]; then
		echo "clean->Compile->executing"
		rm send.exe
		g++ -o send.exe SocketConnection.cpp udp_send.cpp 
		./send.exe
	else
		g++ -o send.exe SocketConnection.cpp udp_send.cpp
	fi
	
fi

