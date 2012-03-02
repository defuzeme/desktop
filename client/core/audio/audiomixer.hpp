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

#ifndef AUDIOMIXER_HPP
#define AUDIOMIXER_HPP

#include "audioio.hpp"
#include <QMap>
#include <QStringList>
#include <QList>
#include "audioeffect.hpp"
#include "audiocore.hpp"

namespace Audio
{

	/**
	  ** This class must be instantiated by AudioCore::newAudioMixer(...)
	  **/

	class AudioMixer : public AudioIO
	{
		friend AudioMixer* AudioCore::newAudioMixer(const QString &name, AudioIO *input);

		Q_OBJECT
	public:
		void							addInput(AudioIO *input);
		bool							isStarted();

		qint64							bytesAvailable() const;

		QMap<QString, AudioEffect*>		&getEffects();

		qint64							readData(char *data, qint64 maxlen, AudioIO *from);
		AudioIO							*getAudioInput(const AudioIO *output);
		const AudioIO					*getOutputDevice(const AudioIO *input) const;

	signals:

	public slots:

	protected:
		qint64							writeData(const char *data, qint64 maxlen);
		qint64							readData(char *data, qint64 maxlen);
	private:
		AudioMixer(const QString& name, AudioIO *input);
		QMap<QString, AudioEffect*>		effects;
		QMap<QString, AudioIO*>			audioInputs;
		bool							_isStarted;
		void							applyEffects(char* data, int len);
	};
}

#endif // AUDIOMIXER_HPP
