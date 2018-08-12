cmake_minimum_required(VERSION 2.8.11)
project(qtkeychain)

include(FindPkgConfig)

###

set(QTKEYCHAIN_VERSION 0.9.90)
set(QTKEYCHAIN_SOVERSION 1)

###

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${PROJECT_SOURCE_DIR}/cmake/Modules")
include(GNUInstallDirs)
include(GenerateExportHeader)
include(ECMPackageConfigHelpers)
include(ECMSetupVersion)
include(ECMGeneratePriFile)

option(BUILD_WITH_QT4 "Build qtkeychain with Qt4 no matter if Qt5 was found" OFF)
option(BUILD_TEST_APPLICATION "Build test application" ON)
option(BUILD_TRANSLATIONS "Build translations" ON)
option(QTKEYCHAIN_STATIC "Build static library" OFF)

if(CMAKE_SYSTEM_NAME STREQUAL Android)
    set(ANDROID 1)
endif()

if (WIN32)
    option(USE_CREDENTIAL_STORE "Build with windows CredentialStore support" ON)

    if (USE_CREDENTIAL_STORE)
        add_definitions(-DUSE_CREDENTIAL_STORE=1)
    endif()
endif()

if( NOT BUILD_WITH_QT4 )
    # try Qt5 first, and prefer that if found
    find_package(Qt5Core QUIET)
endif()

