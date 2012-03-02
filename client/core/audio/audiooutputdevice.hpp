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

#ifndef AUDIO_OUTPUT_HPP
#define AUDIO_OUTPUT_HPP

#include <QAudioOutput>
#include <QAudioDeviceInfo>
#include <QThread>
#include "audioio.hpp"
#include "audiocore.hpp"

namespace Audio
{
	class AudioCore;
	class AudioOutputDeviceThread;

	/**
	  ** This class represent an Output device.
	  ** It Contains a thread (AudioOutputDeviceThread) so the playing doesn't interfer with the UI.
	  ** This clas must be instantiated by AudioCore::newAudioDevice(...)
	  **/

	class AudioOutputDevice : public AudioIO
	{
		Q_OBJECT
		friend AudioOutputDevice* AudioCore::newAudioDevice(QAudioDeviceInfo device = QAudioDeviceInfo::defaultOutputDevice());
	public:
		~AudioOutputDevice();

		QAudio::State	state();
		bool			setFormat(QAudioFormat format);
		void			setInput(AudioIO *input);
		bool			isStarted();

		void			start();
		void			stop();
		void			pause();
		AudioIO *getAudioInput(const AudioIO *output);
		const AudioIO	*getOutputDevice(const AudioIO *input) const;
		qint64	readData(char *data, qint64 maxlen, AudioIO *from);

	signals:
		void			startAudio();
		void			stopAudio();
		void			pauseAudio();
		void			changeFormat(QAudioFormat format);
	protected:
		qint64	writeData(const char *data, qint64 maxlen);
		qint64	readData(char *data, qint64 maxlen);

	private:
		AudioIO						*input;
		AudioOutputDeviceThread		*audioThread;
		AudioOutputDevice(const QString& name, QAudioDeviceInfo device = QAudioDeviceInfo::defaultOutputDevice());
	};
}

#endif // AUDIO_OUTPUT_HPP
