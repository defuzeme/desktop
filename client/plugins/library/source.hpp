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

#ifndef SOURCE_HPP
#define SOURCE_HPP

#include <QString>
#include <QDateTime>
#include <QUrl>

namespace Library
{
	class Source : public QObject
	{
		Q_OBJECT
		Q_PROPERTY(unsigned int id READ id)
		Q_PROPERTY(QString path READ path)
		Q_PROPERTY(QString filename READ filename)
		Q_PROPERTY(QDateTime updatedAt READ updatedAt WRITE setUpdatedAt)
		Q_PROPERTY(Status status READ status WRITE setStatus)
		Q_PROPERTY(bool isRecursive READ isRecursive WRITE setIsRecursive)
		Q_ENUMS(Status)

	public:

		enum Status
		{
			OK = 0,
			OUTDATED = 1,
			ERROR = 2,
			UNINITIALIZED = 3
		};

		Source(const QString &path, bool isRecursive = true, const QString &filename = QString());
		Source(int id);

		static Source*	getSource(int id);

		int				id() const;
		const			QString& path() const;
		const			QString& filename() const;
		QDateTime		updatedAt() const;
		Status			status() const;
		bool			isRecursive() const;
		void			setUpdatedAt(QDateTime updatedAt);
		void			setStatus(Status status);
		void			setIsRecursive(bool isRecursive);

		Status			update();
		bool			newRecord() const;
		void			save();
		void			merge(Source *oldSource);
		QUrl			toUrl() const;
		void			remove();

	signals:
		void			progressValueChange(int progressValue);
		void			addToMaximumProgress(int maxValue);

	private:
		int				m_id;
		QString			m_path;
		QString			m_filename;
		QDateTime		m_updatedAt;
		Status			m_status;
		bool			m_recursive;

		static QMap<int, Source*>	sourcesMap;
	};
}
#endif // SOURCE_HPP
