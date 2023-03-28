vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO Chlumsky/msdf-atlas-gen
    REF f6a1bc9f76cdaee40a4392e4b4c151cdc959bbb9
    SHA512 d992258037d113fd3f44b1f5b6511993900b9572c3701aade1ad9af0e9bd7a7939f77f0c7e8a730111eaacbdd5425d5e241ad3f52129bf6639a0b128db895781
    HEAD_REF master
)

vcpkg_check_features(
    OUT_FEATURE_OPTIONS FEATURE_OPTIONS
    FEATURES
        geometry-preprocessing MSDF_ATLAS_USE_SKIA
)

if (VCPKG_CRT_LINKAGE STREQUAL dynamic)
    set(MSDF_ATLAS_DYNAMIC_RUNTIME ON)
else()
    set(MSDF_ATLAS_DYNAMIC_RUNTIME OFF)
endif()

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        -DMSDF_ATLAS_BUILD_STANDALONE=OFF
        -DMSDF_ATLAS_NO_ARTERY_FONT=ON
        -DMSDF_ATLAS_MSDFGEN_EXTERNAL=ON
        -DMSDF_ATLAS_INSTALL=ON
        -DMSDF_ATLAS_DYNAMIC_RUNTIME="${MSDF_ATLAS_DYNAMIC_RUNTIME}"
        -DBUILD_SHARED_LIBS=ON
        ${FEATURE_OPTIONS}
    MAYBE_UNUSED_VARIABLES
        MSDFGEN_VCPKG_FEATURES_SET
)
vcpkg_cmake_install()
vcpkg_cmake_config_fixup(CONFIG_PATH lib/cmake)


file(
  INSTALL "${SOURCE_PATH}/LICENSE.txt"
  DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}"
  RENAME copyright
)

file(
    COPY "${CURRENT_PACKAGES_DIR}/share/${PORT}/${PORT}"
    DESTINATION "${CURRENT_PACKAGES_DIR}/share"
)
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/share/${PORT}/${PORT}")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/share")

if (WIN32)
    vcpkg_replace_string("${CURRENT_PACKAGES_DIR}/include/msdf-atlas-gen/Charset.h" "MSDF_ATLAS_PUBLIC" "__declspec(dllimport)")
else()
    vcpkg_replace_string("${CURRENT_PACKAGES_DIR}/include/msdf-atlas-gen/Charset.h" "MSDF_ATLAS_PUBLIC " "")
endif()