#include "windows/permissioneditor.h"
#include "base/application.h"
#include <QScrollArea>
#include <QToolButton>
#include <QDesktopServices>

#ifdef QT_DEBUG
    #include <QDebug>
#endif

PermissionEditor::PermissionEditor(Manifest *manifest, QWidget *parent) : QDialog(parent), manifest(manifest)
{
    //: This string refers to multiple permissions (as in "Editor of permissions").
    setWindowTitle(tr("Permission Editor"));
    setWindowIcon(app->icons.get("permissions.png"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    resize(app->scale(550, 400));

    const QStringList permissionStrings = {
        "ACCEPT_HANDOVER",
        "ACCESS_BACKGROUND_LOCATION",
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
        "BIND_CONDITION_PROVIDER_SERVICE",
        "BIND_DEVICE_ADMIN",
        "BIND_DREAM_SERVICE",
        "BIND_INCALL_SERVICE",
        "BIND_INPUT_METHOD",
        "BIND_MIDI_DEVICE_SERVICE",
        "BIND_NFC_SERVICE",
        "BIND_NOTIFICATION_LISTENER_SERVICE",
        "BIND_PRINT_SERVICE",
        "BIND_QUICK_SETTINGS_TILE",
        "BIND_REMOTEVIEWS",
        "BIND_SCREENING_SERVICE",
        "BIND_TELECOM_CONNECTION_SERVICE",
        "BIND_TEXT_SERVICE",
        "BIND_TV_INPUT",
        "BIND_VISUAL_VOICEMAIL_SERVICE",
        "BIND_VOICE_INTERACTION",
        "BIND_VPN_SERVICE",
        "BIND_VR_LISTENER_SERVICE",
        "BIND_WALLPAPER",
        "BLUETOOTH",
        "BLUETOOTH_ADMIN",
        "BLUETOOTH_PRIVILEGED",
        "BODY_SENSORS",
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
        "INSTALL_LOCATION_PROVIDER",
        "INSTALL_PACKAGES",
        "INSTALL_SHORTCUT",
        "INSTANT_APP_FOREGROUND_SERVICE",
        "INTERNET",
        "KILL_BACKGROUND_PROCESSES",
        "LOCATION_HARDWARE",
        "MANAGE_DOCUMENTS",
        "MANAGE_OWN_CALLS",
        "MASTER_CLEAR",
        "MEDIA_CONTENT_CONTROL",
        "MODIFY_AUDIO_SETTINGS",
        "MODIFY_PHONE_STATE",
        "MOUNT_FORMAT_FILESYSTEMS",
        "MOUNT_UNMOUNT_FILESYSTEMS",
        "NFC",
        "NFC_TRANSACTION_EVENT",
        "PACKAGE_USAGE_STATS",
        "PERSISTENT_ACTIVITY",
        "PROCESS_OUTGOING_CALLS",
        "READ_CALENDAR",
        "READ_CALL_LOG",
        "READ_CONTACTS",
        "READ_EXTERNAL_STORAGE",
        "READ_INPUT_STATE",
        "READ_LOGS",
        "READ_PHONE_NUMBERS",
        "READ_PHONE_STATE",
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
        "REQUEST_COMPANION_RUN_IN_BACKGROUND",
        "REQUEST_COMPANION_USE_DATA_IN_BACKGROUND",
        "REQUEST_DELETE_PACKAGES",
        "REQUEST_IGNORE_BATTERY_OPTIMIZATIONS",
        "REQUEST_INSTALL_PACKAGES",
        "REQUEST_PASSWORD_COMPLEXITY",
        "RESTART_PACKAGES",
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
        "START_VIEW_PERMISSION_USAGE",
        "STATUS_BAR",
        "SYSTEM_ALERT_WINDOW",
        "TRANSMIT_IR",
        "UNINSTALL_SHORTCUT",
        "UPDATE_DEVICE_STATS",
        "USE_BIOMETRIC",
        "USE_FINGERPRINT",
        "USE_FULL_SCREEN_INTENT",
        "USE_SIP",
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
    btnAdd = new QPushButton(app->icons.get("add.png"), tr("Add"), this);
    btnAdd->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(btnAdd, &QPushButton::clicked, [=]() {
        const QString permission = QString("android.permission.%1").arg(comboAdd->currentText());
        if (!permission.isEmpty()) {
            addPermissionLine(manifest->addPermission(permission));
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
    auto permissions = manifest->getPermissionList();
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
    btnHelp->setIcon(app->icons.get("help.png"));
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
    btnRemove->setIcon(app->icons.get("remove.png"));
    connect(btnRemove, &QToolButton::clicked, [=]() {
        delete labelTitle;
        delete btnHelp;
        delete btnRemove;
        manifest->removePermission(permission);
    });
    grid->addWidget(btnRemove, row, 2);
}
