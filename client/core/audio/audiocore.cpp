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

#include <QDebug>
#include <QVariant>
#include <QMap>
#include "audiocore.hpp"
#include "ffmpeg.hpp"
#include "audiodecoder.hpp"
#include "audiooutputdevice.hpp"
#include "audiomixer.hpp"
#include "exception.hpp"
#include "audioeffectvolume.hpp"

using namespace Audio;

AudioCore::AudioCore(QStringList &)
{
	nbOutput = 0;
	nbMixer = 0;
	nbDecoder = 0;
	stopThread = false;
	masterMixer = 0;
	crossfading = false;
}

AudioCore::~AudioCore()
{
	wait(1000);
}

void    AudioCore::init(Cores *cores)
{
	initCodecs();
	_cores = cores;
	start();
	QList<QAudioDeviceInfo> l = availableDevices(QAudio::AudioOutput);
	foreach(QAudioDeviceInfo i, l)
	{
		qDebug() << "Availble device: " << i.deviceName();
	}
	qDebug() << "Default device: " << QAudioDeviceInfo::defaultOutputDevice().deviceName();
}


void    AudioCore::aboutToQuit()
{
	stopThread = true;
}

QList<QAudioDeviceInfo>	AudioCore::availableDevices(QAudio::Mode mode = QAudio::AudioOutput) const
{
	return QAudioDeviceInfo::availableDevices(mode);
}

void	AudioCore::initCodecs()
{
	avcodec_init();
	avcodec_register_all();
	av_register_all();

	qDebug() << "AVCodec version: " << avformat_version();
	qDebug() << "AVFormat configuration:" << avformat_configuration();
}



AudioDecoder*	AudioCore::newAudioDecoder(QString playerName)
{
	nbDecoder++;
	QString name = playerName + QString("_DECODER_") + QVariant(nbDecoder).toString();
	AudioDecoder *decoder = new AudioDecoder(name, this);
	audioDecoders.insert(name, decoder);
	return decoder;
}

AudioMixer*		AudioCore::newAudioMixer(const QString& name, AudioIO *input)
{
	nbMixer++;
	QString _name = name;
	if (_name.isEmpty())
		_name = QString("MIXER_") + QVariant(nbMixer).toString();
	AudioMixer *mixer = new AudioMixer(_name, input);
	audioMixers.insert(_name, mixer);
	audioMixersList.push_front(mixer);
	return mixer;
}

AudioOutputDevice*	AudioCore::newAudioDevice(QAudioDeviceInfo device)
{
	nbOutput++;
	QString name = QString("OUTPUT_") + QVariant(nbOutput).toString();
	AudioOutputDevice *output = new AudioOutputDevice(name, device);
//	audioOutputDevices.insert(name, output);
	output->setParent(this);
	return output;
}

//AudioOutputDevice*	AudioCore::getAudioOutputDevice(QString name)
//{
//	return audioOutputDevices.value(name);
//}

QString	AudioCore::formatSeconds(int seconds)
{
	int mins = seconds / 60;
	int secs = seconds % 60;

	return QString("%1:%2").arg(mins, 2, 10, QChar('0')).arg(secs, 2, 10, QChar('0'));
}

void	AudioCore::run()
{
	while(!stopThread)
	{
		try {
			foreach(AudioDecoder *decoder, audioDecoders)
			{
				if (!stopThread && decoder->state() == AudioDecoder::Decoding)
				{
					decoder->run();
				}
			}
			if (crossfading)
			{
				qint64	length = crossfadeStart.msecsTo(QDateTime::currentDateTimeUtc());
				float	cross = length / 5000.0;

				if (cross >= 1)
				{
					crossfading = false;
					emit crossfadeFinished();
				}
				else
				{
					static_cast<AudioEffectVolume*>(crossfadeFrom->getEffects()["Volume"])->setVolume(crossfadeFromVolume - cross*crossfadeFromVolume);
					static_cast<AudioEffectVolume*>(crossfadeTo->getEffects()["Volume"])->setVolume(cross * crossfadeToVolume);
				}
			}
			usleep(100);
		} catch (Exception&	e) {
			forward(e);		// Send the exception to main thread and continue executing
			usleep(1000000);
		}
	}
}

void			AudioCore::startCrossfade(AudioMixer* from, AudioMixer* to)
{
	crossfadeFrom = from;
	crossfadeTo = to;
	crossfadeFromVolume = static_cast<AudioEffectVolume*>(crossfadeFrom->getEffects()["Volume"])->volume();
	crossfadeToVolume = static_cast<AudioEffectVolume*>(crossfadeTo->getEffects()["Volume"])->volume();
	crossfadeStart = QDateTime::currentDateTimeUtc();
	crossfading = true;
}

void			AudioCore::stopCrossfade()
{
	crossfading = false;
	static_cast<AudioEffectVolume*>(crossfadeFrom->getEffects()["Volume"])->setVolume(crossfadeFromVolume);
	static_cast<AudioEffectVolume*>(crossfadeTo->getEffects()["Volume"])->setVolume(crossfadeToVolume);
}


AudioMixer*		AudioCore::getMasterMixer()
{
	return masterMixer;
}

QList<AudioMixer*>&	AudioCore::getMixers()
{
	return audioMixersList;
}
