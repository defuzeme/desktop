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

#ifndef AUDIOCORE_HPP
#define AUDIOCORE_HPP

#include "core.hpp"
#include "thread.hpp"
#include <QList>
#include <QMap>
#include <QVector>
#include <QAudioDeviceInfo>
#include <QThread>
#include <QDateTime>

namespace Audio
{
	class AudioIO;
	class AudioDecoder;
	class AudioOutputDevice;
	class AudioMixer;
	class AudioOutputDevices;

	/** The main class handling Audio functions
	  ** This class inherit QThread to play send the audio to the AudioOutput inside a thread.
	  **/

	class AudioCore : public Thread, public Core
	{
		Q_OBJECT
	public:
		AudioCore(QStringList &arguments);
		~AudioCore();
		void							init(Cores *cores);
		void							aboutToQuit();
		QList<QAudioDeviceInfo>			availableDevices(QAudio::Mode mode) const;

		AudioDecoder					*newAudioDecoder(QString playerName);

		AudioMixer						*newAudioMixer(const QString& name = "", AudioIO *input = 0);

		AudioOutputDevice						*newAudioDevice(QAudioDeviceInfo device = QAudioDeviceInfo::defaultOutputDevice());
//		AudioOutputDevice						*getAudioOutputDevice(QString name);


		static QString					formatSeconds(int seconds);
		AudioMixer*						getMasterMixer();
		QList<AudioMixer*>&				getMixers();
		void							startCrossfade(AudioMixer* from, AudioMixer* to);
		void							stopCrossfade();
	signals:
		void							crossfadeFinished();

	private:
		void							initCodecs();
		void							run();

		AudioMixer*						masterMixer;
		bool							stopThread;
		int								nbDecoder;
		int								nbOutput;
		int								nbMixer;
		bool							crossfading;
		AudioMixer						*crossfadeFrom, *crossfadeTo;
		int								crossfadeFromVolume, crossfadeToVolume;
		QDateTime						crossfadeStart;
		Cores							*_cores;

		QList<AudioMixer*>							audioMixersList;
		QMap<QString, AudioOutputDevices*>			audioOutputDevices;
		QMap<QString, AudioMixer*>					audioMixers;

		QMap<QString, AudioDecoder*>				audioDecoders;
	};
}

#endif // AUDIOCORE_HPP
