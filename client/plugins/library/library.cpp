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

#include "library.hpp"
#include "exception.hpp"
#include "playqueue.hpp"
#include "queuetrack.hpp"
#include "sourcesparams.hpp"
#include <QSqlQueryModel>
#include <QFutureWatcher>

using namespace Library;

QStringList	LibraryPlugin::allowedExtension;

LibraryPlugin::LibraryPlugin()
{
	allowedExtension << "mp3" << "wma" << "ogg" << "oga" << "wav" << "flac" << "aac" << "m4a";
}

LibraryPlugin::~LibraryPlugin()
{
}

void LibraryPlugin::init()
{
	setParamsName("library");
	setParamsBackEnd(Params::Parameterizable::DATABASE, cores->db());
	registerToParamsCore(cores->params());

	widget = new LibraryWidget(this);
	model = new LibraryModel(this);
	widget->getTreeViewWidget()->setModel(model);

	Gui::Module *playerModule = Gui::ModuleFactory::create("Library", QPoint(0, 0), widget);
	playerModule->addParametersPage(new SourcesParams("Sources", "Library"));
	playerModule->submitForDisplay();
	loadSources();
	model->update();
}

void LibraryPlugin::aboutToQuit()
{

}

LibraryWidget *LibraryPlugin::getWidget() const
{
	return widget;
}

LibraryModel *LibraryPlugin::getModel() const
{
	return model;
}

void	LibraryPlugin::loadSources()
{
	QSqlQuery	query("SELECT id FROM sources");
	query.exec();
	while (query.next())
	{
		sources << Source::getSource(query.value(0).toInt());
	}
}

void LibraryPlugin::addSource(const QUrl &url, bool recursive)
{
	assertNotLocked("sources");
	setLock("sources");

	QList<Source*> toMerge;
	bool useless = false;

	if (!url.isValid())
		throw_exception(0x01, tr("Invalid source : %1").arg(url.toString()));
	qDebug() << "Adding" << url.toString() << "to the sources";
	foreach(Source *source, sources)
	{
		if (source->isRecursive() && source->toUrl().isParentOf(url))
		{
			useless = true;
			qDebug() << "  - is alreay included in" << source->toUrl().toString();
		}
		else if (recursive && url.isParentOf(source->toUrl()))
		{
			toMerge << source;
			qDebug() << "  - will replace" << source->toUrl().toString();
		}
		else if (source->toUrl() == url)
		{
			if (source->isRecursive() == recursive || (source->isRecursive() && !recursive))
			{
				useless = true;
				qDebug() << "  - is alreay a source :" << source->toUrl().toString();
			}
			else
			{
				toMerge << source;
				qDebug() << "  - will replace" << source->toUrl().toString() << "[Non-recursive]";
			}
		}
	}
	if (!useless)
	{
		Source *newSource = new Source(url.path(), recursive);
		foreach (Source *source, toMerge)
		{
			newSource->merge(source);
		}
		foreach (Source *source, toMerge)
		{
			sources.takeAt(sources.indexOf(source))->remove();
		}
		sources << newSource;
		qDebug() << "  - Done";
	}
	unLock("sources");
}

void LibraryPlugin::updateSource(quintptr &sourceAddr)
{
	Source *source;
	source = (Source*)sourceAddr;
	source->update();
}

void LibraryPlugin::updateSources()
{
	assertNotLocked("sources");
	setLock("sources");

	watcher = new QFutureWatcher<void>;
	connect(watcher, SIGNAL(started()), this, SLOT(updateStarted()));
	connect(watcher, SIGNAL(finished()), this, SLOT(updateFinished()));
	//connect(watcher, SIGNAL(progressValueChanged(int)), this, SLOT(updateProgress(int)));
	//connect(watcher, SIGNAL(progressRangeChanged(int,int)), this, SLOT(updateProgressRange(int,int)));
	sourcesAddr.clear();
	foreach (Source *source, sources)
	{
		connect(source, SIGNAL(progressValueChange(int)), this, SLOT(updateProgress(int)));
		connect(source, SIGNAL(addToMaximumProgress(int)), this, SLOT(updateProgressMax(int)));
		sourcesAddr << (quintptr)((void*)source);
	}
	QFuture<void> sourceUpdate = QtConcurrent::map(sourcesAddr, LibraryPlugin::updateSource);
	watcher->setFuture(sourceUpdate);
}

void LibraryPlugin::updateStarted()
{
	widget->updatingSources();
}

void LibraryPlugin::updateFinished()
{
	delete watcher;
	widget->endUpdatingSources();
	getModel()->update();
	unLock("sources");
}

void LibraryPlugin::updateProgress(int progressValue)
{
	static int value = 0;
	value += progressValue;
	getWidget()->getUpdateProgressBar()->setValue(value);
}

void LibraryPlugin::updateProgressMax(int maximum)
{
	static int max = 0;

	max += maximum;
	getWidget()->getUpdateProgressBar()->setRange(0, max);
}

void LibraryPlugin::addTrackToQueue(int id)
{
	Queue::PlayQueue	*queue = dynamic_cast<Queue::PlayQueue*>(plugins->get("queue"));
	AudioTrack			*track = AudioTrack::getTrack(id);
	if (queue && track)
		queue->add(new Queue::QueueTrack(*track));
}

void LibraryPlugin::defineParams()
{
}

QObject *LibraryPlugin::getTrack(int id)
{
	return AudioTrack::getTrack(id);
}
