/*
 * Copyright (C) 2021 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aidl/android/hardware/power/BnPower.h>
#include <android-base/file.h>
#include <android-base/logging.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

// Touch device constants
#define TOUCH_DEV_PATH "/dev/xiaomi-touch"
#define TOUCH_ID 0

// IOCTL commands
#define IOCTL_SET_TOUCH_ID 0x40005403
#define IOCTL_TOUCH_OPERATION 0xc4085400

// Touch modes
#define Touch_Doubletap_Mode 14

// Touch command types
#define CMD_SET_CUR_VALUE 0

namespace aidl {
namespace google {
namespace hardware {
namespace power {
namespace impl {
namespace pixel {

using ::aidl::android::hardware::power::Mode;

namespace {

// Touch ioctl data structure
struct CommonDataPacket {
    uint8_t touchId;
    uint8_t command;
    uint16_t mode;
    uint16_t length;
    uint16_t reserved;
    int buffer[256];

    CommonDataPacket()
        : touchId(0),
          command(0),
          mode(0),
          length(0),
          reserved(0) {
        memset(buffer, 0, sizeof(buffer));
    }
};

/**
 * Close the Xiaomi touch device.
 */
static void closeTouchDevice(int fd) {
    if (fd >= 0) {
        close(fd);
    }
}

/**
 * Initialize the Xiaomi touch device.
 *
 * A fresh file descriptor is opened for every operation.
 * This avoids keeping a stale /dev/xiaomi-touch FD across
 * touch-driver resets or reinitialization.
 */
static int initTouchDevice() {
    int fd = open(TOUCH_DEV_PATH, O_RDWR);

    if (fd < 0) {
        LOG(ERROR) << "Failed to open touch device "
                   << TOUCH_DEV_PATH << ": "
                   << strerror(errno);
        return -1;
    }

    int result = ioctl(
        fd,
        IOCTL_SET_TOUCH_ID,
        static_cast<unsigned long>(TOUCH_ID)
    );

    if (result < 0) {
        LOG(ERROR) << "Failed to set touch ID "
                   << TOUCH_ID << ": "
                   << strerror(errno);

        closeTouchDevice(fd);
        return -1;
    }

    LOG(INFO) << "Touch device initialized successfully";

    return fd;
}

/**
 * Set Xiaomi touch mode.
 *
 * For DT2W:
 *   mode  = 14
 *   value = 1 -> enable
 *   value = 0 -> disable
 */
static bool setTouchMode(int mode, int value) {
    int fd = initTouchDevice();

    if (fd < 0) {
        LOG(ERROR) << "Touch device initialization failed";
        return false;
    }

    CommonDataPacket packet;

    packet.touchId = TOUCH_ID;
    packet.command = CMD_SET_CUR_VALUE;
    packet.mode = static_cast<uint16_t>(mode);
    packet.length = 1;
    packet.buffer[0] = value;

    int result = ioctl(
        fd,
        IOCTL_TOUCH_OPERATION,
        &packet
    );

    if (result < 0) {
        LOG(ERROR) << "Failed to set touch mode "
                   << mode
                   << " to value "
                   << value
                   << ": "
                   << strerror(errno);

        closeTouchDevice(fd);
        return false;
    }

    LOG(INFO) << "Touch mode "
              << mode
              << " successfully set to "
              << value;

    closeTouchDevice(fd);

    return true;
}

}  // anonymous namespace

bool isDeviceSpecificModeSupported(
        Mode type,
        bool* _aidl_return) {
    switch (type) {
        case Mode::DOUBLE_TAP_TO_WAKE:
            *_aidl_return = true;
            return true;

        default:
            return false;
    }
}

bool setDeviceSpecificMode(
        Mode type,
        bool enabled) {
    switch (type) {
        case Mode::DOUBLE_TAP_TO_WAKE: {
            LOG(INFO) << "DOUBLE_TAP_TO_WAKE: "
                      << (enabled ? "enabled" : "disabled");

            return setTouchMode(
                Touch_Doubletap_Mode,
                enabled ? 1 : 0
            );
        }

        default:
            return false;
    }
}

}  // namespace pixel
}  // namespace impl
}  // namespace power
}  // namespace hardware
}  // namespace google
}  // namespace aidl
