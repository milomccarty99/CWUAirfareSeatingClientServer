#include <netinet/in.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <chrono>
#include <thread>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
using namespace std;


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

int main(int argc, char** argv)
{
	display_startup_sequence();
}