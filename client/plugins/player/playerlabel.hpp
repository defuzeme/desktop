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

#ifndef PLAYERLABEL_HPP
#define PLAYERLABEL_HPP

#include <QWidget>
#include <QPainter>
#include <QStyledItemDelegate>

namespace Player
{
	class PlayerLabel : public QWidget
	{
		Q_OBJECT
	public:
		explicit PlayerLabel(QWidget *parent = 0);
		void	paintEvent(QPaintEvent *);
		void	setRight();
		void	setCurrent();
		void	setNext();

		static QColor	currentColor();
		static QColor	nextColor();
	signals:

	public slots:

	private:
		QColor			color;
		QString			text;
		bool			right;
		QPainter		painter;
		QPolygon	letterPolygonA, textPolygonA;
		QPolygon	letterPolygonB, textPolygonB;

	};
}

#endif // PLAYERLABEL_HPP
