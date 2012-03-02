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

namespace Gui
{
	class Module;
}

#ifndef MODULE_HPP
#define MODULE_HPP

#include "modulewidget.hpp"
#include "parameterscategory.hpp"
#include "parameterspage.hpp"
#include <QtCore/QString>
#include <QMap>

namespace Params
{
	class Parameterizable;
}

namespace Gui
{
	class GuiCore;

	/** The logical element displayed inside a Gui::GridElement.
	  * The graphical represenetation is the contained ModuleWidget.
	  * Modules are always created with the ModuleFactory, and, once ready, have to
	  * be submitted to the GuiCore using submitForDisplay().
	  **/

	class Module : public QObject
	{
		Q_OBJECT
		friend class ModuleFactory;
		friend class GridElement;
		friend class GuiCore;
		friend class Tab;

	public:
		void 								submitForDisplay(ModuleWidget* widget);
		void 								submitForDisplay();
		const QString&						getUniqId() const;
		void								addParametersPage(ParametersPage *parameterPage);
		bool								hasParameters() const;
		void								setSizePolicy(const QSizePolicy::Policy &sizePolicy, const QSizePolicy::Policy &colSizePolicy = QSizePolicy::Expanding);

	private:
		Module(GuiCore* guiCore);
		void								setTabIndex(int tabIndex);
		void								setPosition(QPoint position);
		void								setTitle(const QString &title);
		void								setUniqId(const QString &uniqId);
		void								setWidget(ModuleWidget *widget);
		int									getTabIndex() const;
		const QPoint&						getPosition() const;
		int									getRow() const;
		int									getColumn() const;
		ModuleWidget*						getWidget() const;
		ModuleWidget						*widget;
		GuiCore								*guiCore;
		QString								title;
		QString								uniqId;
		QPoint								position;
		QMap<QString, ParametersPage*>		parametersPages;
		ParametersCategory*					parametersCategory;
		QSizePolicy::Policy					sizePolicy, colSizePolicy;
		int									tabIndex;

	signals:
		void displayPending();
	};
}

#endif // MODULE_HPP
