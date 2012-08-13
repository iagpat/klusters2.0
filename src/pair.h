/***************************************************************************
                          pair.h  -  description
                             -------------------
    begin                : Thu Nov 13 2003
    copyright            : (C) 2003 by 
    email                : 
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PAIR_H
#define PAIR_H

// include files for QT
#include <qstring.h>


//General C++ include files
#include <iostream>
#include <stdlib.h>
using namespace std;

/**
  * Class used to represent the clusters corresponding to a given correlogram.
  * This class is used in CorrelationView and CorrelationThread.
  *@author Lynn Hazan
  */

class Pair {
public: 
  inline Pair(){x = 0; y = 0;};
  inline Pair(int x, int y):x(x),y(y){};
  inline int getX(){return x;};
  inline int getY(){return y;};
  inline void setX(int x){this->x = x;};
  inline void setY(int y){this->y = y;};
  
  inline QString toString(){
   QString s = QString("%1-%2").arg(x).arg(y);
   return s;
  };
  inline bool operator==(const Pair& p){
   return (x == p.x && y == p.y);
  }
  inline bool operator <(const Pair& p){
   return ((x < p.x) || (x == p.x) && (y < p.y));
  }
  inline bool operator <=(const Pair& p){
   return ((x < p.x) || (x == p.x) && (y <= p.y));
  }
  inline bool operator >(const Pair& p){
   return ((x > p.x) || (x == p.x) && (y > p.y));
  }
  inline bool operator >=(const Pair& p){
   return ((x > p.x) || (x == p.x) && (y >= p.y));
  }
  
	inline ~Pair(){};

private:
  int x;
  int y;


};

#endif
