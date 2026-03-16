#!/usr/bin/env python3
"""
Listen for desktop notifications and trigger MX Master 4 haptic feedback.

Monitors org.freedesktop.Notifications on the session D-Bus and triggers
a haptic effect via logid's system D-Bus interface.

Usage: python3 haptic-notify.py [effect_id]
  effect_id: 0-14 (default: 5 = HappyAlert)

Effects:
  0  Sharp state change    5  Happy alert     10  Firework
  1  Damp state change     6  Angry alert     11  Mad
  2  Sharp collision       7  Completed       12  Knock
  3  Damp collision        8  Square wave     13  Jingle
  4  Subtle collision      9  Wave            14  Ringing
"""
import dbus
import dbus.mainloop.glib
from gi.repository import GLib
import subprocess
import sys

EFFECT = int(sys.argv[1]) if len(sys.argv) > 1 else 5

LOGID_DEST = "pizza.pixl.LogiOps"
LOGID_PATH = "/pizza/pixl/logiops/devices/0"
LOGID_IFACE = "pizza.pixl.LogiOps.HapticFeedback"


def trigger_haptic():
    try:
        subprocess.run([
            "sudo", "busctl", "call",
            LOGID_DEST, LOGID_PATH, LOGID_IFACE,
            "TriggerEffect", "y", str(EFFECT)
        ], timeout=2, capture_output=True)
    except Exception as e:
        print(f"Haptic trigger failed: {e}")


def on_notify(*args, **kwargs):
    app_name = args[0] if args else "unknown"
    summary = args[3] if len(args) > 3 else ""
    print(f"Notification from {app_name}: {summary}")
    trigger_haptic()


def main():
    dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)
    bus = dbus.SessionBus()

    bus.add_signal_receiver(
        on_notify,
        dbus_interface="org.freedesktop.Notifications",
        signal_name="Notify",
    )

    # Also monitor via becoming a proxy for the notification daemon
    # by watching method calls to the notification interface
    proxy = bus.get_object(
        "org.freedesktop.DBus",
        "/org/freedesktop/DBus"
    )
    proxy.BecomeMonitor(
        dbus.Array([
            "type='method_call',interface='org.freedesktop.Notifications',member='Notify'"
        ], signature='s'),
        dbus.UInt32(0),
        dbus_interface="org.freedesktop.DBus.Monitoring"
    )

    bus.add_message_filter(lambda bus, msg: _filter(bus, msg))

    print(f"Listening for notifications, will trigger haptic effect {EFFECT}")
    print("Press Ctrl+C to stop")

    loop = GLib.MainLoop()
    try:
        loop.run()
    except KeyboardInterrupt:
        print("\nStopped")


def _filter(bus, message):
    if message.get_member() == "Notify":
        args = message.get_args_list()
        app_name = str(args[0]) if args else "unknown"
        summary = str(args[3]) if len(args) > 3 else ""
        print(f"Notification from {app_name}: {summary}")
        trigger_haptic()


if __name__ == "__main__":
    main()
