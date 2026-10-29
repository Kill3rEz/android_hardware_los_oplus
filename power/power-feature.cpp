/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

// Device extension for the AOSPA power feature HAL (vendor.aospa.power-service): the OPlus touch
// driver does not take plain 0/1 gesture nodes but a gesture bitmask, so the off-screen gestures
// are handled here through IOplusTouch.

#include <aidl/vendor/aospa/power/Feature.h>
#include <aidl/vendor/oplus/hardware/touch/IOplusTouch.h>

#include <android-base/logging.h>
#include <android/binder_manager.h>

#include <OplusTouchConstants.h>

#include <cerrno>
#include <cstdlib>
#include <mutex>
#include <unordered_map>

using aidl::vendor::oplus::hardware::touch::IOplusTouch;

namespace aidl::vendor::aospa::power {

// Gesture ids from the OPlus touchpanel driver (touchpanel_common.h); the driver reports
// double tap as KEY_WAKEUP and every other gesture as KEY_GESTURE_START (246) + id.
static const std::unordered_map<Feature, int> GESTURE_MAP = {
    {Feature::DOUBLE_TAP, 1},
    {Feature::DRAW_V, 2},
    {Feature::DRAW_INVERSE_V, 3},
    {Feature::DRAW_O, 6},
    {Feature::DRAW_M, 12},
    {Feature::DRAW_W, 13},
    {Feature::DRAW_ARROW_LEFT, 5},
    {Feature::DRAW_ARROW_RIGHT, 4},
    {Feature::ONE_FINGER_SWIPE_UP, 11},
    {Feature::ONE_FINGER_SWIPE_RIGHT, 8},
    {Feature::ONE_FINGER_SWIPE_DOWN, 10},
    {Feature::ONE_FINGER_SWIPE_LEFT, 9},
    {Feature::TWO_FINGER_SWIPE, 7},
    {Feature::SINGLE_TAP, 16},
};

static std::mutex gGestureLock;

static void setGestureEnabled(Feature feature, int id, bool enabled) {
    const std::string instance = std::string() + IOplusTouch::descriptor + "/default";
    std::shared_ptr<IOplusTouch> oplusTouch = IOplusTouch::fromBinder(
            ndk::SpAIBinder(AServiceManager_waitForService(instance.c_str())));
    if (oplusTouch == nullptr) {
        LOG(ERROR) << "Failed to get " << instance;
        return;
    }

    std::lock_guard<std::mutex> lock(gGestureLock);

    std::string tmp;
    if (!oplusTouch->touchReadNodeFile(OplusTouchConstants::DEFAULT_TP_IC_ID,
                                       OplusTouchConstants::DOUBLE_TAP_INDEP_NODE, &tmp)
                 .isOk()) {
        LOG(ERROR) << "Failed to read gesture node";
        return;
    }

    errno = 0;
    char* end = nullptr;
    const long parsed = std::strtol(tmp.c_str(), &end, 16);
    if (end == tmp.c_str() || errno == ERANGE) {
        LOG(ERROR) << "Unparseable gesture node contents: '" << tmp << "'";
        return;
    }
    int contents = static_cast<int>(parsed);
    if (enabled) {
        contents |= (1 << id);
    } else {
        contents &= ~(1 << id);
    }

    LOG(INFO) << "Power feature: " << toString(feature) << " enabled: " << enabled;
    int aidl_return = 0;
    oplusTouch->touchWriteNodeFile(OplusTouchConstants::DEFAULT_TP_IC_ID,
                                   OplusTouchConstants::DOUBLE_TAP_ENABLE_NODE, "1",
                                   &aidl_return);
    oplusTouch->touchWriteNodeFile(OplusTouchConstants::DEFAULT_TP_IC_ID,
                                   OplusTouchConstants::DOUBLE_TAP_INDEP_NODE,
                                   std::to_string(contents), &aidl_return);
}

bool setDeviceSpecificFeature(Feature feature, bool enabled) {
    if (feature == Feature::GESTURES) {
        // The master switch is already folded into the per-gesture state by
        // PowerManagerService (each gesture is sent as gesture && gestures).
        return true;
    }

    auto gesture = GESTURE_MAP.find(feature);
    if (gesture == GESTURE_MAP.end()) {
        return false;
    }

    setGestureEnabled(feature, gesture->second, enabled);
    return true;
}

}  // namespace aidl::vendor::aospa::power
