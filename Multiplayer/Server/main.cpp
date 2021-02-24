//Copyright 2021. 김교일 all rights reserved.

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <math.h>
#include<string.h>
#include<sys/time.h>

#define mapX 10
#define mapY 10
#define mapS 100
#define PI 3.1415926535
#define P2 PI/2
#define P3 3*PI/2
#define DR 0.0174533 //one degree in radian
#define max_clients 30

//COMMAND SET
//"PLAYERID" == 0
//"TOTAlPLAYER" == 1
//"DATA" == 2
//"ATTACK" == 3
//"DOOR" == 4
//"MAP" == 5
//"HP" == 6

#pragma comment(lib, "Ws2_32.lib")
//this is a server

WSADATA wsa;
SOCKET master , new_socket , client_socket[max_clients] , s;
struct sockaddr_in server, address;
int /*max_clients = 30 ,*/ activity, addrlen, i, valread;
char message[1000];

//size of our receive buffer, this is string length.
int MAXRECV = 1024;
//set of socket descriptors
fd_set readfds;
//1 extra for null character, string termination
char *buffer;
unsigned long long last_send_time;

int totalplayer;

float px_arr[max_clients]={0};
float py_arr[max_clients]={0};
float pa_arr[max_clients]={0};
int pMI_arr[max_clients]={0};
int pHP_arr[max_clients]={0};

int mapW[mapS]=
{
1,1,1,1,1,1,1,1,1,1,
1,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,1,
1,1,1,1,1,1,1,1,1,1,
};
//---------------
void send_total_player()
{
    int temp=0;
    //send total player
    for(int k=0; k<max_clients;k++)
    {
        if(client_socket[k]!=0)
        {
            temp++;
        }
    }
    sprintf(message, "T %d \n", temp);
    for(int j=0;j<max_clients;j++)
    {
        if(client_socket[j]!=0)
        {
            send(client_socket[j], message, strlen(message), 0);
        }
    }
    totalplayer=temp;
}


float dist(float ax, float ay, float bx, float by)
{
    return( sqrt((bx-ax)*(bx-ax)+(by-ay)*(by-ay)) );
}
int floatcomp(const void* elem1, const void* elem2) //내림차순 정렬
{
    if(*(const float*)elem1 > *(const float*)elem2)
        return -1;
    return *(const float*)elem1 < *(const float*)elem2;
}

int floatcomp_rev(const void* elem1, const void* elem2) //오름차순 정렬
{
    if(*(const float*)elem1 < *(const float*)elem2)
        return -1;
    return *(const float*)elem1 > *(const float*)elem2;
}


