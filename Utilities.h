#ifndef UTILITIES_H
#define UTILITIES_H
#include "VirtualMachine.h"


double Precision = .001;
double infinity=3.40282347E+38;


//Checks whether the differnce of the two values given is less than the default precision defined, so that we can conclude they are same. 
bool AreSame(float Value1, float Value2)
{
  if(abs(Value1-Value2)<Precision)
    return true;
  return false;

}

//Checks with the given precision the values 1 and 2. 
bool AreSame(float Value1, float Value2, float precision)
{
  if(abs(Value1-Value2)<precision)
    return true;
  return false;

}

#endif