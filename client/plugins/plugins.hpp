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

class Plugins;

#ifndef PLUGINS_HPP
#define PLUGINS_HPP

#include <QObject>
#include <QHash>
#include "plugin.hpp"

/** Strucure storing references to all plugins.
  * It also contains some conveniant methods.
  **/

class Plugins : public QObject
{
	Q_OBJECT
public:
	explicit Plugins(Cores* c);
	~Plugins();
	void			init();										///< Call all plugins init() method.
	void			add(const QString& name, Plugin* plugin);	///< Add a plugin to the list of loaded plugins
	Plugin*			get(const QString& name);					///< Return the desired plugin
	template<class T> T*	cast(const QString& name) {
		return dynamic_cast<T*>(get(name));
	}

signals:
	void			message(const QString& msg);
public slots:
	void			aboutToQuit();								///< Call all plugins aboutToQuit() method.

private:
	Cores					&cores;
	QHash<QString, Plugin*>	plugins;
	QList<QString>			order;
};

#endif // PLUGINS_HPP
