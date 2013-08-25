/***************************************************************************
 *   Copyright (C) 2007 by Lynn Hazan   *
 *   lynn.hazan@myrealbox.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef CLUSTERUSERINFORMATION_H
#define CLUSTERUSERINFORMATION_H

/**
	@author Lynn Hazan <lynn.hazan@myrealbox.com>
	This class contains the information manually entered by the user about a cluster.
	
*/
class ClusterUserInformation{
public:
	ClusterUserInformation(int pGroup = 0, int pCluster = 0, const QString& pStructure = QString(),const  QString& pType = QString(),const QString &pID = QString(),const QString &pQuality = QString(), const QString &pNotes = QString())
    :group(pGroup), 
     cluster(pCluster), 
     structure(pStructure), 
     type(pType), 
     ID(pID), 
     quality(pQuality), 
     notes(pNotes){};

    ~ClusterUserInformation(){};

		int getGroup() const {return group;};
		int getCluster() const {return cluster;};
		QString getStructure() const { return structure; };
		QString getType() const { return type; };
		QString getId() const { return ID; };
		QString getQuality() const { return quality; };
		QString getNotes() const { return notes; };
                void setGroup(int pGroup) { group = pGroup; };
		void setCluster(int pCluster) { cluster = pCluster; };
		void setStructure(const QString &pStructure) { structure = pStructure; };
		void setType(const QString &pType) { type = pType; };
		void setId(const QString &pId) { ID = pId; };
		void setQuality(const QString &pQuality) { quality = pQuality; };
		void setNotes(const QString &pNotes) { notes = pNotes; };
		
	private:
		
		int			group;
		int			cluster;
		QString		structure;
		QString		type;
		/**Isolation Distance*/
		QString		ID;
		QString		quality;
		QString		notes;

};

#endif
