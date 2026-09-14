#include "myVector.h"

#include <iostream>

void myVector::Print()
{
   std::cout << "x = " << m_x 
             << " y = " << m_y 
             << " z = " << m_z 
             << " mag = " << m_mag 
             << std::endl;
}
