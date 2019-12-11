
#include <QApplication>
#include <QCommandLineParser>
#include <QStandardPaths>

#include <OpenLibrary/utils_qt/configuration.hpp>
#include <OpenLibrary/utils_qt/configuration_json_storage.hpp>

#include "main_window.hpp"
#include "user_keys_manager.hpp"
#include "server.hpp"
#include "encryption/encrypted_client.hpp"
#include "connection_manager.hpp"


int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    const QString defaultConfigDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QCommandLineOption configDirOption({"c", "config"},
                                       QString("Configuration directory to be used. Defaults to %1").arg(defaultConfigDir),
                                       "directory",
                                       defaultConfigDir);
    QCommandLineParser cmdLineParser;
    cmdLineParser.addOption(configDirOption);
    cmdLineParser.addHelpOption();

    cmdLineParser.process(app);

    const QString configDir = cmdLineParser.value(configDirOption);

    ConfigJsonStorage configStorage(configDir + "/config.json");
    Configuration configuration(configStorage);
    configuration.setDefaultValue("port", 1234);

    UserKeysManager manager(configDir + "/user_keys");

    /// @todo handle this one properly (like as user what to do when one file is missing)
    if (manager.privateKeyExists() == false || manager.publicKeyExists() == false)
        manager.generateKeysPair();

    /// @todo validate keys

    ConnectionManager connectionManager;

    Server server(&manager, connectionManager, configuration.getEntry("port").toInt());
    server.start();

    // temporary debug code
    EncryptedClient client(&manager, connectionManager);
    if (configuration.getEntry("port").toInt() != 1234)
        client.makeConnection("localhost", 1234);

    MainWindow main_window;
    main_window.show();

    return app.exec();
}
