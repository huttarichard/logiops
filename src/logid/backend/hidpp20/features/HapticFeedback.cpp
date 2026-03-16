/*
 * Copyright 2019-2023 PixlOne
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <backend/hidpp20/features/HapticFeedback.h>
#include <util/log.h>
#include <sstream>
#include <iomanip>

using namespace logid::backend::hidpp20;

static std::string hexdump(const std::vector<uint8_t>& data) {
    std::ostringstream ss;
    for (size_t i = 0; i < data.size(); i++) {
        if (i > 0) ss << " ";
        ss << std::hex << std::setfill('0') << std::setw(2) << (int)data[i];
    }
    return ss.str();
}

HapticFeedback::HapticFeedback(Device* dev) : Feature(dev, ID) {
}

void HapticFeedback::setMode(uint8_t mode_flags, uint8_t strength) {
    std::vector<uint8_t> params(2);
    params[0] = mode_flags;
    params[1] = strength;
    callFunction(SetMode, params);
}

void HapticFeedback::triggerEffect(uint8_t effect_type) {
    std::vector<uint8_t> params(1);
    params[0] = effect_type;
    callFunction(TriggerHapticEffect, params);
}

void HapticFeedback::triggerEffectRaw(std::vector<uint8_t> params) {
    logPrintf(DEBUG, "HapticFeedback::triggerEffectRaw sending: %s",
              hexdump(params).c_str());
    auto response = callFunction(TriggerHapticEffect, params);
    logPrintf(DEBUG, "HapticFeedback::triggerEffectRaw response: %s",
              hexdump(response).c_str());
}

void HapticFeedback::callRaw(uint8_t function, std::vector<uint8_t> params) {
    logPrintf(DEBUG, "HapticFeedback::callRaw fn=%d sending: %s",
              function, hexdump(params).c_str());
    auto response = callFunction(function, params);
    logPrintf(DEBUG, "HapticFeedback::callRaw fn=%d response: %s",
              function, hexdump(response).c_str());
}