void playerAttack(int id, float px, float py, float pa)
{
    float arr[max_clients]={0};
    for(int i=0; i<max_clients; i++)
    {
        float mx=px_arr[i];
        float my=py_arr[i];
        float disT=dist(px, py, mx, my);
        double ma=atan2((double)(my-py), (double)(mx-px));
        double theta=(my-py)/(mx-px);

        if((my-py)>0 && (mx-px)>0){ma=atan(theta);}
        else if((my-py)>0 && (mx-px)<0){ma=PI+atan(theta);}
        else if((my-py)<0 && (mx-px)<0){ma=PI+atan(theta);}
        else if((my-py)<0 && (mx-px)>0){ma=2*PI+atan(theta);}


        int mix,miy, mp, dof; float rx, ry , xo,yo, disTR;
        double ra=ma; if(ra<0){ra+=2*PI;} if(ra>2*PI){ra-=2*PI;}

        int vmt=0;
        int hmt=0;//vertical and horizontal map texture number, ooo map texture
        //----check horizontal lines
        dof=0;
        float disH=1000000,hx=px, hy=py;
        float aTan=-1/tan(ra);
        if(ra>PI){ry=(((int)py>>6)<<6)-0.001; rx=(py-ry)*aTan+px; yo=-64; xo=-yo*aTan;} //looking down
        if(ra<PI){ry=(((int)py>>6)<<6)+64; rx=(py-ry)*aTan+px; yo=64; xo=-yo*aTan;} //looking up
        if(ra==0 || ra==PI){rx=px;ry=py;dof=8;}
        while(dof<8)
        {
            mix=(int)(rx)>>6; miy=(int)(ry)>>6; mp=miy*mapX+mix;
            if(mp>0 && mp<mapX*mapY && mapW[mp]>0){hmt=mapW[mp]-1;hx=rx; hy=ry; disH=dist(px,py,hx,hy); dof=8;}//hit wall
                else{rx+=xo;ry+=yo;dof+=1;} //next line
        }

        //check vertical lines
        dof=0;
        float disV=1000000,vx=px, vy=py;
        float nTan=-tan(ra);
        if(ra>P2 && ra<P3){rx=(((int)px>>6)<<6)-0.001; ry=(px-rx)*nTan+py; xo=-64; yo=-xo*nTan;} //looking left
        if(ra<P2 || ra>P3){rx=(((int)px>>6)<<6)+64; ry=(px-rx)*nTan+py; xo=64; yo=-xo*nTan;} //looking right
        if(ra==0 || ra==PI){rx=px;ry=py;dof=8;} //looking straight up or down
        while(dof<8)
        {
            mix=(int)(rx)>>6; miy=(int)(ry)>>6; mp=miy*mapX+mix;
            if(mp>0 &&mp<mapX*mapY && mapW[mp]>0){vmt=mapW[mp]-1;vx=rx; vy=ry; disV=dist(px,py,vx,vy);dof=8;}//hit wall

            else{rx+=xo;ry+=yo;dof+=1;} //next line
        }
        if(disV<disH){hmt=vmt;rx=vx; ry=vy;disTR=disV;}
        if(disH<disV){rx=hx; ry=hy;disTR=disH;}
        //------------------------------------

        if(((pa-ma<0)&&(pa-ma>-PI/36)) || ((pa-ma>0)&&(pa-ma<PI/36)))
        {
            if(disTR>disT){arr[i]=disT;}
            else{arr[i]=1000000;}
        }
        else
        {
            arr[i]=1000000;
        }
    }
    qsort(arr, max_clients, sizeof(float), floatcomp_rev);
    //printf("%f %f %f %f %f %f %f\n", arr[0], arr[1], arr[2], arr[3], arr[4], arr[5], arr[6]);
    if(arr[0]!=1000000)
    {
        for(int i=0;i<max_clients; i++)
        {
            if(id==i) continue;
            if(arr[0]==dist(px, py, px_arr[i], py_arr[i]) )
            {
                pHP_arr[i]-=4;
                //printf("H %d %d\n", i, pHP_arr[i]);
                //sprintf(message, "H %d \n",pHP_arr[i]);
                //send(client_socket[i], message, strlen(message), 0);
                if(pHP_arr[i]<=0)
                {
                    /*
                    //px_arr[i]=-10; py_arr[i]=-10;
                    sprintf(message, "K %d \n", i); //KILLED id
                    for(int j; j<max_clients; j++)
                    {
                        if(client_socket[j]==0) continue;
                        send(client_socket[j], message, strlen(message), 0);
                    }

                    */
                }
                break;
            }
        }
    }
}



//---------------

