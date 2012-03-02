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

#ifndef AUDIO_OUTPUT_DEVICE_THREAD_HPP
#define AUDIO_OUTPUT_DEVICE_THREAD_HPP

#include <QAudioOutput>
#include <QAudioDeviceInfo>
#include <QThread>
#include "audioio.hpp"
#include "thread.hpp"

namespace Audio
{
	class AudioOutputDevice;

	/**
	  ** This clas inherit QThread so it doesn't interfer with the UI.
	  ** It can only be instantiated by AudioOutputDevice
	  ** It use a QAudioOutput in pull mode
	  **/

	class AudioOutputDeviceThread : public Thread
	{
		Q_OBJECT
		friend class AudioOutputDevice;
	private slots:
		void				startAudio();
		void				stopAudio();
		void				pauseAudio();
		QAudio::State		state();
		void				stateChanged(QAudio::State);
		void				setInput(AudioIO *input);
		void				changeFormat(QAudioFormat);

	private:
		AudioOutputDeviceThread(QAudioDeviceInfo device = QAudioDeviceInfo::defaultOutputDevice());
		~AudioOutputDeviceThread();

		void				startThread();
		bool				isStarted();

		void				run();

		QAudioFormat		_format;
		QAudioOutput*		audioOutput;
		bool				_isStarted;
		QAudioDeviceInfo	_audioDevice;
		AudioIO				*input;
	};
}

#endif // AUDIO_OUTPUT_DEVICE_THREAD_HPP
