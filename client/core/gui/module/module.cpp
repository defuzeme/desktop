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

#include "module.hpp"
#include "guicore.hpp"
#include "exception.hpp"
#include "parametersmodule.hpp"

using namespace Gui;

Module::Module(GuiCore *guiCore) : guiCore(guiCore)
{
	widget = 0;
	position.setX(-1);
	position.setY(0);
	tabIndex = 0;
	sizePolicy = QSizePolicy::Preferred;
	colSizePolicy = QSizePolicy::Expanding;
}

void Module::setPosition(QPoint position)
{
	this->position = position;
	if (position.x() < -1)
		position.setX(-1);
	if (position.y() < 0)
		position.setY(0);
}

void Module::setTitle(const QString &title)
{
	this->title = title;
}

void Module::setWidget(ModuleWidget *widget)
{
	this->widget = widget;
	this->title = widget->windowTitle();
}

void Module::setTabIndex(int tabIndex)
{
	this->tabIndex = tabIndex;
}

void Module::setUniqId(const QString &uniqId)
{
	Q_ASSERT(!ModuleFactory::modules.contains(uniqId));
	this->uniqId = uniqId;
}

void Module::setSizePolicy(const QSizePolicy::Policy &sizePolicy, const QSizePolicy::Policy &colSizePolicy)
{
	this->sizePolicy = sizePolicy;
	this->colSizePolicy = colSizePolicy;
}

void Module::submitForDisplay(ModuleWidget* widget)
{
	this->widget = widget;
	submitForDisplay();
}

void Module::submitForDisplay()
{
	Q_ASSERT(widget);
//	if (hasParameters())
//	{
//		foreach (ParametersPage *page, parametersPages)
//		{
//			ParametersCategory *category;
//			if (guiCore->getParametersModule()->getCategories().keys().contains(page->getPath()))
//				category = guiCore->getParametersModule()->getCategories()[page->getPath()];
//			else
//			{
//				category = new ParametersCategory(page->getCategory());
//				guiCore->getParametersModule()->registerCategory(category);
//			}
//			category->addPage(page);
//		}

//	}
	guiCore->addModule(this);
}

void Module::addParametersPage(ParametersPage *parameterPage)
{
	parametersPages[parameterPage->getPath()] = parameterPage;
}

bool Module::hasParameters() const
{
	return !parametersPages.isEmpty();
}

const QString& Module::getUniqId() const
{
	return uniqId;
}

int Module::getTabIndex() const
{
	return tabIndex;
}

const QPoint& Module::getPosition() const
{
	return position;
}

int Module::getRow() const
{
	return position.y();
}

int Module::getColumn() const
{
	return position.x();
}

ModuleWidget *Module::getWidget() const
{
	return widget;
}
