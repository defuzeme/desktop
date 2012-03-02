/*******************************************************************************
**  defuze.me - modern radio automation software suite
**  
**  Copyright © 2012
**    Athena Calmettes - Jocelyn De La Rosa - Francois Gaillard
**    Adrien Jarthon - Alexandre Moore - Luc Peres - Arnaud Sellier
**
**  website: http://defuze.me
**  contact: team@defuze.me
**
**  This program is free software: you can redistribute it and/or modify it
**  under the terms of the GNU Lesser General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU Lesser General Public License for more details.
**
**  You should have received a copy of the GNU Lesser General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
**  This software includes code from Nokia (Qt) under the GNU LGPLv2.1
**  This software uses libraries from the FFmpeg project under the GNU LGPLv2.1
**  This software uses libraries from the TagLib project under the GNU LGPLv2.1
**
*******************************************************************************/

#ifndef LISTSITEM_HPP
#define LISTSITEM_HPP

#include "editabletreeitem.hpp"
#include <QPixmap>
#include <QPushButton>

namespace Lists
{
	class ListsModel;

	class ListsItem : public EditableTreeItem
	{
	public:
		enum Kind {
			CATEGORY, ADD, PLAYLIST, PARAMETER, SEPARATOR
		};
		enum SubKind {
			INVALID_SUBKIND, NORMAL_PLAYLIST, DYNAMIC_PLAYLIST, ADD_NORMAL, ADD_DYNAMIC, GENRE, ALBUM, ARTIST, TRACK, AND, INTRO
		};

		ListsItem(const QString &title, QPixmap icon, ListsModel *model, EditableTreeItem *parent = 0);
		bool insertChildren(int position, int count, int columns);
		ListsItem *child(int number);

		Kind getKind() const;
		SubKind getSubKind() const;
		void setKind(Kind kind);
		void setSubKind(SubKind subKind);
		ListsModel *getModel() const;
		bool isOfKind(Kind _kind);
		bool isOfKind(Kind _kind, SubKind _subKind);
		unsigned int getId() const;
		void setId(unsigned int id);
		void setIcon(QPixmap icon);
		const QPixmap &getIcon() const;

	private:
		QPixmap icon;
		ListsModel *model;
		Kind kind;
		SubKind subKind;
		unsigned int id;
	};

}

#endif // LISTSITEM_HPP
