# MX Master 4 Configuration Guide

## Button Mapping

The MX Master 4 has 3 remappable thumb buttons:

| CID | Physical Button | Description |
|-----|----------------|-------------|
| 0xc3 | Gesture button (wide thumb pad) | The large button under your thumb |
| 0x56 | Forward button (top) | Upper small button above thumb |
| 0x53 | Back button (bottom) | Lower small button above thumb |

### Pop!_OS Workspace Switching

Pop!_OS uses vertical workspaces. Example mapping:

```
buttons: (
    // Gesture button — show all workspaces
    {
        cid: 0xc3;
        action = {
            type: "Keypress";
            keys: ["KEY_LEFTMETA"];
        };
    },
    // Forward button — next workspace
    {
        cid: 0x56;
        action = {
            type: "Keypress";
            keys: ["KEY_LEFTCTRL", "KEY_LEFTMETA", "KEY_DOWN"];
        };
    },
    // Back button — previous workspace
    {
        cid: 0x53;
        action = {
            type: "Keypress";
            keys: ["KEY_LEFTCTRL", "KEY_LEFTMETA", "KEY_UP"];
        };
    }
);
```

Other remappable buttons: 0x52 (middle click), 0xc4 (smart shift toggle), 0xd7 (top button), 0x1a0 (side button).

## Thumb Wheel (Volume Control)

The horizontal thumb wheel (HID++ 0x2150) can be diverted to custom actions. Volume control works well:

```
thumbwheel: {
    divert: true;
    invert: false;
    left: {
        mode: "OnInterval";
        interval: 8;
        action = { type: "Keypress"; keys: ["KEY_VOLUMEDOWN"]; };
    };
    right: {
        mode: "OnInterval";
        interval: 8;
        action = { type: "Keypress"; keys: ["KEY_VOLUMEUP"]; };
    };
};
```

The `interval` controls sensitivity — lower = more frequent key presses per scroll distance. `15` is a good default.

Set `divert: false` to use the thumb wheel as a native horizontal scroll instead.

## Scroll Wheel Ratchet (SmartShift)

The electromagnetic scroll wheel ratchet is controlled by SmartShift V2 (HID++ 0x2111).

```
smartshift: {
    on: true;          // true = ratchet mode, false = free-spin
    threshold: 15;     // speed threshold to auto-switch to free-spin (1-100, lower = easier to trigger)
    torque: 50;        // ratchet click intensity (1-100)
};
```

**Torque values:**
- `1` — very subtle, smooth ratchet
- `15` — light clicks
- `50` — medium (similar to default Logitech Options+ "medium")
- `100` — firm, precise ratchet clicks

**Note:** Torque changes require the ratchet setting (`on`) to be written in the same call to take effect.

## Scroll Bounce-Back Filter

The MX Master 4's free-spin scroll wheel sends small reverse-direction events when stopping, causing visible page jump-back. The bounce filter drops these.

```
hiresscroll: {
    hires: false;
    invert: false;
    target: false;
    bounce_threshold: 60;   // max delta to consider as bounce (0 = disabled)
    bounce_time_ms: 150;    // time window after last scroll to check for bounce
};
```

Set `bounce_threshold: 0` to disable the filter entirely.

## Haptic Feedback (HID++ 0x19b0)

The MX Master 4 has a thumb rest haptic vibration motor controlled via HID++ feature 0x19b0. The motor is functional and can be triggered via D-Bus.

**D-Bus interface:** `pizza.pixl.LogiOps.HapticFeedback`
- `TriggerEffect(uint8 effect)` — trigger a haptic waveform pattern (0-14)
- `SetStrength(uint8 strength)` — set haptic intensity (0-100)

**Config:**
```
haptic_feedback: {
    strength: 50;    // 0 = off, 1-100 = vibration intensity
};
```

**Available effects:** 0=SharpStateChange, 1=DampStateChange, 2=SharpCollision, 3=DampCollision, 4=SubtleCollision, 5=HappyAlert, 6=AngryAlert, 7=Completed, 8=Square, 9=Wave, 10=Firework, 11=Mad, 12=Knock, 13=Jingle, 14=Ringing

**Trigger from command line:**
```bash
sudo busctl call pizza.pixl.LogiOps /pizza/pixl/logiops/devices/0 \
    pizza.pixl.LogiOps.HapticFeedback TriggerEffect y 12
```

### Notification Haptics

Trigger haptic feedback on desktop notifications:

```bash
# Test it
./haptic-notify.sh 12   # 12 = Knock effect

# Install as user service (auto-starts on login)
mkdir -p ~/.config/systemd/user
cat > ~/.config/systemd/user/haptic-notify.service << 'EOF'
[Unit]
Description=Haptic feedback on desktop notifications
After=graphical-session.target

[Service]
ExecStart=/opt/logiops/haptic-notify.sh 12
Restart=on-failure
RestartSec=5

[Install]
WantedBy=default.target
EOF

systemctl --user daemon-reload
systemctl --user enable --now haptic-notify.service
```

## Full Example Config (Pop!_OS)

```
devices: (
{
    name: "MX Master 4";
    smartshift: {
        on: true;
        threshold: 15;
        torque: 50;
    };
    hiresscroll: {
        hires: false;
        invert: false;
        target: false;
        bounce_threshold: 60;
        bounce_time_ms: 150;
    };
    haptic_feedback: {
        strength: 50;
    };
    thumbwheel: {
        divert: true;
        invert: false;
        left: {
            mode: "OnInterval";
            interval: 8;
            action = { type: "Keypress"; keys: ["KEY_VOLUMEDOWN"]; };
        };
        right: {
            mode: "OnInterval";
            interval: 8;
            action = { type: "Keypress"; keys: ["KEY_VOLUMEUP"]; };
        };
    };
    dpi: 1000;

    buttons: (
        {
            cid: 0xc3;
            action = { type: "Keypress"; keys: ["KEY_LEFTMETA"]; };
        },
        {
            cid: 0x56;
            action = { type: "Keypress"; keys: ["KEY_LEFTCTRL", "KEY_LEFTMETA", "KEY_DOWN"]; };
        },
        {
            cid: 0x53;
            action = { type: "Keypress"; keys: ["KEY_LEFTCTRL", "KEY_LEFTMETA", "KEY_UP"]; };
        }
    );
});
```

## Troubleshooting

- **Config changes not applying:** Use `sudo systemctl stop logid && sudo systemctl start logid` instead of `restart`.
- **Run `sudo systemctl daemon-reload`** after installing a new build.
- **Debug mode:** `sudo logid -v debug` to see all HID++ communication.
- **Torque not changing:** Ensure `on: true` is set — torque only applies in ratchet mode.
