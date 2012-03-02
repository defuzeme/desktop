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

namespace Library {
	class AudioTrack;
}

#ifndef AUDIOTRACK_HPP
#define AUDIOTRACK_HPP

#include "source.hpp"
#include <QVariantMap>
#include <QMap>
#include <QHash>
#include <QObject>
#include <QImage>
#include <QMutex>
#include <QPixmap>
#include "audiotrackloader.hpp"

namespace Library
{
	/** Represent a track file with his tags, album cover and more.
	  * AudioTrack must be fetched using the factory method: getTrack(int id)
	  **/

	class AudioTrack : public QObject
	{
		Q_OBJECT
		Q_PROPERTY(QString exArtist READ exArtist)
		Q_PROPERTY(QString exTitle READ exTitle)
	public:
		AudioTrack(const QString &path, Source *source);
		static AudioTrack*	getTrack(int id);			///< Fetch & cache audiotracks
		static bool			isLoaded(int id);			///< Check cache for audiotrack
		static void			loadAndCall(int id, QObject* obj, const char* slot);  ///< Asynchronously load the given track and call back the given slot after
		const QString&		getTitle() const;
		const QString&		getArtist() const;
		const QString&		getAlbumArtist() const;
		const QString&		getAlbum() const;
		const QString&		getGenre() const;
		const QString&		getPath() const;
		const QString		getAbsolutePath() const;		///< Returns the absolute file path (using source path + relative path)
		int					getUid() const;					///< Unique track identifier
		int					getYear() const;
		int					getTrack() const;				///< Get track number inside album
		int					getDuration() const;			///< Duration is in seconds
		const QVariant		getAttribute(const QString& name) const;	///< Fetch custom attribute by name
		Source*				getSource() const;
		void				setSource(Source *source);
		void				setTitle(const QString& title);
		void				setArtist(const QString& artist);
		void				setAlbumArtist(const QString& albumArtist);
		void				setAlbum(const QString& album);
		void				setGenre(const QString& genre);
		void				setPath(const QString &path);
		void				setUid(int uid);
		void				setYear(int year);
		void				setTrack(int track);
		void				setDuration(int duration);
		void				setAttribute(const QString& name, const QVariant& value);
		bool				newRecord() const;			///< true if the track IS NOT currently saved in the library (going to be added)
		bool				valid() const;				///< true if the track IS saved in the library
		void				save();						///< Save or update the track to library
		void				remove();					///< Remove from library
		static const QMap<int, AudioTrack*>&	getTracksMap();
		const QImage&		getAlbumArt() const;		///< Get the original album cover image (as stored)
		const QPixmap&		getAlbumArtAtSize(int size);	///< Generate, cache and serve a scaled version of the cover image
		const QImage&		getArtistArt() const;		///< Get the artist image
		const QPixmap&		getArtistArtAtSize(int size);	///< Generate, cache and serve a scaled version of the artist image
		QString exArtist() const;
		QString exTitle() const;

	signals:
		void				removed(Library::AudioTrack*);

	private:
		AudioTrack(int id);
		void				extractTags();
		void				extractCover();

		Source				*source;
		int					uid, year, track, duration;
		QString				title, artist, albumArtist, album, genre, path;
		QVariantMap			attributes;
		QImage				image;
		QHash<int, QPixmap>	resizedImages;

		static QMap<int, AudioTrack*>	tracksMap;
		static QMap<int, AudioTrackLoader*>	trackLoaders;
		static QImage					defaultImage, artistImage;
		static QHash<int, QPixmap>		resizedArtist;
		static QMutex					taglibMutex;
		static QMutex					trackMapMutex;
	};
}

#endif // AUDIOTRACK_HPP
