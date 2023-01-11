/*
  Copyright (C) krycha88

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#ifndef _SUPLA_UPDATE_COMMON_H
#define _SUPLA_UPDATE_COMMON_H

#include <pgmspace.h>

static const char serverIndex[] PROGMEM =
    R"(<div>{M}: {m}kB</div>
     <div>{U}: {u}kB</div>
     <div>{S}: {s}kB</div>
     <form method='POST' action='' enctype='multipart/form-data'>
         <input type='file' accept='.bin,.bin.gz' name='firmware'>
         </br>
         <input type='submit' value='{b}'>
     </form>
     {g})";
static const char successResponse[] PROGMEM = "<META http-equiv='refresh' content='5'>{m}";
static const char twoStepResponse[] PROGMEM = "<META http-equiv='refresh' content='5'><b>{w}</b> {o} {gg}";
static const char twoStepButton[] PROGMEM = "<a href='update?url={gu}'><br><button>{gg}</button></a>";

#endif
