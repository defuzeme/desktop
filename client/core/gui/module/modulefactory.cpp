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

#include "modulefactory.hpp"
#include "exception.hpp"
#include <QVariant>

using namespace Gui;

QHash<QString, Module*> ModuleFactory::modules = QHash<QString, Module*>();
GuiCore* ModuleFactory::guiCore = 0;

ModuleFactory::ModuleFactory()
{
}

Module* ModuleFactory::create(const QString uniqName, QPoint position, ModuleWidget *widget, int tabIndex)
{
	Q_ASSERT(guiCore);
	QString normalizedName = uniqName.toLower();
	Q_ASSERT(!(normalizedName.isEmpty() || modules.contains(normalizedName)));
	Module *module = new Module(guiCore);
	module->setWidget(widget);
	QString uniqId = normalizedName;
	int i = 0;
	while (modules.contains(uniqId))
	{
		++i;
		uniqId = normalizedName + "_" + QVariant(i).toString();
	}
	module->setUniqId(uniqId);
	module->setPosition(position);
	module->setTabIndex(tabIndex);
	modules[uniqId] = module;
	return module;
}

Module* ModuleFactory::getModule(const QString &uniqId)
{
	return modules[uniqId];
}

void ModuleFactory::initModuleFactory(GuiCore *_guiCore)
{
	guiCore = _guiCore;
}
