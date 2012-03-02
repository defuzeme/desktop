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

#ifndef PLAYQUEUEDELEGATE_HPP
#define PLAYQUEUEDELEGATE_HPP

#include <QStyledItemDelegate>
#include "plugins.hpp"
#include "playqueue.hpp"

namespace Queue
{
	/** The PlayQueueDelegate class provides a custom visual rendering for play queue elements
	  **/
	class PlayQueueDelegate : public QStyledItemDelegate
	{
		Q_OBJECT
	public:
		explicit	PlayQueueDelegate(PlayQueue *queue);
		void		paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
		QSize		sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

	signals:

	public slots:
	private:
		QPolygon	positionPolygon, timePolygon;
		PlayQueue	*queue;
	};
}

#endif // PLAYQUEUEDELEGATE_HPP
