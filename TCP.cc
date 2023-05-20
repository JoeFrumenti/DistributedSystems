/*
 * Copyright (C) 2018-2022 David C. Harrison. All rights reserved.
 *
 * You may not use, distribute, publish, or modify this code without 
 * the express written permission of the copyright holder.
 */

#include <iostream>
#include <math.h>
#include <list>
#include <thread>

#include "radix.h"

using namespace std;

// void display(auto &vectorWrap)
// {
// int count = 0;
//   for(auto &element : vectorWrap)
//     {
//       clog << element << endl;
//       count ++;
//     }
//     clog <<endl << "TOTAL ELEMENTS: " << count << "\n-----------------\n\n";
// }


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

void subList(vector<unsigned int> &vectorWrap, vector<vector<unsigned int>> &container, const int lowerBound, const int upperBound)
{
  int count = 0;
  int printcount = 0;
  for(auto &element : container)
  {
    for(auto &thing : element)
    {
      if(count >=lowerBound && count < upperBound)
      {
        vectorWrap[count] = thing;
        printcount ++;
      }
      count ++;
    }
  }
  

}
void splitList(vector<unsigned int> vectorWrap, vector<vector<unsigned int>> &container, const int delimiter)
{
  for(auto &element : vectorWrap)
  {
    if(getSignum(element, 1) == delimiter)
      container[delimiter - 1].push_back(element);
  }
  //display(container[delimiter - 1]);
}

void ParallelRadixSort::msd(std::vector<std::reference_wrapper<std::vector<unsigned int>>> &lists, unsigned int cores) 
{ 
  thread threads[24];
 

  for(auto &element : lists)
  {
    int threadsAssigned = 0;
    int threadsUsed = 0;
    vector<vector<unsigned int>> container(9);

    //cout << "RUNNING LOOP\n\n\n\n\n";

    while(threadsUsed < 10)
    {
      for(unsigned int i = 0; i < cores; i++)
      {
        if(threadsUsed < 9)
          threads[threadsUsed] = thread(splitList, element, ref(container), threadsUsed + 1);
        threadsUsed ++;
      }

      for(unsigned int i = 0; i < cores; i++)
      {
        if(threadsAssigned < 9)
          threads[threadsAssigned].join();
        threadsAssigned ++;
      }

    }
    threadsUsed = 0;
    threadsAssigned = 0;

    while(threadsUsed < 10)
    {
      for(unsigned int i = 0; i < cores; i++)
      {
        if(threadsUsed < 9)
          threads[threadsUsed] = thread(startRadix, ref(container[threadsUsed]));
        threadsUsed ++;
      }
      for(unsigned int i = 0; i < cores; i++)
      {
        if(threadsAssigned < 9)
          threads[threadsAssigned].join();
        threadsAssigned ++;
      }
    } 


    threadsAssigned = 0;
    threadsUsed = 0;
    

    

    int divider = cores + element.get().size()/cores;
    
    // for(int i = 0; i <)



    for(unsigned int i = 0; i < cores; i++)
    {
      threads[i] = thread(subList, element, ref(container), divider * i, divider * (i + 1));
    }
    for(unsigned int i = 0; i < cores; i++)
    {
      threads[i].join();
    }


    // cout << "\n\nDISPLAYING ELEMENT.GET():\n";
    // display(element.get());
    
  }

}


  ///put new sublists back into original lists








 
 