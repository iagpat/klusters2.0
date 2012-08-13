/***************************************************************************
 *   Copyright (C) 2007 by Lynn Hazan   *
 *   lynn.hazan@myrealbox.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
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
	inline ClusterUserInformation(int pGroup = 0, int pCluster = 0, QString pStructure = "",QString pType = "",QString pID = "",QString pQuality = "",QString pNotes = ""):group(pGroup), cluster(pCluster), structure(pStructure), type(pType), ID(pID), quality(pQuality), notes(pNotes){};

    inline ~ClusterUserInformation(){};

		inline int getGroup() const {return group;};
		inline int getCluster() const {return cluster;};
		inline QString getStructure() const { return structure; };
		inline QString getType() const { return type; };
		inline QString getId() const { return ID; };
		inline QString getQuality() const { return quality; };
		inline QString getNotes() const { return notes; };

		inline void setGroup(int pGroup) { group = pGroup; };
		inline void setCluster(int pCluster) { cluster = pCluster; };
		inline void setStructure(QString pStructure) { structure = pStructure; };
		inline void setType(QString pType) { type = pType; };
		inline void setId(QString pId) { ID = pId; };
		inline void setQuality(QString pQuality) { quality = pQuality; };
		inline void setNotes(QString pNotes) { notes = pNotes; };
		
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
