/***************************************************************************
                          sortabletable.cpp  -  description
                             -------------------
    begin                : jeu sep 18 2003
    copyright            : (C) 2003 by Lynn Hazan
    email                : lynn.hazan@myrealbox.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "sortabletable.h"

//C include files
#include <cstring>


const dataType SortableTable::kSMALL_ENOUGH = 15;

SortableTable::SortableTable(const SortableTable& currentSortableTable):
    Array<dataType>(currentSortableTable.nbRows,currentSortableTable.nbColumns){

    memcpy(array,currentSortableTable.array,
           nbRows * nbColumns * sizeof(dataType));
}

void SortableTable::subset(SortableTable& subsetTable, dataType row,dataType startColumn,dataType endColumn){
    if(row == 2 && nbRows == 1) return;

    subsetTable.setSize((endColumn - startColumn) + 1,false);

    memcpy(&(subsetTable.array[(row - 1)*nbColumns]),&array[(row - 1)*nbColumns + (startColumn - 1)],((endColumn - startColumn) + 1) * sizeof(dataType));
}                      

void SortableTable::SelectionSort(dataType rowToSort,dataType secondRow, dataType left, dataType right){
    for(dataType i = left; i < right; i++) {
        dataType min = i;
        for(dataType j=i+1; j <= right; j++)
            if((*this)(rowToSort,j) < (*this)(rowToSort,min)) min = j;
        dataType temp = (*this)(rowToSort,min);
        dataType temp2 = (*this)(secondRow,min);
        (*this)(rowToSort,min) = (*this)(rowToSort,i);
        (*this)(secondRow,min) = (*this)(secondRow,i);
        (*this)(rowToSort,i) = temp;
        (*this)(secondRow,i) = temp2;
    }
}

dataType SortableTable::Partition(dataType rowToSort,dataType secondRow, dataType left, dataType right){
    dataType val = (*this)(rowToSort,left);
    dataType lm = left-1;
    dataType rm = right+1;
    for(;;) {
        do
            rm--;
        while ((*this)(rowToSort,rm) > val);

        do
            lm++;
        while( (*this)(rowToSort,lm) < val);

        if(lm < rm) {
            dataType tempr = (*this)(rowToSort,rm);
            dataType tempr2 = (*this)(secondRow,rm);
            (*this)(rowToSort,rm) = (*this)(rowToSort,lm);
            (*this)(secondRow,rm) = (*this)(secondRow,lm);
            (*this)(rowToSort,lm) = tempr;
            (*this)(secondRow,lm) = tempr2;
        }
        else
            return rm;
    }
}

void SortableTable::Quicksort(dataType rowToSort,dataType secondRow, dataType left, dataType right){
    if(left < (right-kSMALL_ENOUGH)) {
        dataType split_pt = Partition(rowToSort, secondRow, left, right);
        Quicksort(rowToSort,secondRow, left, split_pt);
        Quicksort(rowToSort,secondRow, split_pt+1, right);
    }
    else SelectionSort(rowToSort, secondRow, left, right);
}



void SortableTable::SelectionSort(dataType left, dataType right){
    for(dataType i = left; i < right; i++) {
        dataType min = i;
        for(dataType j=i+1; j <= right; j++)
            if((*this)(1,j) < (*this)(1,min)) min = j;
        dataType temp = (*this)(1,min);
        (*this)(1,min) = (*this)(1,i);
        (*this)(1,i) = temp;
    }
}

dataType SortableTable::Partition(dataType left, dataType right){  
    dataType val = (*this)(1,left);
    dataType lm = left-1;
    dataType rm = right+1;
    for(;;) {
        do
            rm--;
        while ((*this)(1,rm) > val);

        do
            lm++;
        while( (*this)(1,lm) < val);

        if(lm < rm) {
            dataType tempr = (*this)(1,rm);
            (*this)(1,rm) = (*this)(1,lm);
            (*this)(1,lm) = tempr;
        }
        else
            return rm;
    }
}

void SortableTable::Quicksort(dataType left, dataType right){
    if(left < (right-kSMALL_ENOUGH)) {
        dataType split_pt = Partition(left, right);
        Quicksort(left, split_pt);
        Quicksort(split_pt+1, right);
    }
    else SelectionSort(left, right);
}




