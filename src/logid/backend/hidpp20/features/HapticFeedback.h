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
#ifndef LOGID_BACKEND_HIDPP20_FEATURE_HAPTICFEEDBACK_H
#define LOGID_BACKEND_HIDPP20_FEATURE_HAPTICFEEDBACK_H

#include <backend/hidpp20/feature_defs.h>
#include <backend/hidpp20/Feature.h>

namespace logid::backend::hidpp20 {
    class HapticFeedback : public Feature {
    public:
        static const uint16_t ID = FeatureID::HAPTIC_FEEDBACK;

        uint16_t getID() override { return ID; }

        // Function 0: GetCapabilities
        // Function 1: GetMode - returns mode flags (byte 0) and strength (byte 1)
        // Function 2: SetMode - sets mode flags and strength for thumb haptic motor
        // Function 3: TriggerHapticEffect - plays a haptic waveform pattern
        enum Function {
            GetCapabilities = 0,
            GetMode = 1,
            SetMode = 2,
            GetWaveformList = 3,
            TriggerHapticEffect = 4
        };

        explicit HapticFeedback(Device* dev);

        void setMode(uint8_t mode_flags, uint8_t strength);

        void triggerEffect(uint8_t effect_type);

        void triggerEffectRaw(std::vector<uint8_t> params);

        void callRaw(uint8_t function, std::vector<uint8_t> params);
    };
}

#endif //LOGID_BACKEND_HIDPP20_FEATURE_HAPTICFEEDBACK_H
