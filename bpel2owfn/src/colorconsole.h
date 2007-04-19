/*****************************************************************************\
 * Copyright 2007 Niels Lohmann, Christian Gierds                            *
 *                                                                           *
 * This file is part of GNU BPEL2oWFN.                                       *
 *                                                                           *
 * GNU BPEL2oWFN is free software; you can redistribute it and/or modify it  *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * GNU BPEL2oWFN is distributed in the hope that it will be useful, but      *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General  *
 * Public License for more details.                                          *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with GNU BPEL2oWFN; see file COPYING. if not, write to the Free Software  *
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA. *
\*****************************************************************************/

/*!
 * \file    colorconsole.h
 *
 * \brief   colored console output
 *
 *          This header defines stream manipulators that allow for colored
 *          stream output.
 * 
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $
 * 
 * \since   2007-04-17
 *
 * \date    \$Date: 2007/04/19 06:40:48 $
 * 
 * \note    This code was inspired by code published by Jaded Hobo at "The
 *          Code Project" in December 2004, available at
 *          http://www.codeproject.com/cpp/AddColorConsole.asp.
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.7 $
 *
 * \todo    
 *          - finish code for non-Windows operating systems
 */

#ifndef COLORCONSOLE_H
#define COLORCONSOLE_H

#include <iostream>
#include <iomanip>
#include "bpel2owfn.h"

using std::ostream;





#if HAVE_WINDOWS_H == 1

#include <windows.h>

namespace colorconsole
{
  static const WORD bgMask     (BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY);
  static const WORD fgMask     (FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
  static const WORD fgBlack    (0);
  static const WORD fgLoRed    (FOREGROUND_RED);
  static const WORD fgLoGreen  (FOREGROUND_GREEN);
  static const WORD fgLoBlue   (FOREGROUND_BLUE );
  static const WORD fgLoCyan   (fgLoGreen   | fgLoBlue);
  static const WORD fgLoMagenta(fgLoRed     | fgLoBlue);
  static const WORD fgLoYellow (fgLoRed     | fgLoGreen);
  static const WORD fgLoWhite  (fgLoRed     | fgLoGreen | fgLoBlue);
  static const WORD fgGray     (fgBlack     | FOREGROUND_INTENSITY);
  static const WORD fgHiWhite  (fgLoWhite   | FOREGROUND_INTENSITY);
  static const WORD fgHiBlue   (fgLoBlue    | FOREGROUND_INTENSITY);
  static const WORD fgHiGreen  (fgLoGreen   | FOREGROUND_INTENSITY);
  static const WORD fgHiRed    (fgLoRed     | FOREGROUND_INTENSITY);
  static const WORD fgHiCyan   (fgLoCyan    | FOREGROUND_INTENSITY);
  static const WORD fgHiMagenta(fgLoMagenta | FOREGROUND_INTENSITY);
  static const WORD fgHiYellow (fgLoYellow  | FOREGROUND_INTENSITY);
  static const WORD bgBlack    (0);
  static const WORD bgLoRed    (BACKGROUND_RED  );
  static const WORD bgLoGreen  (BACKGROUND_GREEN);
  static const WORD bgLoBlue   (BACKGROUND_BLUE );
  static const WORD bgLoCyan   (bgLoGreen   | bgLoBlue);
  static const WORD bgLoMagenta(bgLoRed     | bgLoBlue);
  static const WORD bgLoYellow (bgLoRed     | bgLoGreen);
  static const WORD bgLoWhite  (bgLoRed     | bgLoGreen | bgLoBlue);
  static const WORD bgGray     (bgBlack     | BACKGROUND_INTENSITY);
  static const WORD bgHiWhite  (bgLoWhite   | BACKGROUND_INTENSITY);
  static const WORD bgHiBlue   (bgLoBlue    | BACKGROUND_INTENSITY);
  static const WORD bgHiGreen  (bgLoGreen   | BACKGROUND_INTENSITY);
  static const WORD bgHiRed    (bgLoRed     | BACKGROUND_INTENSITY);
  static const WORD bgHiCyan   (bgLoCyan    | BACKGROUND_INTENSITY);
  static const WORD bgHiMagenta(bgLoMagenta | BACKGROUND_INTENSITY);
  static const WORD bgHiYellow (bgLoYellow  | BACKGROUND_INTENSITY);

  static class con_dev
  {
    private:
      HANDLE                      hCon;
      CONSOLE_SCREEN_BUFFER_INFO  csbi;

    public:
      WORD                        fg;
      WORD                        bg;

      con_dev() // constructor
      {
	hCon = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hCon, &csbi);
        fg = csbi.wAttributes & fgMask;
        bg = csbi.wAttributes & bgMask;
      }

      void setColor(WORD wRGBI, WORD Mask)
      {
	GetConsoleScreenBufferInfo(hCon, &csbi);
	csbi.wAttributes &= Mask;
	csbi.wAttributes |= wRGBI;
	SetConsoleTextAttribute(hCon, csbi.wAttributes);
      }
  } console;

