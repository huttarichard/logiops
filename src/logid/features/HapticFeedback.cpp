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
#include <features/HapticFeedback.h>
#include <Device.h>
#include <util/log.h>
#include <ipc_defs.h>

using namespace logid::features;
using namespace logid::backend;

HapticFeedback::HapticFeedback(Device* dev) :
        DeviceFeature(dev),
        _config(dev->activeProfile().haptic_feedback) {
    try {
        _haptic_feedback = std::make_shared<hidpp20::HapticFeedback>(
                &dev->hidpp20());
    } catch (hidpp20::UnsupportedFeature& e) {
        throw UnsupportedFeature();
    }

    _ipc_interface = dev->ipcNode()->make_interface<IPC>(this);
}

void HapticFeedback::configure() {
    std::shared_lock lock(_config_mutex);

    auto& config = _config.get();
    if (config.has_value()) {
        auto& conf = config.value();
        if (conf.strength.has_value()) {
            auto strength = conf.strength.value();
            if (strength > 100)
                strength = 100;

            uint8_t mode_flags = (strength > 0) ? 0x01 : 0x00;
            logPrintf(DEBUG, "Setting haptic feedback: mode=0x%02x strength=%d",
                      mode_flags, strength);
            try {
                _haptic_feedback->setMode(mode_flags,
                                          static_cast<uint8_t>(strength));
            } catch (std::exception& e) {
                logPrintf(WARN, "HapticFeedback: setMode failed: %s", e.what());
            }
        }
    }
}

void HapticFeedback::listen() {
}

void HapticFeedback::setProfile(config::Profile& profile) {
    std::unique_lock lock(_config_mutex);
    _config = profile.haptic_feedback;
}

void HapticFeedback::triggerEffect(uint8_t effect) {
    _haptic_feedback->triggerEffect(effect);
}

HapticFeedback::IPC::IPC(HapticFeedback* parent) :
        ipcgull::interface(
                SERVICE_ROOT_NAME ".HapticFeedback", {
                        {"TriggerEffect", {this, &IPC::triggerEffect, {"effect"}}},
                        {"SetStrength",   {this, &IPC::setStrength,   {"strength"}}},
                        {"CallRaw",       {this, &IPC::callRaw,       {"function", "b0", "b1", "b2", "b3"}}},
                }, {}, {}),
        _parent(*parent) {
}

void HapticFeedback::IPC::triggerEffect(uint8_t effect) {
    _parent.triggerEffect(effect);
}

void HapticFeedback::IPC::callRaw(uint8_t function, uint8_t b0, uint8_t b1,
                                   uint8_t b2, uint8_t b3) {
    _parent._haptic_feedback->callRaw(function, {b0, b1, b2, b3});
}

void HapticFeedback::IPC::setStrength(uint8_t strength) {
    std::unique_lock lock(_parent._config_mutex);
    if (strength > 100)
        strength = 100;
    uint8_t mode_flags = (strength > 0) ? 0x01 : 0x00;
    _parent._haptic_feedback->setMode(mode_flags, strength);

    auto& config = _parent._config.get();
    if (!config.has_value())
        config = config::HapticFeedback{};
    config.value().strength = strength;
}
