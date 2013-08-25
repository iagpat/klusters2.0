/***************************************************************************
                          sortabletable.h  -  description
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

#ifndef SORTABLETABLE_H
#define SORTABLETABLE_H

#include "array.h"
#include "types.h"

/**
* This class is a one or two line Array of dataType with a sort feature on one of the lines. The sort 
* is an implementation of the quick sort algorithm.
* @author Lynn Hazan
*/
class SortableTable : public Array<dataType>{

public: 
    SortableTable(){}
    ~SortableTable(){}

    /**Sets the dimensions of the table, by default it will be a two row table.
  * @param nbOfColumns numbero f column for the table.
  * @param twoRows true if the table has to be a two row table, false
  * if it has to be a one row table, the default is true.
  */
    void setSize(dataType nbOfColumns,bool twoRows = true){
        if(twoRows)Array<dataType>::setSize(2,nbOfColumns);
        else Array<dataType>::setSize(1,nbOfColumns);
    }

    /**Copy constructor.*/
    SortableTable(const SortableTable& currentSortableTable);

    /**
  * Sorts the two row table using @p rowToSort as the row to sort (row numbering start at 1).
  */
    void sort(dataType rowToSort){
        if(rowToSort == 1)
            Quicksort(1, 2, 1, nbColumns);
        else if(rowToSort == 2)
            Quicksort(2, 1, 1, nbColumns);
    }

    /**
  * Sorts the one row table.
  */
    void sort(){Quicksort(1,nbColumns);}


    /**Returns a subset of the table, the data from one row (@p row) contained
  * between @p startColumn and @p endColumn. The data are store in a one row table.
  * If @p row equals 2 but the table is a one row table, nothing is done.
  * @param subsetTable an empty table where the data will be store. The size of this table will
  * become (1,@p startColumn - @p endColumn).
  * @param row the row to get the data from.
  * @param startColumn first column from where to get the data.
  * @param endColumn last column from where to get the data.
  */
    void subset(SortableTable& subsetTable,dataType row,dataType startColumn,dataType endColumn);
    
private:

    void setSize(dataType nbOfRows, dataType nbOfColumns){}

    /*QuickSort algorithm.
  * To speed the sort, two sort algorithms are combined.
  * Quicksort() is used as the main algorithm, but when the subarrays get small enough,
  * we switch to an alternative, a selection sort.
  * The small enough is a value between 5 and 20.
  */

    void SelectionSort(dataType rowToSort,dataType secondRow, dataType left, dataType right);
    dataType Partition(dataType rowToSort,dataType secondRow, dataType left, dataType right);
    void Quicksort(dataType rowToSort,dataType secondRow, dataType left, dataType right);

    void SelectionSort(dataType left, dataType right);
    dataType Partition(dataType left, dataType right);
    void Quicksort(dataType left, dataType right);

    static const dataType kSMALL_ENOUGH;

};

#endif