int main(int argc , char *argv[])
{
    buffer =  (char*) malloc((MAXRECV + 1) * sizeof(char));

	for(i = 0 ; i < 30;i++)
	{
		client_socket[i] = 0;
	}

	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		printf("Failed. Error Code : %d",WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	printf("Initialised.\n");

	//Create a socket
	if((master = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d" , WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	printf("Socket created.\n");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( 8888 );

	//Bind
	if( bind(master ,(struct sockaddr *)&server , sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d" , WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	puts("Bind done");

	//Listen to incoming connections
	listen(master , 3);

	//Accept and incoming connection
	puts("Waiting for incoming connections...");

	addrlen = sizeof(struct sockaddr_in);

	while(TRUE)
    {
/*
        struct timeval time_now{};
        gettimeofday(&time_now, nullptr);
        unsigned long long current_time=((unsigned long long)time_now.tv_sec * 1000) + ((unsigned long long)time_now.tv_usec / 1000);
        if(current_time-last_time<=2) continue;
        last_time=current_time;
*/
        //clear the socket fd set
        FD_ZERO(&readfds);

        //add master socket to fd set
        FD_SET(master, &readfds);

        //add child sockets to fd set
        for (  i = 0 ; i < max_clients ; i++)
        {
            s = client_socket[i];
            if(s > 0)
            {
                FD_SET( s , &readfds);
            }
        }

        //wait for an activity on any of the sockets, timeout is NULL , so wait indefinitely
        activity = select( 0 , &readfds , NULL , NULL , NULL);

        if ( activity == SOCKET_ERROR )
        {
            printf("select call failed with error code : %d" , WSAGetLastError());
			exit(EXIT_FAILURE);
        }

        //If something happened on the master socket , then its an incoming connection
        if (FD_ISSET(master , &readfds))
        {
            if ((new_socket = accept(master , (struct sockaddr *)&address, (int *)&addrlen))<0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            //inform user of socket number - used in send and receive commands
            printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
/*
            //send new connection greeting message
            if( send(new_socket, message, strlen(message), 0) != strlen(message) )
            {
                perror("send failed");
            }

            puts("Welcome message sent successfully");
*/
            //add new socket to array of sockets
            for (i = 0; i < max_clients; i++)
            {
                if (client_socket[i] == 0)
                {
                    client_socket[i] = new_socket;
                    printf("Adding to list of sockets at index %d \n" , i);

                    //변수 초기화 의미가 있나? 있는듯
                    //px[i]=100; py[i]=100; pa[i]=3*PI/2;
                    pHP_arr[i]=100;

                    //send player number
                    sprintf(message, "P %d \n", i);
                    send(client_socket[i], message, strlen(message),0);
                    //send total player
                    send_total_player();

                    //init HP
                    sprintf(message, "H %d \n", pHP_arr[i]);
                    send(client_socket[i], message, strlen(message), 0);


                    break;
                }
            }
        }

        //else its some IO operation on some other socket :)
        for (i = 0; i < max_clients; i++)
        {
            s = client_socket[i];
			//if client presend in read sockets
            if (FD_ISSET( s , &readfds))
            {
                //get details of the client
				getpeername(s , (struct sockaddr*)&address , (int*)&addrlen);

				//Check if it was for closing , and also read the incoming message
				//recv does not place a null terminator at the end of the string (whilst printf %s assumes there is one).
                valread = recv( s , buffer, MAXRECV, 0);

				if( valread == SOCKET_ERROR)
				{
					int error_code = WSAGetLastError();
					if(error_code == WSAECONNRESET)
					{
						//Somebody disconnected , get his details and print
						printf("Host disconnected unexpectedly , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

						//Close the socket and mark as 0 in list for reuse
						closesocket( s );
						client_socket[i] = 0;
						send_total_player();
					}
					else
					{
						printf("recv failed with error code : %d" , error_code);
					}
				}
				if ( valread == 0)
                {
                    //Somebody disconnected , get his details and print
                    printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

                    //Close the socket and mark as 0 in list for reuse
                    closesocket( s );
                    client_socket[i] = 0;
                    send_total_player();
                }

                //Echo back the message that came in
                else
                {
					//add null character, if you want to use with printf/puts or other string handling functions
					buffer[valread] = '\0';
					//printf("%s:%d - %s \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port), buffer);

					char *ptr =strtok(buffer, " ");

                    if(ptr[0]=='A')//ATTACK id
                    {
                        int id;
                        ptr=strtok(NULL, " ");
                        id=atoi(ptr);
                        //printf("attack attack %d", id);
                        playerAttack(id, px_arr[id], py_arr[id], pa_arr[id]);
                    }

					else if(ptr[0]=='D') //DATA id x y a
                    {
                        int id;
                        float x,y,a;

                        ptr=strtok(NULL, " ");
                        id=atoi(ptr);

                        ptr=strtok(NULL, " ");
                        x=atof(ptr);

                        ptr=strtok(NULL, " ");
                        y=atof(ptr);

                        ptr=strtok(NULL, " ");
                        a=atof(ptr);

                        px_arr[id]=x; py_arr[id]=y; pa_arr[id]=a;
/*
                        for(int j=0;j<max_clients;j++)
                        {
                            if(s!=client_socket[j]) send( client_socket[j] , buffer , valread , 0 );//보내온 놈 빼고 전달
                        }*/

                    }
                    else if(ptr[0]=='R')//DOOR 문 인덱스(mapW[index])
                    {
                        int mi;
                        ptr=strtok(NULL, " ");
                        mi=atoi(ptr);
                        if(mapW[mi]==4){mapW[mi]=0;}
                    }


                    //이제 데이터를 업데이트 시켜 주자
                    //맵, 자신 HP, 다른 클라이언트 위치
                    /*
                    if(
					for(int j=0;j<max_clients;j++)
                    {
                        if(s!=client_socket[j]) send( client_socket[j] , buffer , valread , 0 );//보내온 놈 빼고 전달
                    }
                    */
					//send( s , buffer , valread , 0 );
                }
            }
        }

        //클라이언트 데이터 업데이트
        //D 다른 클라이언트 위치
/*
        struct timeval time_now{};
        gettimeofday(&time_now, nullptr);
        unsigned long long current_time=((unsigned long long)time_now.tv_sec * 1000) + ((unsigned long long)time_now.tv_usec / 1000);
        if(current_time-last_send_time>=5)
        {
        last_send_time=current_time;
        */
        for(i=0; i<max_clients; i++)
        {
            s=client_socket[i];
            if(s==0) continue;
            char temp[1000];

            //TOTALPLAYER num /HP num /DATA id x y id x y id x y ...
            sprintf(message, "T %d ", totalplayer);
            sprintf(temp, "H %d ", pHP_arr[i]);
            strcat(message, temp);

            sprintf(temp, "D ");
            strcat(message, temp);
            for(int j=0;j<max_clients;j++)
            {
                if(j==i) continue;
                if(client_socket[j]==0) continue;
                char temp[1000];
                sprintf(temp, "%d %f %f ", j, px_arr[j], py_arr[j]);
                strcat(message, temp);
            }
            char end[]="\n";
            strcat(message, end);
            if(strlen(message)<=3) continue;
            send(client_socket[i], message, strlen(message),0);
        }


    }

	closesocket(s);
	WSACleanup();

	return 0;
}
