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
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PAIR_H
#define PAIR_H

// include files for QT
#include <QString>


#include <stdlib.h>


/**
  * Class used to represent the clusters corresponding to a given correlogram.
  * This class is used in CorrelationView and CorrelationThread.
  *@author Lynn Hazan
  */

class Pair {
public: 
    Pair(){x = 0; y = 0;}
    Pair(int x, int y):x(x),y(y){}
    int getX() const {return x;}
    int getY() const {return y;}
    void setX(int x){this->x = x;}
    void setY(int y){this->y = y;}

    QString toString() const {
        const QString s = QString::fromLatin1("%1-%2").arg(x).arg(y);
        return s;
    }
    bool operator==(const Pair& p){
        return (x == p.x && y == p.y);
    }
    bool operator <(const Pair& p){
        return ((x < p.x) || (x == p.x) && (y < p.y));
    }
    bool operator <=(const Pair& p){
        return ((x < p.x) || (x == p.x) && (y <= p.y));
    }
    bool operator >(const Pair& p){
        return ((x > p.x) || (x == p.x) && (y > p.y));
    }
    bool operator >=(const Pair& p){
        return ((x > p.x) || (x == p.x) && (y >= p.y));
    }

    ~Pair(){}

private:
    int x;
    int y;


};

#endif
