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

#ifndef LIBRARY_HPP
#define LIBRARY_HPP

#include "staticplugin.hpp"
#include "parameterizable.hpp"
#include "librarywidget.hpp"
#include "librarymodel.hpp"
#include "playqueue.hpp"
#include "source.hpp"
#include "locker.hpp"
#include <QUrl>
#include <QtConcurrentMap>

namespace Library
{

/**
  * \todo Gestion du filename lors de l'ajout d'une source
  **/

class LibraryPlugin : public Params::Parameterizable, public StaticPlugin, public Locker
{
	Q_OBJECT
public:
	LibraryPlugin();
	~LibraryPlugin();
	void					init();
	void					aboutToQuit();
	LibraryModel			*getModel() const;
	LibraryWidget			*getWidget() const;
	void					addSource(const QUrl &url, bool recursive = true);
	void					updateSources();
	void					addTrackToQueue(int id);
	static void				updateSource(quintptr &sourceAddr);
	static QStringList		allowedExtension;
	Q_INVOKABLE QObject *getTrack(int id);

public slots:
	void					updateStarted();
	void 					updateFinished();
	void					updateProgress(int progressValue);
	void					updateProgressMax(int maximum);

private:
	void					loadSources();
	void					defineParams();
	QList<Source*>			sources;
	QList<quintptr>			sourcesAddr;
	LibraryModel			*model;
	LibraryWidget			*widget;
	QFutureWatcher<void>	*watcher;
};

}

#endif // LIBRARY_HPP
