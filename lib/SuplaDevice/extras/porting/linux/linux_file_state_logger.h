/*
 Copyright (C) AC SOFTWARE SP. Z O.O.

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

#ifndef EXTRAS_PORTING_LINUX_LINUX_FILE_STATE_LOGGER_H_
#define EXTRAS_PORTING_LINUX_LINUX_FILE_STATE_LOGGER_H_

#include <supla/device/last_state_logger.h>

#include <string>

namespace Supla {
namespace Device {
class FileStateLogger : public LastStateLogger {
 public:
  explicit FileStateLogger(const std::string &);
  void log(const char *) override;
  void addToFile(const char *line);

 protected:
  std::string file;
};
};  // namespace Device
};  // namespace Supla

#endif  // EXTRAS_PORTING_LINUX_LINUX_FILE_STATE_LOGGER_H_
