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

namespace Network {
	class WebFrame;
}

#ifndef WEBFRAME_HPP
#define WEBFRAME_HPP

#include <QByteArray>
#include <QString>
#include <QIODevice>

namespace Network {
	const qint64	MaxRead = 1 << 16;
	const int		MaxFrameSize = 1 << 16;

	/** This class is a part of the C++/Qt implementation of the WebSocket protocol
	  * It represents a network frame (sent or received)
	  * It supports frame concatenation, data masking, multiframe encoding
	  **/

	class WebFrame
	{
		enum OpCode {
			CONTINUATION = 0,
			TEXT = 1,
			BINARY = 2,
			CLOSE = 8,
			PING = 9,
			PONG = 10
		};

	public:
		WebFrame(const QByteArray &binary);
		WebFrame(const QString &text);
		static WebFrame*	fromStream(QIODevice* device);
		void				append(QIODevice* device);
		bool				complete() const;
		const QByteArray&	content() const;
		QByteArray			encoded();
		bool				isMultiFrame() const;
		int					remainingFrames() const;

	private:
		WebFrame();
		void				init(const QByteArray &data, bool masked = true);
		void				generateMask();
		void				applyMask(int until = -1);

		bool				fin;
		bool				rsv1, rsv2, rsv3;
		OpCode				opcode;
		bool				masked;
		qint64				length;
		QByteArray			mask;
		QByteArray			payload;
	};
}

#endif // WEBFRAME_HPP
