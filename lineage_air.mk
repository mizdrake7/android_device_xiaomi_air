#
# SPDX-FileCopyrightText: The LineageOS Project
# SPDX-License-Identifier: Apache-2.0
#

# Inherit from those products. Most specific first.
$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit_only.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base_telephony.mk)

# Inherit from device makefile.
$(call inherit-product, device/xiaomi/air/device.mk)

# Inherit some common LineageOS stuff.
$(call inherit-product, vendor/lineage/config/common_full_phone.mk)

PRODUCT_NAME := lineage_air
PRODUCT_DEVICE := air
PRODUCT_MANUFACTURER := Xiaomi
PRODUCT_BRAND := Redmi
PRODUCT_MODEL := 23124RN87G

PRODUCT_BUILD_PROP_OVERRIDES += \
    BuildDesc="air_global-user 15 AP3A.240905.015.A23 OS2.0.206.0.VGQMIXM release-keys" \
    BuildFingerprint=Redmi/air_global/air:15/AP3A.240905.015.A2/OS2.0.206.0.VGQMIXM:user/release-keys

PRODUCT_GMS_CLIENTID_BASE := android-xiaomi
