/*
 * SPDX-FileCopyrightText: 2024-2025 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aidl/android/hardware/power/BnPower.h>
#include <aidl/vendor/oplus/hardware/charger/ICharger.h>
#include <aidl/vendor/oplus/hardware/touch/IOplusTouch.h>

#include <android-base/logging.h>
#include <android/binder_manager.h>

#include <OplusTouchConstants.h>

#include <cerrno>
#include <cstdlib>
#include <iterator>
#include <mutex>

using aidl::android::hardware::power::Mode;
using aidl::vendor::oplus::hardware::charger::ICharger;
using aidl::vendor::oplus::hardware::touch::IOplusTouch;

#ifndef LIBPERFMGR_EXT
// vendor/qcom/opensource/power/utils.h
extern "C" int interaction_with_handle(int lock_handle, int duration, int num_args,
                                       int opt_list[]);
extern "C" void release_request(int lock_handle);
#endif

#ifdef LIBPERFMGR_EXT
namespace aidl::google::hardware::power::impl::pixel {
#else
namespace aidl::android::hardware::power::impl {
#endif

#ifndef LIBPERFMGR_EXT
namespace {

// Battery saver caps for canoe: salami's LOW_POWER share of each cluster's top
// step (59% big, 39% prime). perfd takes MHz and rounds up to the next step.
int sLowPowerCaps[] = {
        0x40804000, 2112,  // cluster 0 (policy0) max: 2112000 of 3628800 kHz
        0x40804200, 1747,  // cluster 2 (policy6) max: 1747200 of 4608000 kHz
        0x42808000, 6,     // kgsl max_pwrlevel: 726 of 1200 MHz (578/826 on other bins)
};

std::mutex sLowPowerLock;
int sLowPowerHandle = 0;

void setLowPower(bool enabled) {
    std::lock_guard<std::mutex> lock(sLowPowerLock);
    if (enabled == (sLowPowerHandle > 0)) return;

    if (enabled) {
        const int handle = interaction_with_handle(
                0, 0, static_cast<int>(std::size(sLowPowerCaps)), sLowPowerCaps);
        if (handle > 0) {
            sLowPowerHandle = handle;
        } else {
            LOG(ERROR) << "Failed to cap clocks for battery saver";
        }
    } else {
        release_request(sLowPowerHandle);
        sLowPowerHandle = 0;
    }
}

}  // namespace
#endif

bool isDeviceSpecificModeSupported(Mode type, bool* _aidl_return) {
    switch (type) {
#ifndef TAP_TO_WAKE_NODE
        case Mode::DOUBLE_TAP_TO_WAKE:
            *_aidl_return = true;
            return true;
#endif
#ifndef LIBPERFMGR_EXT
        case Mode::LOW_POWER:
            *_aidl_return = true;
            return true;
#endif
        default:
            return false;
    }
}

bool setDeviceSpecificMode(Mode type, bool enabled) {
    switch (type) {
#ifndef TAP_TO_WAKE_NODE
        case Mode::DOUBLE_TAP_TO_WAKE: {
            std::string tmp;
            int contents = 0;

            const std::string instance = std::string() + IOplusTouch::descriptor + "/default";
            std::shared_ptr<IOplusTouch> oplusTouch = IOplusTouch::fromBinder(
                    ndk::SpAIBinder(AServiceManager_waitForService(instance.c_str())));
            if (oplusTouch == nullptr) {
                LOG(ERROR) << "Failed to get " << instance;
                return true;
            }

            LOG(INFO) << "Power mode: " << toString(type) << " isDoubleTapEnabled: " << enabled;

            if (!oplusTouch->touchReadNodeFile(OplusTouchConstants::DEFAULT_TP_IC_ID,
                                               OplusTouchConstants::DOUBLE_TAP_INDEP_NODE, &tmp)
                         .isOk()) {
                LOG(ERROR) << "Failed to read double tap node";
                return true;
            }

            errno = 0;
            char* end = nullptr;
            const long parsed = std::strtol(tmp.c_str(), &end, 16);
            if (end == tmp.c_str() || errno == ERANGE) {
                LOG(ERROR) << "Unparseable double tap node contents: '" << tmp << "'";
                return true;
            }
            contents = static_cast<int>(parsed);

            if (enabled) {
                contents |= OplusTouchConstants::DOUBLE_TAP_GESTURE;
            } else {
                contents &= ~OplusTouchConstants::DOUBLE_TAP_GESTURE;
            }

            int aidl_return = 0;
            oplusTouch->touchWriteNodeFile(OplusTouchConstants::DEFAULT_TP_IC_ID,
                                           OplusTouchConstants::DOUBLE_TAP_ENABLE_NODE, "1",
                                           &aidl_return);
            oplusTouch->touchWriteNodeFile(OplusTouchConstants::DEFAULT_TP_IC_ID,
                                           OplusTouchConstants::DOUBLE_TAP_INDEP_NODE,
                                           std::to_string(contents), &aidl_return);
            return true;
        }
#endif
#ifndef LIBPERFMGR_EXT
        case Mode::LOW_POWER:
            setLowPower(enabled);
            return true;
#endif
        case Mode::INTERACTIVE: {
            const std::string instance = std::string() + ICharger::descriptor + "/default";
            std::shared_ptr<ICharger> charger = ICharger::fromBinder(
                    ndk::SpAIBinder(AServiceManager_checkService(instance.c_str())));
            if (charger) {
                int aidl_return = 0;
                charger->notifyScreenStatus(enabled, &aidl_return);
            }
            return false;
        }
        default:
            return false;
    }
}

}  // namespace
