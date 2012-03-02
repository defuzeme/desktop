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

#ifndef DROPLOCATION_HPP
#define DROPLOCATION_HPP

#include "ui_droplocation.h"

namespace Gui
{
	class Tab;

	/** The place where it's possible to drop a grid element.
	  * The widget will accept drop with mime-type 'application/x-defuzeme-gridelement'.
	  *
	  * \todo Document the data for the mime-type 'application/x-defuzeme-gridelement'
	  **/

	class DropLocation : public QWidget, private Ui::DropLocation
	{
		Q_OBJECT
	public:
		explicit DropLocation(Tab *tab,
							  QPoint targetPosition,
							  bool newColumn,
							  QWidget *parent = 0);
		void	dragEnterEvent(QDragEnterEvent *event);
		void	dragLeaveEvent(QDragLeaveEvent *event);
		void	dropEvent(QDropEvent *event);

	protected:
		void	changeEvent(QEvent *e);
		Tab		*tab;
		QPoint	targetPosition;
		bool	newColumn;
	};
}

#endif // DROPLOCATION_HPP
