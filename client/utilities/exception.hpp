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

#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <QString>
#include <QtCore>

/** This is the default defuze.me exception class.
  * It can be used directly or inherited if you want to change the behaviour.
  *
  * It might be used from the "throw_exception" define to provide automatic filename and line number
  *
  * This class will automatically detect thanks to the file path
  * from which Core the exception is raised from, and display it.
  **/

class Exception : public QtConcurrent::Exception
{
public:
	Exception();
	Exception(unsigned code, const QString msg, const char *file = "unknow file", const int line = 0);
	Exception(const Exception& src);
	virtual		~Exception() throw();
	void		raise() const;
	Exception*	clone() const;

	/** Extract the core from file path.
	  * ie: client/core/db/location.cpp => DbCore
	  **/
	void		extractCore() throw();

	/** Generate a hashcode (4 hex digits) for the error based on file and line number
	  * so we have a "unique" code to reference the error and it makes support easier.
	  **/
	void		generateCode() throw();

	/** Format a nice looking output string from given parameters
	  **/
	void		formatOutput() throw();
	QString		hexCode() const throw();		///< Return the error uniq hex code (4 digits)
	QString		description() const throw();			///< Return a debug string containing all available infos
	QString		msg() const throw();			///< Return a simple message (for user)


protected:
	QString		_msg, file, module;
	unsigned	userCode, code, line;
};


/** This define allows us to throw exception with the filename and the current line easyly
  * Just use it like this:
  *\code
  *		throw_exception("could not open this file");
  *\endcode
  **/

#define throw_exception(code, arg) throw Exception(code, arg, __FILE__, __LINE__)

#endif // EXCEPTION_HPP
