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

#include "webframe.hpp"
#include <QDebug>
#include <QtGlobal>

using namespace Network;

WebFrame::WebFrame()
{
}

WebFrame::WebFrame(const QByteArray &binary)
{
	opcode = BINARY;
	init(binary);
}

WebFrame::WebFrame(const QString &text)
{
	opcode = TEXT;
	init(text.toUtf8());
}

void	WebFrame::init(const QByteArray& data, bool masked)
{
	payload = data;
	fin = (remainingFrames() <= 1);
	rsv1 = rsv2 = rsv3 = false;
	this->masked = masked;
	length = payload.length();
	if (this->masked)
		generateMask();
}

bool	WebFrame::isMultiFrame() const
{
	return remainingFrames() > 1;
}

int		WebFrame::remainingFrames() const
{
	if (payload.length() > 0)
		return ((payload.length() - 1) / MaxFrameSize + 1);
	else
		return 0;
}

void	WebFrame::generateMask()
{
	mask.resize(4);
	for(int i = 0; i < 4; i++)
		mask[i] = qrand() % 0xFF;
}

void	WebFrame::applyMask(int until)
{
	if (until == -1)
		until = payload.length();
	for(int i = 0; i < until; i++)
		payload[i] = payload[i] ^ mask[i % 4];
}

bool	WebFrame::complete() const
{
	return (payload.length() == length);
}

const QByteArray&	WebFrame::content() const
{
	return payload;
}

void		WebFrame::append(QIODevice* device)
{
	qint64	toRead = qMin(length - payload.length(), MaxRead);
	payload += device->read(toRead);
}

QByteArray	WebFrame::encoded()
{
	QByteArray	data;
	uint		length = qMin(payload.length(), MaxFrameSize);
	char		c = 0;

	fin = (remainingFrames() <= 1);
	c |= fin << 7;
	c |= rsv1 << 6;
	c |= rsv2 << 5;
	c |= rsv3 << 4;
	c |= opcode & 0xF;
	data += c;
	c = masked << 7;
	if (length > 65535)
	{
		c |= 127;
		data += c;
		for (int i = 0; i < 8; i++)
			data += static_cast<char>((length >> (8 * (7 - i)) & 0xFF));
	}
	else if (length > 125)
	{
		c |= 126;
		data += c;
		data += length >> 8;
		data += length & 0xFF;
	}
	else
	{
		c |= length;
		data += c;
	}
	if (masked)
	{
		data += mask;
		applyMask(length);
	}
	data += payload.left(length);
	payload.remove(0, length);
	opcode = CONTINUATION;
	return data;
}

WebFrame*	WebFrame::fromStream(QIODevice* device)
{
	WebFrame	*frame = new WebFrame();
	QByteArray	head = device->read(2);

	if (head.size() == 2)
	{
		frame->fin = head[0] & (1 << 7);
		if (!frame->fin)
			qDebug() << "WebSocket: Multipart frame received!";
		frame->rsv1 = head[0] & (1 << 6);
		frame->rsv2 = head[0] & (1 << 5);
		frame->rsv3 = head[0] & (1 << 4);
		frame->opcode = static_cast<OpCode>(head[0] & 0x0F);
		frame->masked = head[1] & (1 << 7);
		frame->length = head[1] & 0x7F;
		if (frame->length == 126)
		{
			head = device->read(2);
			frame->length = (uchar)head[0] << 8 | (uchar)head[1];
		}
		else if (frame->length == 127)
		{
			head = device->read(8);
			frame->length = 0;
			for (int i = 0; i < 8; i++)
			{
				frame->length <<= 8;
				frame->length |= (uchar)head[i];
			}
		}
		if (frame->masked)
			frame->mask = device->read(4);
		frame->append(device);
		return frame;
	}
	delete frame;
	return 0;
}
