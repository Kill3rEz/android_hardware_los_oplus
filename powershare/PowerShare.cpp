/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "vendor.aospa.powershare-service.oplus"

#include "PowerShare.h"

#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/strings.h>

using ::android::base::ReadFileToString;
using ::android::base::Trim;
using ::android::base::WriteStringToFile;

namespace {

constexpr const char* kWirelessTxEnablePath = "/proc/wireless/enable_tx";

}  // anonymous namespace

namespace aidl {
namespace vendor {
namespace aospa {
namespace powershare {

// The node reads back "enable", "charging" or "disable", and the kernel refuses to enable
// wireless TX in some states (wireless RX, FW update), so the state is always re-read. Errors
// are reported through the returned state: the SystemUI tile only handles RemoteException.
bool PowerShare::readEnabled() {
    std::string value;
    if (!ReadFileToString(kWirelessTxEnablePath, &value)) {
        LOG(ERROR) << "Failed to read current PowerShare state";
        return false;
    }

    return Trim(value) != "disable";
}

ndk::ScopedAStatus PowerShare::getMinBattery(int32_t* _aidl_return) {
    *_aidl_return = mMinBattery;
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus PowerShare::isEnabled(bool* _aidl_return) {
    *_aidl_return = readEnabled();
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus PowerShare::setEnabled(bool enable, bool* _aidl_return) {
    if (!WriteStringToFile(enable ? "1" : "0", kWirelessTxEnablePath, true)) {
        LOG(ERROR) << "Failed to write PowerShare state";
    }

    *_aidl_return = readEnabled();
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus PowerShare::setMinBattery(int32_t minBattery, int32_t* _aidl_return) {
    if (minBattery >= 0 && minBattery <= 100) {
        mMinBattery = minBattery;
    }

    *_aidl_return = mMinBattery;
    return ndk::ScopedAStatus::ok();
}

}  // namespace powershare
}  // namespace aospa
}  // namespace vendor
}  // namespace aidl
