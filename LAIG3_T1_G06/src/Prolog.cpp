#include "Prolog.h"

bool PrologConnection::socketConnect()
{
	WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != NO_ERROR)
		printf("Client: Error at WSAStartup().\n");
    else
        printf("Client: WSAStartup() is OK.\n");

    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socket == INVALID_SOCKET) {
        printf("Client: socket() - Error at socket(): %ld\n", WSAGetLastError());
        WSACleanup();
        return false;
    }
	else
       printf("Client: socket() is OK.\n");

    // Connect to a server.
    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    // Just test using the localhost, you can try other IP address
    clientService.sin_addr.s_addr = inet_addr("127.0.0.1");
    clientService.sin_port = htons(60070);

    if (connect(m_socket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
        printf("Client: connect() - Failed to connect.\n");
        WSACleanup();
        return false;
    }
    else {
       printf("Client: connect() is OK.\n");
       printf("Client: Can start sending and receiving data...\n");
    }

    // Send and receive data.
	printf("Connected\n");
	return true;
}

void PrologConnection::sendTo(char *s, int len) {
	int bytesSent = send(m_socket, s, len, 0);
	if(bytesSent == SOCKET_ERROR)
		printf("Client: send() error %ld.\n", WSAGetLastError());
}

void PrologConnection::receiveFrom(char *ans) {
	int bytesRecv = SOCKET_ERROR;
	int pos = 0;
	while (true) {
		recv(m_socket, &ans[pos], 1, 0);
		if (ans[pos] == '\n')
			break;
		pos++;
	}
	ans[pos] = 0;
	cout << "prolog answered: " << ans << endl;
}

void PrologConnection::quit() {
	cout << "Asking prolog to quit" << endl;
	char buff[] = "quit.\n";
	sendTo(buff, 6);
	char ans[128];
	receiveFrom(ans);
}

int PrologConnection::sendStartingPosition(bool player, int x, int y, string Board)
{
	string command = "";
	command += "inputStartPosition(";
	if(player)
		command += "true,";
	else
		command += "false,";
	command += Board + ",";//"-1-1,";
	command += SSTR(x) + "-" + SSTR(y) + ").\n";

	char answer[1024];
	strcpy_s(answer, command.c_str());
	this->sendTo(answer, strlen(answer));
	
	char ans[128];
	this->receiveFrom(ans);
	
	for(unsigned int i=0; i < strlen(ans); i++)
	{
		if(ans[i] == '-')
			return -1;
	}
	return 1;
}

int PrologConnection::sendDestination(bool player, int px, int py, int tx, int ty, int play, string Board)
{
	string command = "";
	command += "inputDestination(";
	if(player)
		command += "true,";
	else
		command += "false,";
	command += Board + ","; // "-1-1,";
	command += SSTR(px) + "-" + SSTR(py) + ", ";
	command += SSTR(tx) + "-" + SSTR(ty) + ", ";
	command += SSTR(play) + ").\n";

	char answer[1024];
	strcpy_s(answer, command.c_str());
	this->sendTo(answer, strlen(answer));
	
	char ans[128];
	this->receiveFrom(ans);
	
	for(unsigned int i=0; i < strlen(ans); i++)
	{
		if(ans[i] == '6') return 6;
		else if(ans[i] == '7') return 7;
		else if(ans[i] == '8') return 8;
		else if(ans[i] == '9') return 9;
	}
	return -1;
	
}

int PrologConnection::mustSavePiece(bool player, string Board)
{
	string command = "";
	command += "mustSavePiece(";
	if(player)
		command += "true,";
	else
		command += "false,";
	//command += Board + "-1-1).\n";
	command += Board + ").\n";

	char answer[1024];
	strcpy_s(answer, command.c_str());
	this->sendTo(answer, strlen(answer));
	
	char ans[128];
	this->receiveFrom(ans);
	for(unsigned int i=0; i < strlen(ans); i++)
	{
		if(ans[i] == '-')
			return -1;
	}
	return 1;
}

int PrologConnection::mandatoryMovement(bool player, int x, int y, int play, string Board)
{
	string command = "";
	command += "mustMoveAgain(";
	if(player)
		command += "true,";
	else
		command += "false,";
	//command += Board + "-1-1,";
	command += Board + ",";
	command += SSTR(play) + ",";
	command += SSTR(x) + "-" + SSTR(y) + ").\n";

	char answer[1024];
	strcpy_s(answer, command.c_str());
	this->sendTo(answer, strlen(answer));
	
	char ans[128];
	this->receiveFrom(ans);
	for(unsigned int i=0; i < strlen(ans); i++)
	{
		if(ans[i] == '-')
			return -1;
	}
	return 1;
}

int PrologConnection::victoryCondition(bool player, string Board, int capW, int capB)
{
	printf("PIECE COUNT: %d %d\n", capW, capB);
	int count;
	if(player) count = 16 - capW;
	else count = 16 - capB;
	string command = "";
	command += "victoryCondition(";
	if(player)
		command += "true,";
	else
		command += "false,";
	command += Board + ").\n";

	char answer[1024];
	strcpy_s(answer, command.c_str());
	this->sendTo(answer, strlen(answer));
	
	char ans[128];
	for(int i = 0; i < count; i++)
	{
		this->receiveFrom(ans);
	}
	for(unsigned int i=0; i < strlen(ans); i++)
	{
		if(ans[i] == '-')
			return -1;
	}
	return 1;
}

int PrologConnection::possibleMove(bool player, string Board, int px, int py)
{
	string command = "";
	command += "possibleMove(";
	if(player)
		command += "true,";
	else
		command += "false,";
	command += Board + ",";
	command += SSTR(px) + "-" + SSTR(py) + ").\n";

	char answer[1024];
	strcpy_s(answer, command.c_str());
	this->sendTo(answer, strlen(answer));
	
	char ans[128];
	this->receiveFrom(ans);
	for(unsigned int i=0; i < strlen(ans); i++)
	{
		if(ans[i] == '-')
			return -1;
	}
	return 1;
}