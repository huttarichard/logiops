# MX Master 4 Configuration Guide

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

The MX Master 4 advertises HID++ feature 0x19b0 (haptic feedback) and accepts setMode/triggerEffect commands, but **no physical haptic motor appears to be present** in the current hardware revision. The feature is exposed via D-Bus in case future firmware updates or hardware revisions enable it.

**D-Bus interface:** `pizza.pixl.LogiOps.HapticFeedback`
- `TriggerEffect(uint8 effect)` — trigger a haptic waveform pattern
- `SetStrength(uint8 strength)` — set haptic intensity (0-100)

**Config (currently non-functional on MX Master 4):**
```
haptic_feedback: {
    strength: 50;    // 0 = off, 1-100 = vibration intensity
};
```

## Full Example Config

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
    dpi: 1000;
});
```

## Troubleshooting

- **Config changes not applying:** Use `sudo systemctl stop logid && sudo systemctl start logid` instead of `restart`.
- **Run `sudo systemctl daemon-reload`** after installing a new build.
- **Debug mode:** `sudo logid -v debug` to see all HID++ communication.
- **Torque not changing:** Ensure `on: true` is set — torque only applies in ratchet mode.
