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
#include "audiooutputdevicethread.hpp"
#include <QDebug>

using namespace Audio;

AudioOutputDeviceThread::AudioOutputDeviceThread(QAudioDeviceInfo device) : input(0)
{
	_audioDevice = device;
	audioOutput = 0;
	_isStarted = false;
}

AudioOutputDeviceThread::~AudioOutputDeviceThread()
{
	quit();
	wait();
	if (audioOutput)
		audioOutput->stop();
}

bool	AudioOutputDeviceThread::isStarted()
{
	return _isStarted;
}

void	AudioOutputDeviceThread::changeFormat(QAudioFormat format)
{
	qDebug() << "Set format";
	_format = format;
	audioOutput = new QAudioOutput(_audioDevice, _format);
	connect(audioOutput, SIGNAL(stateChanged(QAudio::State)), SLOT(stateChanged(QAudio::State)));
	startThread();
}

void	AudioOutputDeviceThread::stateChanged(QAudio::State state)
{
//	qDebug() << "state changed: " << state;
	if (state == QAudio::IdleState)
	{
		// If audio output goes underrun (no available data) retry later!
		QTimer::singleShot(100, this, SLOT(startAudio()));
	}
}

QAudio::State	AudioOutputDeviceThread::state()
{
	if (audioOutput)
		return audioOutput->state();
	return QAudio::StoppedState;
}

void	AudioOutputDeviceThread::startThread()
{
	if (input)
	{
		qDebug() << "Start thread";
		start();
		QObject::moveToThread(this);
	}
}

void	AudioOutputDeviceThread::setInput(AudioIO *i)
{
	qDebug() << "setInput : " << i->getName();
	input = i;
}

void	AudioOutputDeviceThread::startAudio()
{
//	qDebug() << "StartAudio!";
	if (isStarted() && state() == QAudio::SuspendedState)
	{
		audioOutput->resume();
		qDebug() << "Device resume";
	}
	else if (isStarted() && state() != QAudio::ActiveState)
	{
		audioOutput->start(input);
//		qDebug() << "Device start : " << input->isOpen();
	}
	else
	{
		qDebug() << "Device not started !!!";
	}
}

void	AudioOutputDeviceThread::stopAudio()
{
	if (isStarted())
	{
		audioOutput->stop();
		qDebug() << "Device stop";
	}
}


void	AudioOutputDeviceThread::pauseAudio()
{
	if (isStarted())
	{
		audioOutput->suspend();
		qDebug() << "Device suspend";
	}
}

void  AudioOutputDeviceThread::run()
{
	audioOutput = new QAudioOutput(_audioDevice, _format);
	connect(audioOutput, SIGNAL(stateChanged(QAudio::State)), SLOT(stateChanged(QAudio::State)));
	_isStarted = true;
	exec();
}
