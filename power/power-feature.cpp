/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

// Device extension for the AOSPA power feature HAL (vendor.aospa.power-service): the OPlus touch
// driver does not take a plain 0/1 tap-to-wake node but a gesture bitmask, so double tap is
// handled here through IOplusTouch, like power-mode.cpp does for Mode::DOUBLE_TAP_TO_WAKE.

#include <aidl/vendor/aospa/power/Feature.h>
#include <aidl/vendor/oplus/hardware/touch/IOplusTouch.h>

#include <android-base/logging.h>
#include <android/binder_manager.h>

#include <OplusTouchConstants.h>

#include <cerrno>
#include <cstdlib>

using aidl::vendor::oplus::hardware::touch::IOplusTouch;

namespace aidl::vendor::aospa::power {

static void setDoubleTapEnabled(bool enabled) {
    const std::string instance = std::string() + IOplusTouch::descriptor + "/default";
    std::shared_ptr<IOplusTouch> oplusTouch = IOplusTouch::fromBinder(
            ndk::SpAIBinder(AServiceManager_waitForService(instance.c_str())));
    if (oplusTouch == nullptr) {
        LOG(ERROR) << "Failed to get " << instance;
        return;
    }

    std::string tmp;
    if (!oplusTouch->touchReadNodeFile(OplusTouchConstants::DEFAULT_TP_IC_ID,
                                       OplusTouchConstants::DOUBLE_TAP_INDEP_NODE, &tmp)
                 .isOk()) {
        LOG(ERROR) << "Failed to read double tap node";
        return;
    }

    errno = 0;
    char* end = nullptr;
    const long parsed = std::strtol(tmp.c_str(), &end, 16);
    if (end == tmp.c_str() || errno == ERANGE) {
        LOG(ERROR) << "Unparseable double tap node contents: '" << tmp << "'";
        return;
    }
    int contents = static_cast<int>(parsed);
    if (enabled) {
        contents |= OplusTouchConstants::DOUBLE_TAP_GESTURE;
    } else {
        contents &= ~OplusTouchConstants::DOUBLE_TAP_GESTURE;
    }

    LOG(INFO) << "Power feature: DOUBLE_TAP enabled: " << enabled;
    int aidl_return = 0;
    oplusTouch->touchWriteNodeFile(OplusTouchConstants::DEFAULT_TP_IC_ID,
                                   OplusTouchConstants::DOUBLE_TAP_ENABLE_NODE, "1",
                                   &aidl_return);
    oplusTouch->touchWriteNodeFile(OplusTouchConstants::DEFAULT_TP_IC_ID,
                                   OplusTouchConstants::DOUBLE_TAP_INDEP_NODE,
                                   std::to_string(contents), &aidl_return);
}

bool setDeviceSpecificFeature(Feature feature, bool enabled) {
    switch (feature) {
        case Feature::DOUBLE_TAP:
            setDoubleTapEnabled(enabled);
            return true;
        case Feature::GESTURES:
            // The master switch is already folded into the per-gesture state by
            // PowerManagerService (DOUBLE_TAP is sent as double tap && gestures).
            return true;
        default:
            return false;
    }
}

}  // namespace aidl::vendor::aospa::power
