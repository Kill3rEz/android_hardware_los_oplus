/*
 * SPDX-FileCopyrightText: 2024-2025 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aidl/android/hardware/power/BnPower.h>
#include <aidl/vendor/oplus/hardware/touch/IOplusTouch.h>

#include <android-base/logging.h>
#include <android/binder_manager.h>

#include <OplusTouchConstants.h>

#include <cerrno>
#include <cstdlib>

using aidl::android::hardware::power::Mode;
using aidl::vendor::oplus::hardware::touch::IOplusTouch;

#ifdef LIBPERFMGR_EXT
namespace aidl::google::hardware::power::impl::pixel {
#else
namespace aidl::android::hardware::power::impl {
#endif

bool isDeviceSpecificModeSupported(Mode type, bool* _aidl_return) {
    switch (type) {
        case Mode::DOUBLE_TAP_TO_WAKE:
            *_aidl_return = true;
            return true;
        default:
            return false;
    }
}

bool setDeviceSpecificMode(Mode type, bool enabled) {
    switch (type) {
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
        default:
            return false;
    }
}

}  // namespace
