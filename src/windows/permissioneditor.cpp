#include "windows/permissioneditor.h"
#include "windows/yesalwaysdialog.h"
#include "base/utils.h"
#include <QDesktopServices>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QToolButton>
#include <QUrl>

PermissionEditor::PermissionEditor(Manifest *manifest, QWidget *parent) : QDialog(parent), manifest(manifest)
{
    //: This string refers to multiple permissions (as in "Editor of permissions").
    setWindowTitle(tr("Permission Editor"));
    setWindowIcon(QIcon::fromTheme("tool-permissioneditor"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    resize(Utils::scale(550, 400));

    const QStringList permissionStrings = {
        "ACCEPT_HANDOVER",
        "ACCESS_BACKGROUND_LOCATION",
        "ACCESS_BLOBS_ACROSS_USERS",
        "ACCESS_CHECKIN_PROPERTIES",
        "ACCESS_COARSE_LOCATION",
        "ACCESS_FINE_LOCATION",
        "ACCESS_LOCATION_EXTRA_COMMANDS",
        "ACCESS_MEDIA_LOCATION",
        "ACCESS_NETWORK_STATE",
        "ACCESS_NOTIFICATION_POLICY",
        "ACCESS_WIFI_STATE",
        "ACCOUNT_MANAGER",
        "ACTIVITY_RECOGNITION",
        "ADD_VOICEMAIL",
        "ANSWER_PHONE_CALLS",
        "BATTERY_STATS",
        "BIND_ACCESSIBILITY_SERVICE",
        "BIND_APPWIDGET",
        "BIND_AUTOFILL_SERVICE",
        "BIND_CALL_REDIRECTION_SERVICE",
        "BIND_CARRIER_MESSAGING_CLIENT_SERVICE",
        "BIND_CARRIER_MESSAGING_SERVICE",
        "BIND_CARRIER_SERVICES",
        "BIND_CHOOSER_TARGET_SERVICE",
        "BIND_COMPANION_DEVICE_SERVICE",
        "BIND_CONDITION_PROVIDER_SERVICE",
        "BIND_CONTROLS",
        "BIND_DEVICE_ADMIN",
        "BIND_DREAM_SERVICE",
        "BIND_INCALL_SERVICE",
        "BIND_INPUT_METHOD",
        "BIND_MIDI_DEVICE_SERVICE",
        "BIND_NFC_SERVICE",
        "BIND_NOTIFICATION_LISTENER_SERVICE",
        "BIND_PRINT_SERVICE",
        "BIND_QUICK_ACCESS_WALLET_SERVICE",
        "BIND_QUICK_SETTINGS_TILE",
        "BIND_REMOTEVIEWS",
        "BIND_SCREENING_SERVICE",
        "BIND_TELECOM_CONNECTION_SERVICE",
        "BIND_TEXT_SERVICE",
        "BIND_TV_INPUT",
        "BIND_TV_INTERACTIVE_APP",
        "BIND_VISUAL_VOICEMAIL_SERVICE",
        "BIND_VOICE_INTERACTION",
        "BIND_VPN_SERVICE",
        "BIND_VR_LISTENER_SERVICE",
        "BIND_WALLPAPER",
        "BLUETOOTH",
        "BLUETOOTH_ADMIN",
        "BLUETOOTH_ADVERTISE",
        "BLUETOOTH_CONNECT",
        "BLUETOOTH_PRIVILEGED",
        "BLUETOOTH_SCAN",
        "BODY_SENSORS",
        "BODY_SENSORS_BACKGROUND",
        "BROADCAST_PACKAGE_REMOVED",
        "BROADCAST_SMS",
        "BROADCAST_STICKY",
        "BROADCAST_WAP_PUSH",
        "CALL_COMPANION_APP",
        "CALL_PHONE",
        "CALL_PRIVILEGED",
        "CAMERA",
        "CAPTURE_AUDIO_OUTPUT",
        "CHANGE_COMPONENT_ENABLED_STATE",
        "CHANGE_CONFIGURATION",
        "CHANGE_NETWORK_STATE",
        "CHANGE_WIFI_MULTICAST_STATE",
        "CHANGE_WIFI_STATE",
        "CLEAR_APP_CACHE",
        "CONTROL_LOCATION_UPDATES",
        "DELETE_CACHE_FILES",
        "DELETE_PACKAGES",
        "DELIVER_COMPANION_MESSAGES",
        "DIAGNOSTIC",
        "DISABLE_KEYGUARD",
        "DUMP",
        "EXPAND_STATUS_BAR",
        "FACTORY_TEST",
        "FOREGROUND_SERVICE",
        "GET_ACCOUNTS",
        "GET_ACCOUNTS_PRIVILEGED",
        "GET_PACKAGE_SIZE",
        "GET_TASKS",
        "GLOBAL_SEARCH",
        "HIDE_OVERLAY_WINDOWS",
        "HIGH_SAMPLING_RATE_SENSORS",
        "INSTALL_LOCATION_PROVIDER",
        "INSTALL_PACKAGES",
        "INSTALL_SHORTCUT",
        "INSTANT_APP_FOREGROUND_SERVICE",
        "INTERACT_ACROSS_PROFILES",
        "INTERNET",
        "KILL_BACKGROUND_PROCESSES",
        "LAUNCH_MULTI_PANE_SETTINGS_DEEP_LINK",
        "LOADER_USAGE_STATS",
        "LOCATION_HARDWARE",
        "MANAGE_DOCUMENTS",
        "MANAGE_EXTERNAL_STORAGE",
        "MANAGE_MEDIA",
        "MANAGE_ONGOING_CALLS",
        "MANAGE_OWN_CALLS",
        "MANAGE_WIFI_INTERFACES",
        "MANAGE_WIFI_NETWORK_SELECTION",
        "MASTER_CLEAR",
        "MEDIA_CONTENT_CONTROL",
        "MODIFY_AUDIO_SETTINGS",
        "MODIFY_PHONE_STATE",
        "MOUNT_FORMAT_FILESYSTEMS",
        "MOUNT_UNMOUNT_FILESYSTEMS",
        "NEARBY_WIFI_DEVICES",
        "NFC",
        "NFC_PREFERRED_PAYMENT_INFO",
        "NFC_TRANSACTION_EVENT",
        "OVERRIDE_WIFI_CONFIG",
        "PACKAGE_USAGE_STATS",
        "PERSISTENT_ACTIVITY",
        "POST_NOTIFICATIONS",
        "PROCESS_OUTGOING_CALLS",
        "QUERY_ALL_PACKAGES",
        "READ_ASSISTANT_APP_SEARCH_DATA",
        "READ_BASIC_PHONE_STATE",
        "READ_CALENDAR",
        "READ_CALL_LOG",
        "READ_CONTACTS",
        "READ_EXTERNAL_STORAGE",
        "READ_HOME_APP_SEARCH_DATA",
        "READ_INPUT_STATE",
        "READ_LOGS",
        "READ_MEDIA_AUDIO",
        "READ_MEDIA_IMAGES",
        "READ_MEDIA_VIDEO",
        "READ_NEARBY_STREAMING_POLICY",
        "READ_PHONE_NUMBERS",
        "READ_PHONE_STATE",
        "READ_PRECISE_PHONE_STATE",
        "READ_SMS",
        "READ_SYNC_SETTINGS",
        "READ_SYNC_STATS",
        "READ_VOICEMAIL",
        "REBOOT",
        "RECEIVE_BOOT_COMPLETED",
        "RECEIVE_MMS",
        "RECEIVE_SMS",
        "RECEIVE_WAP_PUSH",
        "RECORD_AUDIO",
        "REORDER_TASKS",
        "REQUEST_COMPANION_PROFILE_APP_STREAMING",
        "REQUEST_COMPANION_PROFILE_AUTOMOTIVE_PROJECTION",
        "REQUEST_COMPANION_PROFILE_COMPUTER",
        "REQUEST_COMPANION_PROFILE_WATCH",
        "REQUEST_COMPANION_RUN_IN_BACKGROUND",
        "REQUEST_COMPANION_SELF_MANAGED",
        "REQUEST_COMPANION_START_FOREGROUND_SERVICES_FROM_BACKGROUND",
        "REQUEST_COMPANION_USE_DATA_IN_BACKGROUND",
        "REQUEST_DELETE_PACKAGES",
        "REQUEST_IGNORE_BATTERY_OPTIMIZATIONS",
        "REQUEST_INSTALL_PACKAGES",
        "REQUEST_OBSERVE_COMPANION_DEVICE_PRESENCE",
        "REQUEST_PASSWORD_COMPLEXITY",
        "RESTART_PACKAGES",
        "SCHEDULE_EXACT_ALARM",
        "SEND_RESPOND_VIA_MESSAGE",
        "SEND_SMS",
        "SET_ALARM",
        "SET_ALWAYS_FINISH",
        "SET_ANIMATION_SCALE",
        "SET_DEBUG_APP",
        "SET_PREFERRED_APPLICATIONS",
        "SET_PROCESS_LIMIT",
        "SET_TIME",
        "SET_TIME_ZONE",
        "SET_WALLPAPER",
        "SET_WALLPAPER_HINTS",
        "SIGNAL_PERSISTENT_PROCESSES",
        "SMS_FINANCIAL_TRANSACTIONS",
        "START_FOREGROUND_SERVICES_FROM_BACKGROUND",
        "START_VIEW_APP_FEATURES",
        "START_VIEW_PERMISSION_USAGE",
        "STATUS_BAR",
        "SUBSCRIBE_TO_KEYGUARD_LOCKED_STATE",
        "SYSTEM_ALERT_WINDOW",
        "TRANSMIT_IR",
        "UNINSTALL_SHORTCUT",
        "UPDATE_DEVICE_STATS",
        "UPDATE_PACKAGES_WITHOUT_USER_ACTION",
        "USE_BIOMETRIC",
        "USE_EXACT_ALARM",
        "SCHEDULE_EXACT_ALARM",
        "USE_FINGERPRINT",
        "USE_BIOMETRIC",
        "USE_FULL_SCREEN_INTENT",
        "USE_ICC_AUTH_WITH_DEVICE_IDENTIFIER",
        "USE_SIP",
        "UWB_RANGING",
        "VIBRATE",
        "WAKE_LOCK",
        "WRITE_APN_SETTINGS",
        "WRITE_CALENDAR",
        "WRITE_CALL_LOG",
        "WRITE_CONTACTS",
        "WRITE_EXTERNAL_STORAGE",
        "WRITE_GSERVICES",
        "WRITE_SECURE_SETTINGS",
        "WRITE_SETTINGS",
        "WRITE_SYNC_SETTINGS",
        "WRITE_VOICEMAIL",
    };

    auto viewport = new QWidget(this);
    viewport->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    auto scroll = new QScrollArea(this);
    scroll->setWidget(viewport);
    scroll->setWidgetResizable(true);
    auto layout = new QVBoxLayout(this);
    layout->addWidget(scroll);

    auto layoutAdd = new QHBoxLayout;
    comboAdd = new QComboBox(this);
    comboAdd->setEditable(true);
    comboAdd->addItems(permissionStrings);
    btnAdd = new QPushButton(QIcon::fromTheme("list-add"), tr("Add"), this);
    btnAdd->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(btnAdd, &QPushButton::clicked, this, [=]() {
        const QString newPermission = QString("android.permission.%1").arg(comboAdd->currentText());
        if (!newPermission.isEmpty()) {
            const auto permissions = manifest->getPermissionList();
            for (const Permission &permission : permissions) {
                if (permission.getName() == newPermission) {
                    return;
                }
            }
            addPermissionLine(manifest->addPermission(newPermission));
        }
    });
    layoutAdd->addWidget(comboAdd);
    layoutAdd->addWidget(btnAdd);
    layout->addLayout(layoutAdd);

    buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);

    grid = new QGridLayout(viewport);
    const auto permissions = manifest->getPermissionList();
    for (const auto &permission : permissions) {
        addPermissionLine(permission);
    }
}

void PermissionEditor::addPermissionLine(const Permission &permission)
{
    const int row = grid->rowCount();

    auto permissionName = permission.getName();
    auto labelTitle = new QLabel(permissionName, this);
    grid->addWidget(labelTitle, row, 0);

    auto btnHelp = new QToolButton(this);
    btnHelp->setToolTip(tr("Documentation"));
    btnHelp->setIcon(QIcon::fromTheme("help-about"));
    connect(btnHelp, &QToolButton::clicked, [=]() {
        const QString url("https://developer.android.com/reference/android/Manifest.permission.html#%1");
        QDesktopServices::openUrl(url.arg(permissionName.mid(QString("android.permission.").length())));
    });
    grid->addWidget(btnHelp, row, 1);
    if (!permissionName.startsWith("android.permission.")) {
        btnHelp->setEnabled(false);
    }

    auto btnRemove = new QToolButton(this);
    btnRemove->setToolTip(tr("Remove"));
    btnRemove->setIcon(QIcon::fromTheme("list-remove"));
    connect(btnRemove, &QToolButton::clicked, this, [=]() {
        //: %1 will be replaced with a programmatic Android permission name (e.g., "android.permission.SEND_SMS", "android.permission.CAMERA", etc.).
        if (YesAlwaysDialog::ask("RemovePermission", tr("Are you sure you want to remove the %1 permission?").arg(permissionName), this)) {
            delete labelTitle;
            delete btnHelp;
            delete btnRemove;
            manifest->removePermission(permission);
        }
    });
    grid->addWidget(btnRemove, row, 2);

    setTabOrder(btnRemove, comboAdd);
    setTabOrder(comboAdd, btnAdd);
    setTabOrder(btnAdd, buttons);
}
