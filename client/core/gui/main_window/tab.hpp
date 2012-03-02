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

#ifndef TAB_HPP
#define TAB_HPP

#include "gridwidget.hpp"
#include "gridelement.hpp"
#include <QtCore/QString>
#include <QMultiMap>

namespace Gui
{
	class Module;

	/** The logical representation of a tab in the Gui::TabWidget.
	  * The graphical representation is the contained Gui::GridWidget.
	  **/

	class Tab
	{
	public:
		Tab(const QString &name);
		~Tab();
		QString							getName() const;											///< Return the tab title
		GridWidget*						getGrid() const;
		void							addModule(Module *module);									///< Used to add a module to a tab. This will automatically create a new Gui::GridElement.
		int								columnCount() const;
		GridElement*					getGridElementByModule(Module* module) const;				///< Return the Gui::GridElement containing the given Gui::Module.
		void							cleanGrid(QWidget *targetDropLocation = 0);
		void							showDropLocations(QPoint sourcePosition);
		void							insertGridElement(GridElement *gridElement,
														  QPoint &targetPosition,
														  bool newColumn);

	private:
		void							showColumnDropLocations(int &column, int &orientation, int &offset, QPoint &sourcePosition);
		void							showRowDropLocations(int column, int initialColumn, QPoint &sourcePosition);
		void							showDropLocation(QPoint targetPosition, bool newColumn);
		QString							name;
		GridWidget						*grid;
		QHash<Module*, GridElement*>	elements;
	};
}

#endif // TAB_HPP
