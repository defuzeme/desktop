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

class Core;

#ifndef CORE_HPP
#define CORE_HPP

#include "cores.hpp"

/** Abstract class inherited by all CORES.
  *
  * It contain 2 methods handling CORES loading and releasing.
  * - init() is called at start-up after all CORES are loaded, contrary to the constructor.
  * - aboutToQuit() is called before quit, when all CORES are still loaded, contrary to the destuctor.
  **/

class Core
{
public:
	/** Called once, after all CORES are loaded.
	  * \param cores The already loaded CORES.
	  **/
	virtual void init(Cores *cores) = 0;

	/** Called just before to quit, when CORES are still loaded.
	  **/
	virtual void aboutToQuit() = 0;

protected:
	Cores		*cores;				///< This will store the cores pointer so the core can use any other cores
};

#endif // CORE_HPP