  ostream& fg_standard(ostream& os)
  {
    os.flush();
    console.setColor(console.fg, bgMask);
    return os;
  }
  
  ostream& fg_red(ostream& os)
  {
    os.flush();
    console.setColor(fgHiRed, bgMask);
    return os;
  }
  
  ostream& fg_green(ostream& os)
  {
    os.flush();
    console.setColor(fgHiGreen, bgMask);
    return os;
  }

  ostream& fg_blue(ostream& os)
  {
    os.flush();
    console.setColor(fgHiBlue, bgMask);
    return os;
  }

  ostream& fg_white(ostream& os)
  {
    os.flush();
    console.setColor(fgHiWhite, bgMask);
    return os;
  }

  ostream& fg_cyan(ostream& os)
  {
    os.flush();
    console.setColor(fgHiCyan, bgMask);
    return os;
  }

  ostream& fg_magenta(ostream& os)
  {
    os.flush();
    console.setColor(fgHiMagenta, bgMask);
    return os;
  }

  ostream& fg_yellow(ostream& os)
  {
    os.flush();
    console.setColor(fgHiYellow, bgMask);
    return os;
  }

  ostream& fg_black(ostream& os)
  {
    os.flush();
    console.setColor(fgBlack, bgMask);
    return os;
  }

  ostream& fg_gray(ostream& os)
  {
    os.flush();
    console.setColor(fgGray, bgMask);
    return os;
  }

  ostream& bg_standard(ostream& os)
  {
    os.flush();
    console.setColor(console.bg, bgMask);
    return os;
  }
  
  ostream& bg_red(ostream& os)
  {
    os.flush();
    console.setColor(bgHiRed, fgMask);
    return os;
  }

  ostream& bg_green(ostream& os)
  {
    os.flush();
    console.setColor(bgHiGreen, fgMask);
    return os;
  }

  ostream& bg_blue(ostream& os)
  {
    os.flush();
    console.setColor(bgHiBlue, fgMask);
    return os;
  }

  ostream& bg_white(ostream& os)
  {
    os.flush();
    console.setColor(bgHiWhite, fgMask);
    return os;
  }

  ostream& bg_cyan(ostream& os)
  {
    os.flush();
    console.setColor(bgHiCyan, fgMask);
    return os;
  }

  ostream& bg_magenta(ostream& os)
  {
    os.flush();
    console.setColor(bgHiMagenta, fgMask);
    return os;
  }

  ostream& bg_yellow(ostream& os)
  {
    os.flush();
    console.setColor(bgHiYellow, fgMask);
    return os;
  }

  ostream& bg_black(ostream& os)
  {
    os.flush();
    console.setColor(bgBlack, fgMask);
    return os;
  }

  ostream& bg_gray(ostream& os)
  {
    os.flush();
    console.setColor(bgGray, fgMask);
    return os;
  }
}





#else /* non-windows systems */

namespace colorconsole
{
  ostream& fg_red(ostream& os)
  { return os; }
  
  ostream& fg_magenta(ostream& os)
  { return os; }

  ostream& fg_blue(ostream& os)
  { return os; }
  
  ostream& fg_black(ostream& os)
  { return os; }

  ostream& fg_green(ostream& os)
  { return os; }

  ostream& fg_standard(ostream& os)
  { return os; }

  ostream& bg_standard(ostream& os)
  { return os; }
}

#endif

#endif
