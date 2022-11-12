#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <errno.h>
#include <chrono>
#include <thread>
#include <sstream>
#include <limits>

#define PORT_DEFAULT 5437
#define IP_DEFAULT "127.0.0.1"
#define TIMEOUT_DEFAULT 3

using namespace std;
int row;
int col;
char* display_buffer;
bool seating_available = true;

int port_num = PORT_DEFAULT;
string ip_addr = IP_DEFAULT;
int timeout = TIMEOUT_DEFAULT;
bool automatic_mode = false;
string ini_filename = "";

void display_seating(int sock)
{
	int valread = read(sock, display_buffer, row * col * sizeof(char));
	char signal = display_buffer[0];
	if (signal != 'u' && signal != 'a')
	{
		cout << "[SEATING FULL]" << endl;
	}
	else
	{
		for (int i = 0; i < row; i++)
		{
			for (int j = 0; j < col; j++)
			{
				cout << display_buffer[i * col + j];
			}
			cout << endl;
		}
	}
}
int connect_socket(int sock, string ip_addr, int port_num, int timeout)
{
	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port_num);

	if (inet_pton(AF_INET, ip_addr.c_str(), &serv_addr.sin_addr) <= 0)
	{
		cout << "Invalid address or address is not supported" << endl;
		return -1;
	}

	//try to connect to server 
	chrono::steady_clock::time_point clock_begin = chrono::steady_clock::now();
	bool connection_timedout = false;
	int client_fd = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	while (client_fd < 0 && !connection_timedout)
	{
		std::this_thread::sleep_for(chrono::milliseconds(500));
		cout << "Connection failed, retrying connection" << endl;
		client_fd = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

		chrono::steady_clock::time_point clock_end = chrono::steady_clock::now();
		connection_timedout = chrono::duration_cast<chrono::seconds> (clock_end - clock_begin).count() > timeout;
	}
	if (connection_timedout)
	{
		cout << "Connection timed out!" << endl;
		return -1;
	}
	return client_fd;
}
string remove_spaces(string str)
{

	//str.erase((remove(str.begin(),str.end(),isspace)), str.end());
	return str;
}
bool try_purchase_seat(int sock, int i, int j)
{
	int numbBuffer[2];
	numbBuffer[0] = i;
	numbBuffer[1] = j;
	write(sock, numbBuffer, sizeof(numbBuffer));
	char selection_buffer[1];
	read(sock, selection_buffer, sizeof(char));
	switch (selection_buffer[0])
	{
	case 'a':
		cout << "purchase confirmed for seat " << numbBuffer[0] << ", " << numbBuffer[1] << endl;
		return true;
		break;
	case 'u':
		cout << "selection unavailable, please try a different seat." << endl;
		break;
	case 'o':
		cout << "selection outside of bounds" << endl;
		break;
	case 'f':
		cout << "seating full, purchases are now closed" << endl;
		seating_available = false;
	}
	return false;
}
void automatic_purchase_mode(int sock)
{
	while (seating_available)
	{
		display_seating(sock);
		int wait_time = (rand() % 3) * 2 + 3; // roll for a 3, 5, or 7
		this_thread::sleep_for(chrono::seconds(wait_time));
		int i = rand() % row;
		int j = rand() % col;
		try_purchase_seat(sock, i, j);
	}
}
void manual_purchase_mode(int sock)
{
	while (seating_available)
	{
		display_seating(sock);
		cout << "Please input where you would like to purchase a seat." << endl;
		bool good_input = false;
		int i, j;
		cout << "Row: ";
		cin >> i;
		cout << "Column: ";
		cin >> j;
		while (cin.fail())
		{
			cout << "bad input, please try making purchase again" << endl;
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Row: ";
			cin >> i;
			cout << "Column: ";
			cin >> j;
		}

		while (false);
		try_purchase_seat(sock, i, j);
	}
}

int main(int argc, char* argv[])
{
	srand(time(NULL));
	if (argc >= 2)
	{
		//select mode 
		automatic_mode = argv[1][0] == 'a';

	}
	if (argc == 3)
	{
		ini_filename = argv[2];
		try
		{
			ifstream myinifile(ini_filename);
			string line;
			while (getline(myinifile, line))
			{
				istringstream iss(line);
				//cout << line << endl;
				string key;
				if (getline(iss, key, ' '))
				{
					string data;
					getline(iss, data, ' ');
					// skip the '='
					if (data == "=")
					{
						getline(iss, data, ' ');
					}
					//cout << data << endl;

					if (key == "IP")
					{
						ip_addr = data;
						//cout << "IP ADDRESS IS: " << data << endl;
					}
					else if (key == "Port")
					{
						port_num = stoi(data);
						//cout << "PORT NUMBER IS: " << data << endl;

					}
					else if (key == "Timeout")
					{
						timeout = stoi(data);
						//cout << "TIMEOUT IS: " << data << endl;

					}
				}

			}
		}
		catch (exception& e)
		{
			cout << "error when trying to read ini file: " << ini_filename << "\nExiting\n";
			return -1;
		}
	}
	if (argc > 3)
	{
		cout << "Too many arguments given" << endl;
		return -1;
	}
	if (automatic_mode)
	{
		cout << "entering automatic mode" << endl;
	}
	else
	{
		cout << "entering manual mode" << endl;
	}
	int sock = 0, valread;
	string msg = "hello from client";
	//char *message = msg.c_str() ;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		cout << "socket creation error" << endl;
		return -1;
	}
	int client_fd = connect_socket(sock, ip_addr, port_num, timeout);
	if (client_fd < 0)
	{
		cout << "error when attempting to connect to the socket" << endl;
		return -1;
	}
	//getting size of seating area
	int numbBuffer[2];
	valread = read(sock, numbBuffer, 2 * sizeof(int));
	row = numbBuffer[0];
	col = numbBuffer[1];
	display_buffer = (char*)malloc(row * col * sizeof(char));
	//display_seating(sock);

	//negotiating seatting
	if (automatic_mode)
	{
		automatic_purchase_mode(sock);
	}
	else
	{
		manual_purchase_mode(sock);
	}
	close(client_fd);
	cout << "exiting client" << endl;
	return 0;
}