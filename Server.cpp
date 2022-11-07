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
			char display_char = seating[i*col+row] ? 'a' : 'u';
			cout << display_char;
		}
		cout << endl;
	}
	cout << " ============" << endl;
}
//todo: 
//bool try_purchase_seat(int i, int j) 
// check to see if the seating is full

void* client_connection(void* arg)
{
	int connfd = (int)(long)arg;
	cout << connfd << endl;
	int numbBuffer[2];
	numbBuffer[0] = row;
	numbBuffer[1] = col;
	write(connfd,numbBuffer,2*sizeof(int));
	char bufferSeatsAvailable[row*col];
	memset(bufferSeatsAvailable, 'u',sizeof(bufferSeatsAvailable));
	while (seats_available)
	{
		display_seating();

		int selectionBuffer[2];
		//write available seats
		write(connfd,bufferSeatsAvailable,sizeof(bufferSeatsAvailable));
		//read selection
		//read(connfd,)
		int valread = read(connfd,selectionBuffer, sizeof(selectionBuffer));
		cout << valread << endl; // value read 
		cout << "selecting seat: " << selectionBuffer[0] << "," << selectionBuffer[1] << endl; // output selection
		// lock
		pthread_mutex_lock(&lock);
		//update seats if applicable
		int i = selectionBuffer[0];
		int j = selectionBuffer[1];
		seating[i*col + j] = true;
		// unlock
		pthread_mutex_unlock(&lock);
		//send confirmation
		display_seating();
		//seats_available = false;
	}
	return NULL;
}

int main(int argc, char** argv)
{
	if(argc != 3)
	{
		cout << "improper number of arguments given" << endl;
		return -1;
	}
	try
	{
		row = stoi(argv[1]);
		col = stoi(argv[2]);
	}
	catch (exception &err)
	{
		cout<< "error while trying to read arguments" << endl;
		return -1;
	}
	seating = (bool*)malloc(row * col * sizeof(bool));

	//cout << row << " " << col << endl;
	display_startup_sequence();
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