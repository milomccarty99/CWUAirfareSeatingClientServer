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

#define PORT 8080
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
			int diff = i  - banner[j].length();
			for (int t = i; t < banner[j].length(); t++)
			{
				cout << banner[j][t];
			}
			cout << endl;

		}
		this_thread::sleep_for(chrono::milliseconds(25));
	}
}
//todo: 
//bool try_purchase_seat(int i, int j) 
// check to see if the seating is full

void* client_connection(void* arg)
{
	int connfd = (int)(long)arg;
	cout << connfd << endl;
	while (seats_available)
	{
		//write available seats
		//read selection
		// lock
		//update seats if applicable
		// unlock
		//send confirmation
	}
	return NULL;
}

int main(int argc, char** argv)
{
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
		cout << "connection tried" << endl;
		pthread_create(&tid[counter], NULL, client_connection, (void*)(long)connfd);
		counter++;
	}

}