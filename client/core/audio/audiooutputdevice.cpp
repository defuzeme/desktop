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

#include "audioio.hpp"
#include "audiooutputdevice.hpp"
#include <QDebug>
#include "audiocore.hpp"
#include "audiooutputdevicethread.hpp"

using namespace Audio;

AudioOutputDevice::AudioOutputDevice(const QString& name, QAudioDeviceInfo device)
{
	_name = name;
	_type = OutputDeviceType;
//	input->setOutput(this);
	audioThread = new AudioOutputDeviceThread(device);
	connect(this, SIGNAL(startAudio()), audioThread, SLOT(startAudio()));
	connect(this, SIGNAL(pauseAudio()), audioThread, SLOT(pauseAudio()));
	connect(this, SIGNAL(stopAudio()), audioThread, SLOT(stopAudio()));
	qRegisterMetaType<QAudioFormat>("QAudioFormat");
	connect(this, SIGNAL(changeFormat(QAudioFormat)), audioThread, SLOT(changeFormat(QAudioFormat)));
}

AudioOutputDevice::~AudioOutputDevice()
{
	delete audioThread;
}

bool	AudioOutputDevice::isStarted()
{
	return audioThread->isStarted();
}

bool	AudioOutputDevice::setFormat(QAudioFormat format)
{
	//return audioThread->setFormat(format);
	emit changeFormat(format);
	return true;
}

QAudio::State	AudioOutputDevice::state()
{
	return audioThread->state();
}

void	AudioOutputDevice::start()
{
	emit startAudio();
}

void	AudioOutputDevice::stop()
{
	emit stopAudio();
}


void	AudioOutputDevice::pause()
{
	emit pauseAudio();
}

void	AudioOutputDevice::setInput(AudioIO *i)
{
	input = i;
	open(QIODevice::ReadOnly);
	audioThread->setInput(this);
}


qint64	AudioOutputDevice::writeData(const char *data, qint64 maxlen)
{
	Q_UNUSED(data);
	Q_UNUSED(maxlen);
	return 0;
}

qint64	AudioOutputDevice::readData(char *data, qint64 maxlen)
{
	return input->readData(data, maxlen, this);
}

qint64	AudioOutputDevice::readData(char *data, qint64 maxlen, AudioIO *from)
{
	Q_UNUSED(data);
	Q_UNUSED(maxlen);
	Q_UNUSED(from);
	return 0;
}


AudioIO	*AudioOutputDevice::getAudioInput(const AudioIO*)
{
	return 0;
}

const AudioIO	*AudioOutputDevice::getOutputDevice(const AudioIO*) const
{
	return 0;
}