if (Qt5Core_FOUND AND NOT BUILD_WITH_QT4)
  set(QTKEYCHAIN_VERSION_INFIX 5)

  if(UNIX AND NOT APPLE AND NOT ANDROID)
    find_package(Qt5DBus REQUIRED)
    include_directories(${Qt5DBus_INCLUDE_DIRS})
    set(QTDBUS_LIBRARIES ${Qt5DBus_LIBRARIES})
    macro(qt_add_dbus_interface)
      qt5_add_dbus_interface(${ARGN})
    endmacro()
  endif()

  if(BUILD_TRANSLATIONS)
    find_package(Qt5LinguistTools REQUIRED)
    macro(qt_add_translation)
      qt5_add_translation(${ARGN})
    endmacro(qt_add_translation)
    macro(qt_create_translation)
      qt5_create_translation(${ARGN})
    endmacro(qt_create_translation)
  endif()

  macro(qt_wrap_cpp)
    qt5_wrap_cpp(${ARGN})
  endmacro()

  set(QTCORE_LIBRARIES ${Qt5Core_LIBRARIES})
  include_directories(${Qt5Core_INCLUDE_DIRS})

  if (NOT Qt5Core_VERSION VERSION_LESS "5.7.0")
    if (CMAKE_COMPILER_IS_GNUCXX)
      if ((NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.7.0") AND (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "6.1.0"))
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
      elseif(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.7.0")
        message(FATAL_ERROR "Can't build QtKeychain using g++-${CMAKE_CXX_COMPILER_VERSION} and Qt ${Qt5Core_VERSION}: compiler supporting C++11 is required")
      endif()
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      if (NOT ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 3.3)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
      else()
        message(FATAL_ERROR "Can't build QtKeychain using clang++-${CMAKE_CXX_COMPILER_VERSION} and Qt ${Qt5Core_VERSION}: compiler supporting C++11 is required")
      endif()
    elseif ((CMAKE_CXX_COMPILER_ID MATCHES "MSVC") AND (MSVC_VERSION LESS 1700))
      message(FATAL_ERROR "Can't build QtKeychain using VC++-${MSVC_VERSION} and Qt ${Qt5Core_VERSION}: compiler supporting C++11 is required")
    endif()
  endif()
else()
  set(QTKEYCHAIN_VERSION_INFIX "")
  if(UNIX AND NOT APPLE)
    find_package(Qt4 COMPONENTS QtCore QtDBus REQUIRED)
    set(QTDBUS_LIBRARIES ${QT_QTDBUS_LIBRARY})
    macro(qt_add_dbus_interface)
      qt4_add_dbus_interface(${ARGN})
    endmacro()
  else()
    find_package(Qt4 COMPONENTS QtCore REQUIRED)
  endif()
  include_directories(${QT_INCLUDES})
  set(QTCORE_LIBRARIES ${QT_QTCORE_LIBRARY})
  macro(qt_add_translation)
    qt4_add_translation(${ARGN})
  endmacro(qt_add_translation)
  macro(qt_create_translation)
    qt4_create_translation(${ARGN})
  endmacro(qt_create_translation)
  macro(qt_wrap_cpp)
    qt4_wrap_cpp(${ARGN})
  endmacro()
endif()


include_directories(${CMAKE_CURRENT_BINARY_DIR})

list(APPEND qtkeychain_LIBRARIES ${QTCORE_LIBRARIES})
set(qtkeychain_SOURCES
    keychain.cpp
    qkeychain_export.h
    keychain.h
)

add_definitions( -Wall )

if(WIN32)
    list(APPEND qtkeychain_SOURCES keychain_win.cpp)
    if (NOT USE_CREDENTIAL_STORE)
        list(APPEND qtkeychain_LIBRARIES crypt32)
        list(APPEND qtkeychain_SOURCES plaintextstore.cpp)
    endif()
    #FIXME: mingw bug; otherwise getting undefined refs to RtlSecureZeroMemory there
    if(MINGW)
        add_definitions( -O2 )
    endif()
endif()

if(APPLE)
    if(IOS)
        list(APPEND qtkeychain_SOURCES keychain_ios.cpp)
    else()
        list(APPEND qtkeychain_SOURCES keychain_mac.cpp)
    endif()

    find_library(COREFOUNDATION_LIBRARY CoreFoundation REQUIRED)
    list(APPEND qtkeychain_LIBRARIES ${COREFOUNDATION_LIBRARY})

    find_library(SECURITY_LIBRARY Security REQUIRED)
    list(APPEND qtkeychain_LIBRARIES ${SECURITY_LIBRARY})
endif()

if(UNIX AND NOT APPLE AND NOT ANDROID)
    option(LIBSECRET_SUPPORT "Build with libsecret support" ON)

    if(LIBSECRET_SUPPORT)
        pkg_check_modules(LIBSECRET libsecret-1)

        if (LIBSECRET_FOUND)
            add_definitions(-DHAVE_LIBSECRET=1)
        endif()
        INCLUDE_DIRECTORIES(${LIBSECRET_INCLUDE_DIRS})
        list(APPEND qtkeychain_LIBRARIES ${LIBSECRET_LIBRARIES})
    endif()

    list(APPEND qtkeychain_SOURCES keychain_unix.cpp gnomekeyring.cpp libsecret.cpp plaintextstore.cpp)
    qt_add_dbus_interface(qtkeychain_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/org.kde.KWallet.xml kwallet_interface KWalletInterface)
    list(APPEND qtkeychain_LIBRARIES ${QTDBUS_LIBRARIES} )
endif()

QT_WRAP_CPP(qtkeychain_MOC_OUTFILES keychain.h keychain_p.h gnomekeyring_p.h)

set(qtkeychain_TR_FILES
      translations/qtkeychain_de.ts
      translations/qtkeychain_ro.ts
)

file(GLOB qtkeychain_TR_SOURCES *.cpp *.h *.ui)
if ( BUILD_TRANSLATIONS )
    qt_create_translation(qtkeychain_MESSAGES ${qtkeychain_TR_SOURCES} ${qtkeychain_TR_FILES})
    qt_add_translation(qtkeychain_QM_FILES ${qtkeychain_TR_FILES})
    add_custom_target(messages DEPENDS ${qtkeychain_MESSAGES})
    add_custom_target(translations DEPENDS ${qtkeychain_QM_FILES})

    if(NOT QT_TRANSLATIONS_DIR)
	# If this directory is missing, we are in a Qt5 environment.
	# Extract the qmake executable location
	get_target_property(QT5_QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
	# Ask Qt5 where to put the translations
	execute_process( COMMAND ${QT5_QMAKE_EXECUTABLE} -query QT_INSTALL_TRANSLATIONS
	    OUTPUT_VARIABLE qt_translations_dir OUTPUT_STRIP_TRAILING_WHITESPACE )
	# make sure we have / and not \ as qmake gives on windows
	file( TO_CMAKE_PATH "${qt_translations_dir}" qt_translations_dir)
	set( QT_TRANSLATIONS_DIR ${qt_translations_dir} CACHE PATH
	     "The location of the Qt translations" FORCE)
    endif()

    install(FILES ${qtkeychain_QM_FILES}
	    DESTINATION ${QT_TRANSLATIONS_DIR})
endif( BUILD_TRANSLATIONS )

set(QTKEYCHAIN_TARGET_NAME qt${QTKEYCHAIN_VERSION_INFIX}keychain)
if(NOT QTKEYCHAIN_STATIC)
    add_library(${QTKEYCHAIN_TARGET_NAME} SHARED ${qtkeychain_SOURCES} ${qtkeychain_MOC_OUTFILES} ${qtkeychain_QM_FILES})
else()
    add_library(${QTKEYCHAIN_TARGET_NAME} STATIC ${qtkeychain_SOURCES} ${qtkeychain_MOC_OUTFILES} ${qtkeychain_QM_FILES})
endif()

target_link_libraries(${QTKEYCHAIN_TARGET_NAME} PUBLIC ${qtkeychain_LIBRARIES})
target_include_directories(${QTKEYCHAIN_TARGET_NAME} PUBLIC $<INSTALL_INTERFACE:include/>)

generate_export_header(${QTKEYCHAIN_TARGET_NAME}
  EXPORT_FILE_NAME qkeychain_export.h
  EXPORT_MACRO_NAME QKEYCHAIN_EXPORT
)

set_target_properties(${QTKEYCHAIN_TARGET_NAME} PROPERTIES
    VERSION ${QTKEYCHAIN_VERSION}
    SOVERSION ${QTKEYCHAIN_SOVERSION}
    MACOSX_RPATH 1
    INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}"
    INSTALL_RPATH_USE_LINK_PATH TRUE
)

install(FILES keychain.h ${CMAKE_CURRENT_BINARY_DIR}/qkeychain_export.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/qt${QTKEYCHAIN_VERSION_INFIX}keychain/
)

install(TARGETS ${QTKEYCHAIN_TARGET_NAME}
  EXPORT Qt${QTKEYCHAIN_VERSION_INFIX}KeychainLibraryDepends
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

if(BUILD_TEST_APPLICATION)
    add_executable( testclient testclient.cpp )
    target_link_libraries( testclient ${QTKEYCHAIN_TARGET_NAME})
endif()


###
### CMake config file
###

ecm_configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/QtKeychainConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/Qt${QTKEYCHAIN_VERSION_INFIX}KeychainConfig.cmake"
  INSTALL_DESTINATION Qt${QTKEYCHAIN_VERSION_INFIX}Keychain)

ecm_setup_version("${QTKEYCHAIN_VERSION}" VARIABLE_PREFIX SNORE
  PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/Qt${QTKEYCHAIN_VERSION_INFIX}KeychainConfigVersion.cmake"
  SOVERSION ${QTKEYCHAIN_VERSION})

if(UNIX AND NOT APPLE AND NOT ANDROID)
    set(PRI_EXTRA_DEPS "dbus")
endif()
ecm_generate_pri_file(BASE_NAME Qt${QTKEYCHAIN_VERSION_INFIX}Keychain
  LIB_NAME ${QTKEYCHAIN_TARGET_NAME}
  DEPS "core ${PRI_EXTRA_DEPS}"
  INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR}
  FILENAME_VAR pri_filename)

install(FILES ${pri_filename} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})


install(EXPORT Qt${QTKEYCHAIN_VERSION_INFIX}KeychainLibraryDepends
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Qt${QTKEYCHAIN_VERSION_INFIX}Keychain"
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Qt${QTKEYCHAIN_VERSION_INFIX}KeychainConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/Qt${QTKEYCHAIN_VERSION_INFIX}KeychainConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Qt${QTKEYCHAIN_VERSION_INFIX}Keychain
)

