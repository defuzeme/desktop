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

#include "source.hpp"
#include "dbcore.hpp"
#include "exception.hpp"
#include "library.hpp"
#include <QDir>
#include <QDirIterator>

using namespace Library;

QMap<int, Source*>	Source::sourcesMap;

Source::Source(const QString &path, bool isRecursive, const QString &filename) :
	m_path(path), m_filename(filename), m_recursive(isRecursive)
{
	setUpdatedAt(QDateTime::currentDateTime());
	setStatus(UNINITIALIZED);
	m_id = 0;
	save();
}

Source::Source(int id)
{
	QSqlQuery query;
	query.prepare("SELECT type, path, host, port, username, password, filename, updatedAt, status, recursive "
				  "FROM sources WHERE id = :id LIMIT 1");
	query.bindValue(":id", id);
	query.exec();
	if (query.first())
	{
		m_id = id;
		m_path = query.value(1).toString();
		m_filename = query.value(6).toString();
		m_updatedAt = query.value(7).toDateTime();
		m_status = static_cast<Status>(query.value(8).toInt());
		m_recursive = query.value(9).toBool();
	}
	else
		qDebug() << QString("Can't load source with id %1: %2 (%3)").arg(id).arg(query.lastError().text()).arg(query.lastQuery());
}


Source*	Source::getSource(int id)
{
	if (!sourcesMap.contains(id))
		sourcesMap[id] = new Source(id);
	return sourcesMap[id];
}

Source::Status Source::update()
{
	QMap<QString, int>	collection;
	qDebug() << "Update source" << toUrl().toString();
	{
		QDirIterator filesList(toUrl().toLocalFile(), QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);
		QSqlQuery query;
		query.prepare("SELECT id, path FROM audio_tracks WHERE source = :source");
		query.bindValue(":source", id());
		query.exec();
		while(query.next())
		{
			int		track_id = query.value(0).toInt();
			QString	track_path = query.value(1).toString();
			collection[track_path] = track_id;
		}
		int i = 0;
		while (filesList.hasNext())
		{
			filesList.next();
			++i;
		}
		emit addToMaximumProgress(i);
	}
	{
		QDirIterator filesList(toUrl().toLocalFile(), QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);
		int i = 0;
		while (filesList.hasNext())
		{
			i++;
			if (i % 10 == 0)
				emit progressValueChange(10);
			filesList.next();
			if (!filesList.fileInfo().isFile())
				continue;
			if (LibraryPlugin::allowedExtension.contains(filesList.fileInfo().suffix(), Qt::CaseInsensitive))
			{
				QString	relativePath = filesList.fileInfo().filePath();
				relativePath.remove(QRegExp("^"+ QRegExp::escape(toUrl().toLocalFile())));
				if (collection.contains(relativePath))
				{
					collection.remove(relativePath);
				}
				else
				{
					AudioTrack	track(relativePath, this);
					track.save();
				}
			}
		}
	}

	foreach (int i, collection.values())
		AudioTrack::getTrack(i)->remove();
	return (m_status = OK);
}


bool	Source::newRecord() const
{
	return id() == 0;
}

void	Source::save()
{
	QSqlQuery query;

	if (newRecord())
		query.prepare("INSERT INTO sources(path, filename, updatedAt, status, recursive) "
					  "VALUES (:path, :filename, :updatedAt, :status, :recursive)");
	else
		query.prepare("UPDATE sources SET path = :path, filename = :filename, "
					  "updatedAt = :updatedAt, status = :status, recursive = :recursive "
					  "WHERE id = :id");
	query.bindValue(":path", path());
	query.bindValue(":filename", filename());
	query.bindValue(":updatedAt", updatedAt());
	query.bindValue(":status", status());
	query.bindValue(":recursive", isRecursive());

	if (!query.exec())
		qDebug() << QString("Can't save source: %1").arg(query.lastError().text());
	if (newRecord())
	{
		m_id = query.lastInsertId().toInt();
		sourcesMap[id()] = this;
	}
}

void Source::merge(Source *oldSource)
{
	QString newPrefix = oldSource->path();
	QSqlQuery query;

	newPrefix.remove(QRegExp("^" + QRegExp::escape(path())));
	query.prepare("UPDATE audio_tracks SET path = :prefix || path, source = :source_id WHERE source = :old_source_id;");
	query.bindValue(":prefix", newPrefix);
	query.bindValue(":source_id", id());
	query.bindValue(":old_source_id", oldSource->id());
	if (!query.exec())
		throw_exception(0x01, tr("Can't merge source: %1").arg(query.lastError().databaseText()));

	const QMap<int, AudioTrack*>& tracks = AudioTrack::getTracksMap();
	QMap<int, AudioTrack*>::const_iterator it = tracks.constBegin();

	while (it != tracks.constEnd())
	{
		AudioTrack *track = it.value();
		if (track->getSource() == oldSource)
		{
			track->setSource(this);
			track->setPath(QString(newPrefix + track->getPath()));
		}
		++it;
	}
}

void Source::remove()
{
	QSqlQuery query;
	query.prepare("DELETE FROM sources WHERE id = :id");
	query.bindValue(":id", id());
	if (!query.exec())
		throw_exception(0x02, tr("Cannot remove source: %1").arg(query.lastError().text()));
	sourcesMap.remove(id());
	delete this;
}

QUrl Source::toUrl() const
{
	return QUrl::fromUserInput(path() + filename());
}

int	Source::id() const
{
	return m_id;
}

const QString& Source::path() const
{
	return m_path;
}

const QString& Source::filename() const
{
	return m_filename;
}

QDateTime Source::updatedAt() const
{
	return m_updatedAt;
}

void Source::setUpdatedAt(QDateTime updatedAt)
{
	m_updatedAt = updatedAt;
}

Source::Status Source::status() const
{
	return m_status;
}

void Source::setStatus(Status status)
{
	m_status = status;
}

bool Source::isRecursive() const
{
	return m_recursive;
}

void Source::setIsRecursive(bool isRecursive)
{
	m_recursive = isRecursive;
}
