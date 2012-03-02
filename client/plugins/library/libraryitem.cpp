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

#include "libraryitem.hpp"
#include "librarymodel.hpp"

using namespace Library;

LibraryItem::LibraryItem(const QString title, int id, int itemKind, TreeItem *parent)
	: TreeItem(title, parent), id(id)
{
	isInAlbum = false;
	if (itemKind == LibraryModel::ALBUM)
		kind = ALBUM;
	else if (itemKind == LibraryModel::ALBUM_ARTIST ||
			 itemKind == LibraryModel::ARTIST)
		kind = ARTIST;
	else if (itemKind == LibraryModel::GENRE)
		kind = GENRE;
	else
	{
		if (itemKind == LibraryModel::TRACK)
			isInAlbum = true;
		kind = TRACK;
	}
}

LibraryItem::Kind LibraryItem::getKind() const
{
	return kind;
}

bool LibraryItem::getIsInAlbum() const
{
	return isInAlbum;
}

AudioTrack*	LibraryItem::getTrack()
{
	return AudioTrack::getTrack(id);
}

int			LibraryItem::getId() const
{
	return id;
}
