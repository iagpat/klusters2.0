/***************************************************************************
                          spinbox.cpp  -  description
                             -------------------
    begin                : Tue Jul  30 12:06:21 EDT 2013
    copyright            : (C) 2013 by Lynn Hazan
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

// application specific includes
#include "spinbox.h"
#include <qlineedit.h>

void SpinBox::deselect() {
	lineEdit()->deselect();
}
