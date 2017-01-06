#ifndef PROLOG_H
#define PROLOG_H

#include <winsock2.h>
#include <iostream>
#include <string>
#include <sstream>

#define SSTR( x ) dynamic_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

#define IPADDRESS "127.0.0.1"
#define PORT 60070

using namespace std;

class PrologConnection
{
public:
	PrologConnection(){}
	SOCKET m_socket;
	bool socketConnect();
	void sendTo(char* s, int len);
	void receiveFrom(char* ans);
	void quit();

	int sendStartingPosition(bool player, int x, int y, string Board);
	int sendDestination(bool player, int px, int py, int tx, int ty, int play, string Board);
	int mustSavePiece(bool player, string Board);
	int mandatoryMovement(bool player, int x, int y, int play, string Board);
	int victoryCondition(bool player, string Board, int capW, int capB);
	int possibleMove(bool player, string Board, int px, int py);
};

#endif