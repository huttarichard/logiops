# LogiOps

![Build Status](https://github.com/PixlOne/logiops/actions/workflows/build-test.yml/badge.svg)

This is an unofficial driver for Logitech mice and keyboard.

This is currently only compatible with HID++ \>2.0 devices.

## Fork Changes

This fork adds MX Master 4 improvements:

- **Scroll bounce-back filter** — drops small reverse-direction scroll events that occur when a free-spinning wheel stops, preventing page jump-back
- **Haptic feedback support** — HID++ 0x19b0 feature support for the thumb rest vibration motor, with D-Bus API for triggering effects
- **Notification haptics** — script to trigger haptic feedback on desktop notifications

See [MX_MASTER_4.md](MX_MASTER_4.md) for detailed configuration.

## Installation

### Dependencies

This project requires a C++20 compiler, `cmake`, `libevdev`, `libudev`, `glib`, and `libconfig`.

**Arch Linux:** `sudo pacman -S base-devel cmake libevdev libconfig systemd-libs glib2`

**Debian/Ubuntu:** `sudo apt install build-essential cmake pkg-config libevdev-dev libudev-dev libconfig++-dev libglib2.0-dev`

**Fedora:** `sudo dnf install cmake libevdev-devel systemd-devel libconfig-devel gcc-c++ glib2-devel`

**Gentoo Linux:** `sudo emerge dev-libs/libconfig dev-libs/libevdev dev-libs/glib dev-util/cmake virtual/libudev`

**Solus:** `sudo eopkg install cmake libevdev-devel libconfig-devel libgudev-devel glib2-devel`

**openSUSE:** `sudo zypper install cmake libevdev-devel systemd-devel libconfig-devel gcc-c++ libconfig++-devel libudev-devel glib2-devel`

### Build & Install

```bash
git clone https://github.com/huttarichard/logiops.git
cd logiops
git submodule update --init --recursive
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
sudo make install
sudo systemctl daemon-reload
sudo systemctl enable --now logid
```

### Configuration

Copy the example config and edit for your device:

```bash
sudo cp /path/to/logiops/logid.example.cfg /etc/logid.cfg
sudo vim /etc/logid.cfg
```

[Refer to the wiki for general configuration details.](https://github.com/PixlOne/logiops/wiki/Configuration)

For MX Master 4 specific settings, see [MX_MASTER_4.md](MX_MASTER_4.md).

#### Example MX Master 4 config

```
devices: (
{
    name: "MX Master 4";
    smartshift: {
        on: true;
        threshold: 15;
        torque: 50;       // ratchet click intensity (1-100)
    };
    hiresscroll: {
        hires: false;
        invert: false;
        target: false;
        bounce_threshold: 60;  // scroll bounce-back filter
        bounce_time_ms: 150;
    };
    haptic_feedback: {
        strength: 50;     // thumb rest haptic motor (0-100)
    };
    dpi: 1000;
});
```

### Notification Haptics (optional)

Trigger a haptic vibration on your MX Master 4 whenever you get a desktop notification:

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

Available haptic effects: 0=SharpStateChange, 1=DampStateChange, 2=SharpCollision, 3=DampCollision, 4=SubtleCollision, 5=HappyAlert, 6=AngryAlert, 7=Completed, 8=Square, 9=Wave, 10=Firework, 11=Mad, 12=Knock, 13=Jingle, 14=Ringing

### Applying config changes

```bash
sudo systemctl stop logid && sudo systemctl start logid
```

## Configuration

[Refer to the wiki for details.](https://github.com/PixlOne/logiops/wiki/Configuration)

You may also refer to [logid.example.cfg](./logid.example.cfg) for an example.

Default location for the configuration file is /etc/logid.cfg, but another can be specified using the `-c` flag.

## Development

The project may only run as root, but for development purposes, you may find it
convenient to run as non-root on the user bus. You must compile with the CMake
flag `-DUSE_USER_BUS=ON` to use the user bus.

## Donate
This program is (and will always be) provided free of charge. If you would like to support the development of this project by donating, you can donate to my Ko-Fi below.

<a href='https://ko-fi.com/R6R81QQ9M' target='_blank'><img height='36' style='border:0px;height:36px;' src='https://cdn.ko-fi.com/cdn/kofi1.png?v=2' border='0' alt='Buy Me a Coffee at ko-fi.com' /></a>

I'm also looking for contributors to help in my project; feel free to submit a pull request or e-mail me if you would like to contribute.

## Compatible Devices

[For a list of tested devices, check TESTED.md](TESTED.md)

## Credits

Logitech, Logi, and their logos are trademarks or registered trademarks of Logitech Europe S.A. and/or its affiliates in the United States and/or other countries. This software is an independent product that is not endorsed or created by Logitech.

Thanks to the following people for contributing to this repository.

- [Clément Vuchener & contributors for creating the old HID++ library](https://github.com/cvuchener/hidpp)
- [Developers of Solaar for providing information on HID++](https://github.com/pwr-Solaar/Solaar)
- [Nestor Lopez Casado for providing Logitech documentation on the HID++ protocol](http://drive.google.com/folderview?id=0BxbRzx7vEV7eWmgwazJ3NUFfQ28)
- Everyone listed in the contributors page
