#ifdef QT_WIDGETS_LIB
#include <QApplication>
#else
#include <QGuiApplication>
#endif
#include <QFontDatabase>
#include <QDebug>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlFileSelector>
#include <QQuickStyle>
#include <QMessageBox>
#include "documenthandler.h"
#include <QtDBus/QtDBus>

int main(int argc, char *argv[])
{
    QGuiApplication::setApplicationName("Text Editor");
    QGuiApplication::setOrganizationName("QtProject");
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

#ifdef QT_WIDGETS_LIB
    QApplication app(argc, argv);
#else
    QGuiApplication app(argc, argv);
#endif

    QFontDatabase fontDatabase;
    if (fontDatabase.addApplicationFont(":/fonts/fontello.ttf") == -1)
        qWarning() << "Failed to load fontello.ttf";

    qmlRegisterType<DocumentHandler>("io.qt.examples.texteditor", 1, 0, "DocumentHandler");

    QStringList selectors;
#ifdef QT_EXTRA_FILE_SELECTOR
    selectors += QT_EXTRA_FILE_SELECTOR;
#else
    if (app.arguments().contains("-touch"))
        selectors += "touch";
#endif

    QQmlApplicationEngine engine;
    QQmlFileSelector::get(&engine)->setExtraSelectors(selectors);

    engine.load(QUrl("qrc:/qml/texteditor.qml"));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
