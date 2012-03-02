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

#include "library/audiotrack.hpp"
#include "parser.hpp"
#include "exception.hpp"
#include <QSqlQuery>
#include <QFile>
#include <QDebug>
#include <QSqlError>
#include <QFileInfo>
#include <QMutexLocker>

// TagLib
#include <audioproperties.h>
#include <mpegfile.h>
#include <fileref.h>
#include <id3v2tag.h>
#include <id3v2frame.h>
#include <id3v2header.h>
#include <id3v1tag.h>
#include <apetag.h>
#include <attachedpictureframe.h>
#include <unistd.h>

using namespace Library;

QMap<int, AudioTrack*>			AudioTrack::tracksMap;
QMap<int, AudioTrackLoader*>	AudioTrack::trackLoaders;
QImage							AudioTrack::defaultImage;
QImage							AudioTrack::artistImage;
QMutex							AudioTrack::taglibMutex;
QMutex							AudioTrack::trackMapMutex;
QHash<int, QPixmap>				AudioTrack::resizedArtist;

AudioTrack::AudioTrack(const QString &path, Source *source)
	: source(source), path(path)
{
	uid = 0;
	attributes["extension"] = QFileInfo(getAbsolutePath()).suffix();
	extractTags();
}

AudioTrack::AudioTrack(int id): source(NULL), uid(0)
{
	QSqlQuery query;
	query.prepare("SELECT title, artist, album_artist, album, year, track, genre, duration, attributes, path, source "
				  "FROM audio_tracks WHERE id = :id LIMIT 1");
	query.bindValue(":id", id);
	query.exec();
	if (query.first())
	{
		uid = id;
		title = query.value(0).toString();
		artist = query.value(1).toString();
		albumArtist = query.value(2).toString();
		album = query.value(3).toString();
		year = query.value(4).toInt();
		track = query.value(5).toInt();
		genre = query.value(6).toString();
		duration = query.value(7).toInt();
		attributes = Network::JsonParser().parse(query.value(8).toByteArray()).toMap();
		path = query.value(9).toString();
		source = Source::getSource(query.value(10).toInt());
		if (attributes.contains("image"))
			image.loadFromData(QByteArray::fromHex(attributes["image"].toByteArray()));
		else if (attributes["image-fetched-from-file"] != true)
		{
			extractCover();
			save();
		}
	}
	else
		qDebug() << "No audio track with id " << id;
}

void	AudioTrack::extractTags()
{
	TagLib::FileRef file(getAbsolutePath().toLocal8Bit().data());
	if(!file.isNull() && file.tag()) {

		TagLib::Tag *tag = file.tag();
		title = tag->title().toCString();
		artist = tag->artist().toCString();
		album = tag->album().toCString();
		year = tag->year();
		attributes["comment"] = tag->comment().toCString();
		track = tag->track();
		genre = tag->genre().toCString();
	}
	if (!file.isNull() && file.audioProperties()) {
		TagLib::AudioProperties *properties = file.audioProperties();
		attributes["bitrate"] = properties->bitrate();
		attributes["sampleRate"] = properties->sampleRate();
		attributes["channels"] = properties->channels();
		duration = properties->length();
	}
	if (!attributes["extension"].toString().compare("mp3", Qt::CaseInsensitive))
	{
		TagLib::MPEG::File mp3File(getAbsolutePath().toUtf8().data());
		if (mp3File.ID3v2Tag())
		{
			TagLib::ID3v2::FrameList frames = mp3File.ID3v2Tag()->frameListMap()["TPE2"];
			if(!frames.isEmpty())
				albumArtist = frames.front()->toString().toCString();
		 }
	}
	if (artist.isNull() || artist.isEmpty())
		artist = "Unknown Artist";
	if (album.isNull() || album.isEmpty())
		album = "Unknown Album";
	if (albumArtist.isNull() || albumArtist.isEmpty())
		albumArtist = artist;
	if (title.isNull() || title.isEmpty())
		title = QFileInfo(getAbsolutePath()).fileName();
	extractCover();
}

void	AudioTrack::extractCover()
{
	if (image.isNull())
	{
		QMutexLocker	locker(&taglibMutex);
		TagLib::FileRef fileref(getAbsolutePath().toLocal8Bit().data());
		TagLib::ID3v2::Tag tag(fileref.file(),0);
		TagLib::ID3v2::FrameList list = tag.frameListMap()["APIC"];
		if (!list.isEmpty())
		{
			TagLib::ID3v2::AttachedPictureFrame *picture = static_cast<TagLib::ID3v2::AttachedPictureFrame *>(list.front());
			image.loadFromData((const uchar *) picture->picture().data(), picture->picture().size());
			QByteArray	data;
			QBuffer		buf(&data);
			image.save(&buf, "jpg");
			attributes["image"] = data.toHex();
		}
		attributes["image-fetched-from-file"] = true;
	}
}

const QString	AudioTrack::getAbsolutePath() const
{
	if (source)
		return source->toUrl().toLocalFile() + getPath();
	else
		return getPath();
}

AudioTrack*	AudioTrack::getTrack(int id)
{
	if (!tracksMap.contains(id))
	{
		AudioTrack	*track = new AudioTrack(id);
		if (!track->valid())
		{
			delete track;
			track = 0;
		}
		{
			QMutexLocker	lock(&trackMapMutex);
			tracksMap.insert(id, track);
		}
	}
	return tracksMap[id];
}

bool	AudioTrack::isLoaded(int id)
{
	QMutexLocker	lock(&trackMapMutex);
	return tracksMap.contains(id);
}

