cmake_minimum_required(VERSION 3.20.0)

file(READ VERSION VERSION)
string(REGEX MATCHALL "[0-9]+" VERSION_LIST ${VERSION})
list(GET VERSION_LIST 0 VERSION_MAJOR)
list(GET VERSION_LIST 1 VERSION_MINOR)
list(GET VERSION_LIST 2 VERSION_PATCH)

set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "Minimum macOS version" FORCE)

project(apk-editor-studio
    VERSION ${VERSION}
    HOMEPAGE_URL https://qwertycube.com/apk-editor-studio/
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(UNIX AND NOT APPLE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()

add_executable(apk-editor-studio)
add_subdirectory(src)
find_package(Qt5 COMPONENTS Widgets Xml Network LinguistTools REQUIRED)

target_compile_definitions(apk-editor-studio PRIVATE
    APPLICATION="APK Editor Studio"
    VERSION="${VERSION}"
)

option(PORTABLE "Portable build" OFF)

if(PORTABLE)
    target_compile_definitions(apk-editor-studio PRIVATE PORTABLE)
endif()

if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Release")
    set_property(TARGET apk-editor-studio PROPERTY WIN32_EXECUTABLE TRUE)
endif()

set(TS_FILES
    src/translations/apk-editor-studio.ar.ts
    src/translations/apk-editor-studio.az.ts
    src/translations/apk-editor-studio.ckb.ts
    src/translations/apk-editor-studio.cs.ts
    src/translations/apk-editor-studio.de.ts
    src/translations/apk-editor-studio.el.ts
    src/translations/apk-editor-studio.en_GB.ts
    src/translations/apk-editor-studio.es.ts
    src/translations/apk-editor-studio.es_MX.ts
    src/translations/apk-editor-studio.fa.ts
    src/translations/apk-editor-studio.fr.ts
    src/translations/apk-editor-studio.he.ts
    src/translations/apk-editor-studio.hu.ts
    src/translations/apk-editor-studio.id.ts
    src/translations/apk-editor-studio.it.ts
    src/translations/apk-editor-studio.ja.ts
    src/translations/apk-editor-studio.ko.ts
    src/translations/apk-editor-studio.pl.ts
    src/translations/apk-editor-studio.pt.ts
    src/translations/apk-editor-studio.ro.ts
    src/translations/apk-editor-studio.ru.ts
    src/translations/apk-editor-studio.sv.ts
    src/translations/apk-editor-studio.tr.ts
    src/translations/apk-editor-studio.zh_CN.ts
)

set_source_files_properties(${TS_FILES} PROPERTIES
    # TODO Output .qm files to build directory
    OUTPUT_LOCATION ${CMAKE_SOURCE_DIR}/dist/all/resources/translations
)

qt5_add_translation(QM_FILES ${TS_FILES})
target_sources(apk-editor-studio PRIVATE ${QM_FILES})
target_include_directories(apk-editor-studio PRIVATE src lib)

# KSyntaxHighlighting
add_subdirectory(lib/KSyntaxHighlighting EXCLUDE_FROM_ALL)

# QtKeychain
set(QTKEYCHAIN_STATIC ON CACHE BOOL "Build QtKeychain statically")
set(BUILD_TRANSLATIONS OFF CACHE BOOL "Build QtKeychain translations")
add_subdirectory(lib/QtKeychain EXCLUDE_FROM_ALL)
target_compile_definitions(apk-editor-studio PRIVATE QTKEYCHAIN_NO_EXPORT)

# SingleApplication
set(QAPPLICATION_CLASS QApplication CACHE STRING "Inheritance class for SingleApplication")
add_subdirectory(lib/SingleApplication EXCLUDE_FROM_ALL)

# DarkStyle
target_sources(apk-editor-studio PRIVATE
    lib/DarkStyle/DarkStyle.cpp
    lib/DarkStyle/darkstyle.qrc
)

target_link_libraries(apk-editor-studio
    Qt5::Widgets
    Qt5::Xml
    Qt5::Network
    KSyntaxHighlighting
    SingleApplication::SingleApplication
    qt5keychain
)

# Deployment

macro(deploy)
    add_custom_command(
        TARGET apk-editor-studio POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${ARGV1}
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/dist/${ARGV0}
                ${ARGV1}
    )
endmacro()

function(find OUTPUT_VARIABLE FILE SEARCH_PATH)
    cmake_path(NORMAL_PATH SEARCH_PATH OUTPUT_VARIABLE SEARCH_PATH)
    find_file(${OUTPUT_VARIABLE} ${FILE} PATHS ${SEARCH_PATH}
        NO_DEFAULT_PATH NO_PACKAGE_ROOT_PATH NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH
        NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH NO_CMAKE_FIND_ROOT_PATH
    )
endfunction()

if(NOT QT_QMAKE_EXECUTABLE)
    get_target_property(QT_QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
endif()
get_filename_component(QT_BIN_DIR "${QT_QMAKE_EXECUTABLE}" DIRECTORY)

if(WIN32)
    deploy(all $<TARGET_FILE_DIR:apk-editor-studio>)
    deploy(windows $<TARGET_FILE_DIR:apk-editor-studio>)

    # Qt libraries
    add_custom_command(
        TARGET apk-editor-studio POST_BUILD
        COMMAND ${QT_BIN_DIR}\\windeployqt
                --core
                --gui
                --network
                --widgets
                --no-quick-import
                --no-translations
                --no-system-d3d-compiler
                --no-compiler-runtime
                --no-angle
                --no-opengl-sw
                $<TARGET_FILE:apk-editor-studio>
    )

    # OpenSSL DLLs
    if(OPENSSL_ROOT_DIR)
        find(OPENSSL_LIBCRYPTO_DLL "libcrypto-1_1.dll" "${OPENSSL_ROOT_DIR}/bin")
        find(OPENSSL_LIBSSL_DLL "libssl-1_1.dll" "${OPENSSL_ROOT_DIR}/bin")
        add_custom_command(
            TARGET apk-editor-studio POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${OPENSSL_LIBCRYPTO_DLL} $<TARGET_FILE_DIR:apk-editor-studio>
            COMMAND ${CMAKE_COMMAND} -E copy ${OPENSSL_LIBSSL_DLL} $<TARGET_FILE_DIR:apk-editor-studio>
        )
    else()
        message("OpenSSL deployment disabled: OPENSSL_ROOT_DIR is not provided")
    endif()

    # Windows icons, version info, etc.
    configure_file(
        src/windows.rc.in
        ${CMAKE_BINARY_DIR}/windows.rc
    @ONLY)
    target_sources(apk-editor-studio PRIVATE ${CMAKE_BINARY_DIR}/windows.rc)

elseif(UNIX AND NOT APPLE)
    deploy(all ${CMAKE_BINARY_DIR}/share/apk-editor-studio)
    deploy(linux ${CMAKE_BINARY_DIR})

elseif(APPLE)
    set_target_properties(apk-editor-studio PROPERTIES
        MACOSX_BUNDLE ON
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/dist/macos/Info.plist.in
    )
    deploy(all $<TARGET_FILE_DIR:apk-editor-studio>)
    deploy(macos/app $<TARGET_FILE_DIR:apk-editor-studio>/../..)
    add_custom_command(
        TARGET apk-editor-studio POST_BUILD
        COMMAND ${QT_BIN_DIR}/macdeployqt
                $<TARGET_FILE_DIR:apk-editor-studio>/../..
    )
endif()

# Installation

if(WIN32)
    install(DIRECTORY $<TARGET_FILE_DIR:apk-editor-studio>/ DESTINATION .)
elseif(UNIX AND NOT APPLE)
    install(DIRECTORY ${CMAKE_BINARY_DIR}/bin/ DESTINATION bin USE_SOURCE_PERMISSIONS)
    install(DIRECTORY ${CMAKE_BINARY_DIR}/share/ DESTINATION share)
endif()
