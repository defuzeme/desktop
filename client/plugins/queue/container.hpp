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

namespace Queue {
	class Container;
}

#ifndef CONTAINER_HPP
#define CONTAINER_HPP

#include <QObject>
#include <deque>
#include "queueable.hpp"

namespace Queue
{
	/** This container class can bound together multiple Queueable elements.
	  * It allows to group tracks by playlist for example,
	  * in order to be able to operate on them all in one click.
	  *
	  * \attention It is not used yet
	  **/

	class Container : public QObject
	{
		Q_OBJECT
	public:
		explicit Container(QObject *parent = 0);
		unsigned				size() const;
		bool					empty() const;
		const QueueableDeque&	getChildren() const;

		virtual QString			name() const;					///< This virtual method should return a human-readable string describing the element

	signals:

	public slots:
	private:
		QueueableDeque			children;
	};
}

#endif // CONTAINER_HPP
