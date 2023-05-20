/*
 * Copyright (C) 2018-2022 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without 
 * the express written permission of the copyright holder.
 */
//https://stackoverflow.com/questions/12397175/how-do-i-close-an-open-port-from-the-terminal-on-the-mac

//code obtained from lecture slides
//for sources on radix sort implementation, see Assignment 4


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <math.h>
#include <list>
#include <thread>

#include <iostream>
#include "radix.h"

using namespace std;


//returns the digit of the placeth significant figure (from the left) of element
//returns -1 if place is greater than signums, to signify that nothing is less than zero
int getSignum(unsigned int element, unsigned int place)
{
  unsigned int sigfigs = floor(log10(element) + 1);

  unsigned int temp = element;

  if(place > sigfigs)
  {
    return -1;
  }
  temp = element;

  for(unsigned int i = 0; i < sigfigs - place; i++)
  {
    temp /= 10;
  }

  return temp%10;
}

//used this video to understand radix sort, then changed most everything about it 
//https://www.youtube.com/watch?v=4ungd6NXFYI
void radixSort(vector<unsigned int> &vectorWrap, unsigned int sigfig)
{
  vector<unsigned int> output;

  for(int i = -1; i <=9; i++)
  {
    for(unsigned int &element : vectorWrap)
    {
      if(getSignum(element, sigfig) == i)
      {
        output.push_back(element);
      }
    }
  }

  for(unsigned int i = 0; i < output.size(); i ++)
  {
    vectorWrap.at(i) = output[i];
  }

  output.clear();
}

void startRadix(vector<unsigned int> &vectorWrap)
{
  for(int i = 10; i >=1; i--)
    radixSort(vectorWrap, i);
}

void display(auto &vectorWrap)
{
  for(auto &element : vectorWrap)
  {
    clog << element << endl;
  
  }
  clog << endl;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


RadixServer::RadixServer(const int port, const unsigned int cores) {
  //cout << cores << endl;
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  unsigned int on_wire = 0 * cores;

  vector<unsigned int> numList;

  struct sockaddr_in server_addr;
  bzero((char*) &server_addr, sizeof(server_addr));

  //cout << "server made it here" << endl;

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port);

  if(bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
    exit(-1);

  
  //cout << "server made it here" << endl;

  listen(sockfd, 5);


  for(;;)
  {
    //cout << "running infinite\n";
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);

    int newsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &len);
    if(newsockfd < 0)
    {
      perror("error");  
      exit(-1);
    }
    for(;;)
    {
      int errorCheck = recv(newsockfd, (void*)&on_wire, sizeof(unsigned int), 0);

      if(errorCheck < 0)
      {
        perror("error");
        exit(-1);
      }
      if(errorCheck == 0)
        break;
      unsigned int local = ntohl(on_wire);
      
      if(local == 0)
      {
        startRadix(numList);
        numList.push_back(0);
        //cout << "got a zero\n";
        for(auto &element : numList)
        {
          on_wire = htonl(element);
          if(send(newsockfd, (void*)&on_wire,sizeof(unsigned int), 0) < 0)
          {
            perror("error");
            exit(-1);
          }
        }
        
        numList.clear();
        //break;
      }
      else
        numList.push_back(local);
    }
    close(newsockfd);
  }

  cout << "\nserver finished\n\n";

  close(sockfd);


}

void RadixClient::msd(
const char *hostname,
const int port,
std::vector<std::reference_wrapper<std::vector<unsigned int>>> &lists) {
 
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
 
  struct hostent *server = gethostbyname(hostname);

  struct sockaddr_in serv_addr;

  vector<unsigned int> solutions;

  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *) server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);

  serv_addr.sin_port = htons(port);

  if(connect(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
  {
    perror("error");
    exit(-1);
  }  
  unsigned int number = 1024;
  unsigned int on_wire;
  
  

  for(unsigned int j = 0; j < lists.size(); j ++)
  {
    for(auto &element : lists[j].get())
    {
      number = element;
      on_wire = htonl(number);
      send(sockfd, (void*)&on_wire, sizeof(unsigned int), 0);
    }
    number = 0;
    on_wire = htonl(number);
    send(sockfd, (void*)&on_wire, sizeof(unsigned int), 0);
  
  
    for(;;)
    {
      recv(sockfd, (void*)&on_wire, sizeof(unsigned int), 0);
      number = ntohl(on_wire);
      if(number == 0)
        break;
      solutions.push_back(number);
      //printf("Client recieved: %u\n", number);
    }
    

    for(unsigned int i = 0; i < lists[j].get().size(); i++)
    {
      lists[j].get().at(i) = solutions[i];
    }
    startRadix(lists[j]);
    solutions.clear();
  }
  cout << "Client finished" << endl;
  close(sockfd);
}
