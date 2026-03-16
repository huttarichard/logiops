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
#ifndef LOGID_FEATURE_HAPTICFEEDBACK_H
#define LOGID_FEATURE_HAPTICFEEDBACK_H

#include <features/DeviceFeature.h>
#include <backend/hidpp20/features/HapticFeedback.h>
#include <config/schema.h>
#include <ipcgull/interface.h>
#include <shared_mutex>

namespace logid::features {
    class HapticFeedback : public DeviceFeature {
    public:
        void configure() final;

        void listen() final;

        void setProfile(config::Profile& profile) final;

        void triggerEffect(uint8_t effect);

    protected:
        explicit HapticFeedback(Device* dev);

    private:
        mutable std::shared_mutex _config_mutex;
        std::reference_wrapper<std::optional<config::HapticFeedback>> _config;
        std::shared_ptr<backend::hidpp20::HapticFeedback> _haptic_feedback;

        class IPC : public ipcgull::interface {
        public:
            explicit IPC(HapticFeedback* parent);

            void triggerEffect(uint8_t effect);

            void setStrength(uint8_t strength);

            void callRaw(uint8_t function, uint8_t b0, uint8_t b1,
                         uint8_t b2, uint8_t b3);

        private:
            HapticFeedback& _parent;
        };

        std::shared_ptr<IPC> _ipc_interface;
    };
}

#endif //LOGID_FEATURE_HAPTICFEEDBACK_H
