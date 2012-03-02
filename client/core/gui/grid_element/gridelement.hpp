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

#ifndef GRIDELEMENT_HPP
#define GRIDELEMENT_HPP

#include "ui_gridelement.h"
#include "module.hpp"

namespace Gui
{
	class GridWidget;

	/** The visible elements inside tabs.
	  * - Contained in a Gui::GridWidget at defined column and row
	  * - Contains a Gui::Module
	  * - Can be drap&drop inside their Gui::GridWidget
	  * - Is always created by the Gui::GuiCore
	  **/

	class GridElement : public QWidget, private Ui::GridElement
	{
		Q_OBJECT
	public:
		explicit GridElement(const QString &title, GridWidget *parent = 0);
		void			setModule(Module *module);
		Module*			getModule() const;
		GridWidget*		getGridWidget() const;
		int				getColumn() const;
		void			setColumn(int column);
		int				getRow() const;
		void			setRow(int row);
		const QPoint&	getPosition() const;
		void			setPosition(QPoint position);

	protected:
		void			changeEvent(QEvent *e);
		Module			*module;
		GridWidget		*gridWidget;
		QPoint			position;

	public slots:
		void			showParams();
	};
}

#endif // GRIDELEMENT_HPP
