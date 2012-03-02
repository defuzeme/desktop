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

#include "audiomixerplugin.hpp"
#include <QDebug>

using namespace Mixer;

AudioMixerPlugin::AudioMixerPlugin()
{
	widget = new AudioMixerWidget();
	uiModule = Gui::ModuleFactory::create("Mixer", QPoint(2, 0), widget);
	//uiModule->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
	uiModule->submitForDisplay();
}

AudioMixerPlugin::~AudioMixerPlugin()
{
	//delete slices
}

void	AudioMixerPlugin::refreshUI()
{
	foreach(AudioSlice *slice, audioSlices)
		slice->getWidget()->refreshUI();
}

void	AudioMixerPlugin::init()
{
/*	mixers = cores->audio()->getMixers();
	foreach(Audio::AudioMixer *mixer, mixers)
	{
		AudioSlice *slice = new AudioSlice(mixer);
		widget->addSlice(slice->getWidget());
		audioSlices.insert(mixer->getName(), slice);
		qDebug() << "add Slice for mixer: " << mixer->getName();
	}*/
}

void	AudioMixerPlugin::refresh()
{
	mixers = cores->audio()->getMixers();
	foreach(Audio::AudioMixer *mixer, mixers)
	{
		if (!audioSlices.contains(mixer->getName()))
		{
			AudioSlice *slice = new AudioSlice(mixer);
			widget->addSlice(slice->getWidget());
			audioSlices.insert(mixer->getName(), slice);
			qDebug() << "add Slice for mixer: " << mixer->getName();
		}
	}
}
