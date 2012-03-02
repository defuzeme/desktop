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

namespace Gui {
	class ModuleFactory;
}

#ifndef MODULEFACTORY_HPP
#define MODULEFACTORY_HPP

#include <QtCore/QHash>
#include <QWidget>
#include "module.hpp"

namespace Gui
{
	/** A Factory to create a new Module.
	  * It will check for the uniqueness of the new Module's name.
	  **/

	class ModuleFactory
	{
		friend class GuiCore;
		friend class Module;

	public:
		static Module*					create(const QString uniqName,
											   QPoint position,
											   ModuleWidget *widget,
											   int tabIndex = 0);
		static Module*					getModule(const QString& uniqId);

	private:
		ModuleFactory();
		static void						initModuleFactory(GuiCore *_guiCore);
		static QHash<QString, Module*>	modules;
		static GuiCore*					guiCore;
	};
}

#endif // MODULEFACTORY_HPP
