#include <netinet/in.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <math.h>
#include <chrono>
#include <thread>
#include <pthread.h>

#define PORT 5437
#define DEFAULT_ROW 10
#define DEFAULT_COL 10
using namespace std;

bool seats_available = true;
bool* seating;
int row;
int col;

pthread_mutex_t lock;

void display_startup_sequence()
{
	int banner_len = 75;
	string banner[15];
	banner[0] = R"(  \----------------------------------\)";
	banner[1] = R"(   \                                  \        __)";
	banner[2] = R"(    \                                  \       | \)";
	banner[3] = R"(     >                                  >------|  \       ______)";
	banner[4] = R"(    /                                  /       --- \_____/**|_|_\____  |)" ;
	banner[5] = R"(   /                                  /          \_______ --------- __>-})";
	banner[6] = R"(  /----------------------------------/              /  \_____|_____/   |)" ;
	banner[7] = R"(                                                    *         |)";
	banner[8] = R"(                                                             {O})";
	banner[9] = R"( )";
	banner[10] = R"(       /*\       /*\       /*\       /*\       /*\       /*\       /*\)";
	banner[11] = R"(      |***|     |***|     |***|     |***|     |***|     |***|     |***|)";
	banner[12] = R"(       \*/       \*/ ____  \*/       \*/       \*/       \*/       \*/)" ;
	banner[13] = R"(        |         |  |  |   |         |         |         |         |)";
	banner[14] = R"(  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^-dr)" ;

	for (int i = banner_len; i >= 0; i--)
	{
		system("clear");
		for (int j = 0; j < 15; j++)
		{
			int banner_length = banner[j].length();
			for (int t = i; t < banner_length; t++)
			{
				cout << banner[j][t];
			}
			cout << endl;

		}
		this_thread::sleep_for(chrono::milliseconds(25));
	}
}
void display_seating()
{
	cout << " ============" << endl;
	for(int i = 0; i < row; i++)
	{
		for(int j = 0; j < col; j++)
		{
			string boxel = (seating[i * col + j]) ? "\033[1;42ma" : "u";
			cout << boxel;
			cout << "\033[0m";
		}
		cout << endl;
	}
	cout << " ============" << endl;
}
//todo: 
//bool try_purchase_seat(int i, int j) 
// check to see if the seating is full
bool check_seats_available()
{
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			if (seating[i * col + j])
			{
				return true;
			}
		}
	}
	seats_available = false;
	return false;
}
void seating_to_buffer(char* buffer_output)
{
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			buffer_output[i*col + j] = seating[i * col + j] ? 'a' : 'u';
		}
	}
}
void* client_connection(void* arg)
{
	int connfd = (int)(long)arg;
	cout << connfd << endl;
	int numbBuffer[2];
	numbBuffer[0] = row;
	numbBuffer[1] = col;
	write(connfd,numbBuffer,2*sizeof(int));

	char purchase_status_buffer[1];

	while (seats_available)
	{
		//display_seating();

		int selectionBuffer[2];
		//write available seats
		char seating_display_buffer[row * col];
		seating_to_buffer(seating_display_buffer);
		write(connfd, seating_display_buffer, row * col * sizeof(char));
		//read selection
		//reads buffer from client. keeps reading if not careful
		int valread = read(connfd,selectionBuffer, sizeof(selectionBuffer));
		//cout << valread << endl; // value read 
		// lock
		if (valread > 0)
		{
			int i = selectionBuffer[0];
			int j = selectionBuffer[1];
			cout << "attempting to purchase seat: " << i << "," << j << " from clientID: " << connfd << endl; // output selection
			cout << valread << endl;
			if (i < 0 || i >= row || j < 0 || j >= col)
			{
				//case out of bounds
				purchase_status_buffer[0] = 'o';

			}
			else if (!seating[i * col + j])
			{
				//case seat unavilable
				purchase_status_buffer[0] = 'u';

			}
			else
			{
				//case valid purchase
				purchase_status_buffer[0] = 'a';
				pthread_mutex_lock(&lock);
				seating[i * col + j] = false;
				// unlock
				pthread_mutex_unlock(&lock);
			}
			write(connfd, purchase_status_buffer, sizeof(char));
			display_seating();
		}
		
		//send confirmation
		//seats_available = false;
		check_seats_available();
	}
	cout << " no more seats available " << endl;
	// f is for fencepost, the end of seating available
	purchase_status_buffer[0] = 'f';
	write(connfd, purchase_status_buffer, sizeof(char));
	return NULL;
}

int main(int argc, char** argv)
{
	display_startup_sequence();
	if(argc != 3)
	{
		cout << "improper number of arguments given, using default 10x10" << endl;
		row = DEFAULT_ROW;
		col = DEFAULT_COL;
	}
	else
	{
		try
		{
			row = stoi(argv[1]);
			col = stoi(argv[2]);
		}
		catch (exception& err)
		{
			cout << "error while trying to read arguments" << endl;
			return -1;
		}
	}
	if (row * col <= 0)
	{
		cout << "invalid seating row and column" << endl;
		return -1;
	}
	seating = (bool*)malloc(row * col * sizeof(bool));
	memset(seating, 1, row * col * sizeof(bool)); //initialize seating to be true
	//display_startup_sequence();
	int listenfd = 0, connfd = 0;
	struct sockaddr_in serv_addr;
	char sendBuffer[1024];
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	cout << "listening on listenfd: " << listenfd << endl;
	memset(&serv_addr, '0', sizeof(serv_addr));
	memset(sendBuffer,'0', sizeof(sendBuffer));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(5437); // setting the port number

	bind(listenfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
	int counter = 0;
	listen(listenfd, 15); // number of connections i can accept
	pthread_t tid[15]; // 15 threads corresponding to connections
	pthread_mutex_init(&lock, NULL); //initialize lock
	while(true)
	{
		connfd = accept(listenfd,(struct sockaddr*) NULL, NULL);
		cout << "connection accepted: " << connfd << endl;
		pthread_create(&tid[counter], NULL, client_connection, (void*)(long)connfd);
		counter++;
	}

}