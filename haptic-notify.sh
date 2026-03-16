#!/bin/bash
# Listen for desktop notifications and trigger MX Master 4 haptic feedback.
# Usage: ./haptic-notify.sh [effect_id]
# Default effect: 12 (Knock)

EFFECT="${1:-12}"
DBUS_PATH="/pizza/pixl/logiops/devices/0"
DBUS_IFACE="pizza.pixl.LogiOps.HapticFeedback"

echo "Listening for notifications, triggering haptic effect $EFFECT"
echo "Press Ctrl+C to stop"

dbus-monitor --session "interface='org.freedesktop.Notifications',member='Notify'" 2>/dev/null | \
while read -r line; do
    if echo "$line" | grep -q "member=Notify"; then
        echo "$(date +%H:%M:%S) Notification received, triggering haptic"
        sudo busctl call pizza.pixl.LogiOps "$DBUS_PATH" "$DBUS_IFACE" TriggerEffect y "$EFFECT" 2>/dev/null &
    fi
done
