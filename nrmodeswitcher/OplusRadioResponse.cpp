/*
 * SPDX-FileCopyrightText: 2025 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "OplusRadioResponse.h"

namespace aidl::vendor::oplus::hardware::radio {

void OplusRadioResponse::resetNrModeResult() {
    std::lock_guard lock(mutex_);
    nr_mode_result_.reset();
}

std::optional<int32_t> OplusRadioResponse::waitForNrModeResult(std::chrono::seconds timeout) {
    std::unique_lock lock(mutex_);
    cv_.wait_for(lock, timeout, [this] { return nr_mode_result_.has_value(); });
    return nr_mode_result_;
}

ndk::ScopedAStatus OplusRadioResponse::setCallbackExtResponse() {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus OplusRadioResponse::setNrModeResponse(const OplusRadioResponseInfo& info,
                                                         int32_t result) {
    {
        std::lock_guard lock(mutex_);
        nr_mode_result_ = result;
    }
    cv_.notify_all();
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus OplusRadioResponse::getNrModeResponse(const OplusRadioResponseInfo& info,
                                                         int32_t result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus OplusRadioResponse::setModemErrorFatalResponse(
        const OplusRadioResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus OplusRadioResponse::setVoNrEnabledResponse(const OplusRadioResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus OplusRadioResponse::getVoNrEnabledResponse(const OplusRadioResponseInfo& info,
                                                              bool result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus OplusRadioResponse::setEccListResponse(const OplusRadioResponseInfo& info,
                                                          const std::string& result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus OplusRadioResponse::SetHsrModeForListeningResponse(
        const OplusRadioResponseInfo& info, int32_t result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus OplusRadioResponse::SetLogIdForListeningResponse(
        const OplusRadioResponseInfo& info, int32_t result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus OplusRadioResponse::setSlowStartResponse(const OplusRadioResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus OplusRadioResponse::setCtVolteModeResponse(const OplusRadioResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus OplusRadioResponse::simlockReqResponse(const OplusRadioResponseInfo& info,
                                                          const std::vector<uint8_t>& result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus OplusRadioResponse::updateRegionlockBlobResponse(
        const OplusRadioResponseInfo& info, const std::vector<uint8_t>& status) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus OplusRadioResponse::updateRegionlockStatusResponse(
        const OplusRadioResponseInfo& info, const std::vector<uint8_t>& status) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus OplusRadioResponse::getRegionlockStatusResponse(
        const OplusRadioResponseInfo& info, const std::vector<uint8_t>& status) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus OplusRadioResponse::setNwCongestionCfgResponse(
        const OplusRadioResponseInfo& info, const std::vector<uint8_t>& status) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus OplusRadioResponse::setModemEsimStatusResponse(
        const OplusRadioResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus OplusRadioResponse::getSimlockOperatorIdResponse(
        const OplusRadioResponseInfo& info, int32_t result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus OplusRadioResponse::getSimlockFeestateResponse(
        const OplusRadioResponseInfo& info, int32_t result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus OplusRadioResponse::setSimlockFeestateResponse(
        const OplusRadioResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus OplusRadioResponse::getSimlockUnlockStateResponse(
        const OplusRadioResponseInfo& info, int32_t result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus OplusRadioResponse::setSimlockFactoryResetTimeResponse(
        const OplusRadioResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus OplusRadioResponse::getSimlockFactoryResetTimeResponse(
        const OplusRadioResponseInfo& info, const std::vector<uint8_t>& status) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus OplusRadioResponse::setSimlockActivateTimeResponse(
        const OplusRadioResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus OplusRadioResponse::getSimlockActivateTimeResponse(
        const OplusRadioResponseInfo& info, const std::vector<uint8_t>& status) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus OplusRadioResponse::getSimlockFeatureResponse(
        const OplusRadioResponseInfo& info, const std::vector<uint8_t>& feature) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus OplusRadioResponse::getSimlockLockMarkResponse(
        const OplusRadioResponseInfo& info, const std::vector<uint8_t>& lockmark) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus OplusRadioResponse::getSimlockDeviceLockStateResponse(
        const OplusRadioResponseInfo& info, int32_t lockstate) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus OplusRadioResponse::getSimlockDeviceLockinfoResponse(
        const OplusRadioResponseInfo& info, const std::vector<uint8_t>& lockinfo) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus OplusRadioResponse::getSimlockVersionInfoResponse(
        const OplusRadioResponseInfo& info, const std::vector<uint8_t>& version) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus OplusRadioResponse::getSimlockMaxRetryResponse(
        const OplusRadioResponseInfo& info, int32_t result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus OplusRadioResponse::getSimlockCurrentRetryResponse(
        const OplusRadioResponseInfo& info, int32_t result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus OplusRadioResponse::setPsDetachAttachActionResponse(
        const OplusRadioResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus OplusRadioResponse::unlockRegionlockResponse(const OplusRadioResponseInfo& info,
                                                                int32_t result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus OplusRadioResponse::updateRegionlockKeyResponse(
        const OplusRadioResponseInfo& info, int32_t result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus OplusRadioResponse::getRegionlockSettingDataResponse(
        const OplusRadioResponseInfo& info, const std::vector<uint8_t>& result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus OplusRadioResponse::setRegionlockSettingDataResponse(
        const OplusRadioResponseInfo& info, int32_t result) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus OplusRadioResponse::setSimlockOfflineLockResponse(
        const OplusRadioResponseInfo& info) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus OplusRadioResponse::getSimlockOfflineLockResponse(
        const OplusRadioResponseInfo& info, const std::vector<int32_t>& retryCount) {
    return ndk::ScopedAStatus::ok();
}

};  // namespace aidl::vendor::oplus::hardware::radio
