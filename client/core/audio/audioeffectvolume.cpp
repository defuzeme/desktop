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

#include "audioeffectvolume.hpp"
#include "audioeffectvolumewidget.hpp"
#include "math.h"
#include <QDebug>

using namespace Audio;

const double wema_up = 1/20.;
const double wema_down = 1/10000.;

AudioEffectVolume::AudioEffectVolume()
{
	_volume = _oldVolume = 100;
	widget = 0;
	l_level = r_level = 0;
}

int&	AudioEffectVolume::volume()
{
	return _volume;
}

void	AudioEffectVolume::setVolume(int value)
{
	_volume = value;
	if (_oldVolume != _volume)
	{
		_oldVolume = _volume;
		emit setMixerWidgetVolume(value);
	}
}

double	AudioEffectVolume::levelToDB(double level)
{
	return 10 * log10(level*1.414/32768.);
}

double	AudioEffectVolume::levelToRatio(double level)
{
	return (level/32768.);
}

void	AudioEffectVolume::apply(short *data, int len)
{
	double	*level;
	double mult = (_volume / 100.);

	for(int i = 0; i < (len / 2); i++)
	{
		data[i] *= mult;

		level = (i & 1 ? &r_level : &l_level);
		if (abs(data[i]) > *level)
		{
			(*level) = (abs(data[i]) * wema_up + (*level) * (1.-wema_up));
		}
		else
		{
			(*level) = (abs(data[i]) * wema_down + (*level) * (1.-wema_down));
		}
		if (((i&0xFF) == 1) && widget)
			static_cast<AudioEffectVolumeWidget*>(widget)->setLevels(levelToRatio(l_level), levelToRatio(r_level));
	}
}

AudioEffectWidget*	AudioEffectVolume::getWidget()
{
	if (!widget)
	{
		widget = new AudioEffectVolumeWidget(this);
		connect(this, SIGNAL(setMixerWidgetVolume(int)), static_cast<AudioEffectVolumeWidget*>(widget), SLOT(setVolume(int)));
	}
	return widget;
}
