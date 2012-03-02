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

#include "audiomixer.hpp"
#include <QDebug>
#include "audioplayer.hpp"
#include "audioeffectvolume.hpp"
#include "audioeffectbalance.hpp"

using namespace Audio;

AudioMixer::AudioMixer(const QString& name, AudioIO *input)
{
	_isStarted = false;
	_name = name;
	_type = AudioMixerType;
	if (input)
	{
		audioInputs.insert(input->getName(), input);
		input->setOutput(this);
	}
	AudioEffect *volumeEffect = new  AudioEffectVolume();
	effects.insert("Volume", volumeEffect);
	AudioEffect *balanceEffect = new  AudioEffectBalance();
	effects.insert("Balance", balanceEffect);
	open(QIODevice::ReadWrite);
}


bool	AudioMixer::isStarted()
{
	return _isStarted;
}

void	AudioMixer::applyEffects(char *data, int len)
{
	foreach(AudioEffect *effect, effects)
	{
		effect->apply((short*)data, len);
	}
}

void	AudioMixer::addInput(AudioIO *input)
{
	audioInputs.insert(input->getName(), input);
	input->setOutput(this);
}

QMap<QString, AudioEffect*>&	AudioMixer::getEffects()
{
	return effects;
}

qint64	AudioMixer::writeData(const char *data, qint64 maxlen)
{
	Q_UNUSED(data);
	Q_UNUSED(maxlen);
	return 0;
}

qint64	AudioMixer::readData(char *data, qint64 maxlen)
{
	Q_UNUSED(data);
	Q_UNUSED(maxlen);
	return 0;
}

qint64	AudioMixer::readData(char *data, qint64 maxlen, AudioIO *from)
{
	AudioIO *input = getAudioInput(from);
	qint64 bytesRead = 0;

	if (input)
	{
		bytesRead = input->readData(data, maxlen, from);
		applyEffects(data, maxlen);
	}
	return bytesRead;
}

qint64 AudioMixer::bytesAvailable() const
{
	qint64 bAv = 0;
	return bAv;
}

AudioIO	*AudioMixer::getAudioInput(const AudioIO* output)
{
	foreach(AudioIO* input, audioInputs)
	{
		if (input->getOutputDevice(output) == output)
			return input;
	}
	return 0;
}

const AudioIO *AudioMixer::getOutputDevice(const AudioIO* output) const
{
	foreach(AudioIO* input, audioInputs)
	{
		if (input->getOutputDevice(output) == output)
			return output;
	}
	return 0;
}