void	AudioTrack::loadAndCall(int id, QObject* obj, const char* slot)
{
	if (!trackLoaders.contains(id))
	{
		// Create loader
		AudioTrackLoader	*loader = new AudioTrackLoader(id);
		connect(loader, SIGNAL(loaded(int)), obj, slot);
		trackLoaders[id] = loader;
	}
	else
	{
		//connect(trackLoaders[id], SIGNAL(loaded(int)), obj, slot);
	}
}

const QString&	AudioTrack::getTitle() const
{
	return (title);
}

const QString&	AudioTrack::getArtist() const
{
	return (artist);
}

const QString&	AudioTrack::getAlbumArtist() const
{
	return (albumArtist);
}

const QString&	AudioTrack::getAlbum() const
{
	return (album);
}

const QString&	AudioTrack::getGenre() const
{
	return (genre);
}

const QString& AudioTrack::getPath() const
{
	return (path);
}

int				AudioTrack::getUid() const
{
	return (uid);
}

int				AudioTrack::getYear() const
{
	return (year);
}

int				AudioTrack::getTrack() const
{
	return (track);
}

int				AudioTrack::getDuration() const
{
	return (duration);
}

const QVariant	AudioTrack::getAttribute(const QString& name) const
{
	if (attributes.contains(name))
		return attributes[name];
	else
		return QVariant();
}

Source* AudioTrack::getSource() const
{
	return source;
}

void AudioTrack::setSource(Source *source)
{
	this->source = source;
}

void			AudioTrack::setTitle(const QString& title)
{
	this->title = title;
}

void			AudioTrack::setArtist(const QString& artist)
{
	this->artist = artist;
}

void			AudioTrack::setAlbumArtist(const QString& albumArtist)
{
	this->albumArtist = albumArtist;
}

void			AudioTrack::setAlbum(const QString& album)
{
	this->album = album;
}

void			AudioTrack::setGenre(const QString& genre)
{
	this->genre = genre;
}

void AudioTrack::setPath(const QString &path)
{
	this->path = path;
}

void			AudioTrack::setUid(int uid)
{
	this->uid = uid;
}

void			AudioTrack::setYear(int year)
{
	this->year = year;
}

void			AudioTrack::setTrack(int track)
{
	this->track = track;
}

void			AudioTrack::setDuration(int duration)
{
	this->duration = duration;
}

void			AudioTrack::setAttribute(const QString& name, const QVariant& value)
{
	attributes[name] = value;
}

bool			AudioTrack::newRecord() const
{
	return uid == 0;
}

bool			AudioTrack::valid() const
{
	return uid > 0;
}

void			AudioTrack::save()
{
	QSqlQuery query;

	if (newRecord())
		query.prepare("INSERT INTO audio_tracks(title, artist, album_artist, album, year, track, "
					  "genre, duration, attributes, path, source) VALUES (:title, :artist, :album_artist, :album, "
					  ":year, :track, :genre, :duration, :attributes, :path, :source)");
	else
		query.prepare("UPDATE audio_tracks SET title = :title, artist = :artist, album_artist = :album_artist, "
					  "album = :album, year = :year, track = :track, genre = :genre, "
					  "duration = :duration, attributes = :attributes, path = :path, source = :source "
					  "WHERE id = :id");
	query.bindValue(":id", uid);
	query.bindValue(":title", title);
	query.bindValue(":artist", artist);
	query.bindValue(":album_artist", albumArtist);
	query.bindValue(":album", album);
	query.bindValue(":year", year);
	query.bindValue(":track", track);
	query.bindValue(":genre", genre);
	query.bindValue(":path", path);
	query.bindValue(":source", source->id());
	query.bindValue(":duration", duration);
	query.bindValue(":attributes", Network::JsonParser().serialize(attributes));
	if (!query.exec())
		throw_exception(0x01, tr("Can't save audio track: %1").arg(query.lastError().databaseText()));
	// Fetch new id if inserted
	if (newRecord())
	{
		uid = query.lastInsertId().toInt();
	}
}

void			AudioTrack::remove()
{
	QSqlQuery query;

	if (!newRecord())
	{
		query.prepare("DELETE FROM audio_tracks WHERE id = :id");
		query.bindValue(":id", uid);
		emit removed(this);
		if (query.exec())
		{
			tracksMap.remove(uid);
			uid = 0;
			deleteLater();
		}
		else
			qDebug() << QString("Can't remote tracks: %1 (%2)").arg(query.lastError().text()).arg(query.lastQuery());

	}
}

const QMap<int, AudioTrack*>& AudioTrack::getTracksMap()
{
	return tracksMap;
}

const QImage&	AudioTrack::getAlbumArt() const
{
	if (image.isNull())
	{
		if (defaultImage.isNull())
			defaultImage.load(":cover/default");
		return defaultImage;
	}
	else
		return image;
}

const QPixmap&	AudioTrack::getAlbumArtAtSize(int size)
{
	if (!resizedImages.contains(size))
		resizedImages[size] = QPixmap::fromImage(getAlbumArt().scaled(QSize(size, size), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
	return resizedImages[size];
}

const QImage&	AudioTrack::getArtistArt() const
{
	if (artistImage.isNull())
		artistImage.load(":cover/artist");
	return artistImage;
}

const QPixmap&	AudioTrack::getArtistArtAtSize(int size)
{
	if (!resizedArtist.contains(size))
		resizedArtist[size] = QPixmap::fromImage(getArtistArt().scaled(QSize(size, size), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
	return resizedArtist[size];
}

QString AudioTrack::exArtist() const
{
	return artist;
}

QString AudioTrack::exTitle() const
{
	return title;
}
