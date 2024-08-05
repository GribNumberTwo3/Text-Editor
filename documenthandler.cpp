#include "documenthandler.h"
#include <QtDBus/QtDBus>
#include <QFile>
#include <QFileInfo>
#include <QFileSelector>
#include <QQmlFile>
#include <QQmlFileSelector>
#include <QQuickTextDocument>
#include <QTextCharFormat>
#include <QTextCodec>
#include <QTextDocument>
#include <QDebug>

DocumentHandler::DocumentHandler(QObject *parent)
    : QObject(parent)
    , m_document(nullptr)
    , m_cursorPosition(-1)
    , m_selectionStart(0)
    , m_selectionEnd(0)
    , m_sessionName("allusers")
{
    QStringList arguments = QCoreApplication::arguments();

    if (arguments.size() == 2 && arguments.at(1) == "--detached") {
        m_sessionName = "detached";
    } else if (arguments.size() > 2 && arguments.at(1) == "--channel") {
        m_sessionName = arguments.at(2);
    }

    startDBus(m_sessionName);
}

void DocumentHandler::startDBus(const QString &session)
{
    if (session != "detached"){
        QDBusConnection connection = QDBusConnection::sessionBus();
        if (!connection.isConnected()) {
            qWarning() << "Failed to connect to D-Bus session bus.";
            //handle or send error
            return;
        }

        if (connection.registerService("org.example." + session))
            connection.registerObject("/", this, QDBusConnection::ExportAllInvokables | QDBusConnection::ExportAllSlots);

        connection.connect(QString(), QString(), "org.example."+session, "myTextChanged", this, SLOT(onMyTextChanged(QString)));
        connection.connect(QString(), QString(), "org.example." + session, "undoRequested", this, SLOT(onUndo()));
        connection.connect(QString(), QString(), "org.example." + session, "redoRequested", this, SLOT(onRedo()));

        // загружаем текст из DBus
        QDBusInterface iface("org.example." + session, "/", "", QDBusConnection::sessionBus());
        if (iface.isValid()) {
            QDBusReply<QString> reply = iface.call("myText");
            if (reply.isValid()) {
                setmyText(reply.value());
            } else {
                qWarning() << "Failed to get current text from DBus session.";
            }
        } else {
            qWarning() << "Failed to create DBus interface.";
        }
    }
}

QQuickTextDocument *DocumentHandler::document() const
{
    return m_document;
}

QString DocumentHandler::sessionName() const
{
    return m_sessionName;
}

QString DocumentHandler::myText() const
{
    return m_Text;
}

void DocumentHandler::setmyText(const QString &mytext){

    if (mytext == m_Text) return;

    m_Text = mytext;
    emit myTextChanged(m_Text);
    if (m_sessionName != "detached"){
        QDBusMessage msg = QDBusMessage::createSignal("/", "org.example."+m_sessionName, "myTextChanged");
        msg << m_Text;
        QDBusConnection::sessionBus().send(msg);
    }
}

void DocumentHandler::onMyTextChanged(const QString &mytext){

    if (mytext == m_Text) return;

    m_Text = mytext;
    emit myTextChanged(m_Text);
}

void DocumentHandler::undo()
{
    QDBusMessage msg = QDBusMessage::createSignal("/", "org.example." + m_sessionName, "undoRequested");
    QDBusConnection::sessionBus().send(msg);
}

void DocumentHandler::redo()
{
    QDBusMessage msg = QDBusMessage::createSignal("/", "org.example." + m_sessionName, "redoRequested");
    QDBusConnection::sessionBus().send(msg);
}

void DocumentHandler::onUndo()
{
    if (m_document) {
        m_document->textDocument()->undo();
        emit myTextChanged(m_document->textDocument()->toPlainText());
    }
}

void DocumentHandler::onRedo()
{
    if (m_document) {
        m_document->textDocument()->redo();
        emit myTextChanged(m_document->textDocument()->toPlainText());
    }
}

void DocumentHandler::setDocument(QQuickTextDocument *document)
{
    //if (document == m_document)
    //    return;

    m_document = document;
    emit documentChanged();
}

int DocumentHandler::cursorPosition() const
{
    return m_cursorPosition;
}

void DocumentHandler::setCursorPosition(int position)
{
    if (position == m_cursorPosition)
        return;

    m_cursorPosition = position;
    reset();
    emit cursorPositionChanged();
}

int DocumentHandler::selectionStart() const
{
    return m_selectionStart;
}

void DocumentHandler::setSelectionStart(int position)
{
    if (position == m_selectionStart)
        return;

    m_selectionStart = position;
    emit selectionStartChanged();
}

int DocumentHandler::selectionEnd() const
{
    return m_selectionEnd;
}

void DocumentHandler::setSelectionEnd(int position)
{
    if (position == m_selectionEnd)
        return;

    m_selectionEnd = position;
    emit selectionEndChanged();
}

QString DocumentHandler::fontFamily() const
{
    QTextCursor cursor = textCursor();
    if (cursor.isNull())
        return QString();
    QTextCharFormat format = cursor.charFormat();
    return format.font().family();
}

void DocumentHandler::setFontFamily(const QString &family)
{
    QTextCharFormat format;
    format.setFontFamily(family);
    mergeFormatOnWordOrSelection(format);
    emit fontFamilyChanged();
}

QColor DocumentHandler::textColor() const
{
    QTextCursor cursor = textCursor();
    if (cursor.isNull())
        return QColor(Qt::black);
    QTextCharFormat format = cursor.charFormat();
    return format.foreground().color();
}

void DocumentHandler::setTextColor(const QColor &color)
{
    QTextCharFormat format;
    format.setForeground(QBrush(color));
    mergeFormatOnWordOrSelection(format);
    emit textColorChanged();
}

Qt::Alignment DocumentHandler::alignment() const
{
    QTextCursor cursor = textCursor();
    if (cursor.isNull())
        return Qt::AlignLeft;
    return textCursor().blockFormat().alignment();
}

void DocumentHandler::setAlignment(Qt::Alignment alignment)
{
    QTextBlockFormat format;
    format.setAlignment(alignment);
    QTextCursor cursor = textCursor();
    cursor.mergeBlockFormat(format);
    emit alignmentChanged();
}

bool DocumentHandler::bold() const
{
    QTextCursor cursor = textCursor();
    if (cursor.isNull())
        return false;
    return textCursor().charFormat().fontWeight() == QFont::Bold;
}

void DocumentHandler::setBold(bool bold)
{
    QTextCharFormat format;
    format.setFontWeight(bold ? QFont::Bold : QFont::Normal);
    mergeFormatOnWordOrSelection(format);
    emit boldChanged();
}

bool DocumentHandler::italic() const
{
    QTextCursor cursor = textCursor();
    if (cursor.isNull())
        return false;
    return textCursor().charFormat().fontItalic();
}

void DocumentHandler::setItalic(bool italic)
{
    QTextCharFormat format;
    format.setFontItalic(italic);
    mergeFormatOnWordOrSelection(format);
    emit italicChanged();
}

bool DocumentHandler::underline() const
{
    QTextCursor cursor = textCursor();
    if (cursor.isNull())
        return false;
    return textCursor().charFormat().fontUnderline();
}

void DocumentHandler::setUnderline(bool underline)
{
    QTextCharFormat format;
    format.setFontUnderline(underline);
    mergeFormatOnWordOrSelection(format);
    emit underlineChanged();
}

int DocumentHandler::fontSize() const
{
    QTextCursor cursor = textCursor();
    if (cursor.isNull())
        return 0;
    QTextCharFormat format = cursor.charFormat();
    return format.font().pointSize();
}

void DocumentHandler::setFontSize(int size)
{
    if (size <= 0)
        return;

    QTextCursor cursor = textCursor();
    if (cursor.isNull())
        return;

    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);

    if (cursor.charFormat().property(QTextFormat::FontPointSize).toInt() == size)
        return;

    QTextCharFormat format;
    format.setFontPointSize(size);
    mergeFormatOnWordOrSelection(format);
    emit fontSizeChanged();
}

QString DocumentHandler::fileName() const
{
    const QString filePath = QQmlFile::urlToLocalFileOrQrc(m_fileUrl);
    const QString fileName = QFileInfo(filePath).fileName();
    if (fileName.isEmpty())
        return QStringLiteral("untitled.txt");
    return fileName;
}

QString DocumentHandler::fileType() const
{
    return QFileInfo(fileName()).suffix();
}

QUrl DocumentHandler::fileUrl() const
{
    return m_fileUrl;
}

void DocumentHandler::load(const QUrl &fileUrl)
{
    if (fileUrl == m_fileUrl)
        return;

    QQmlEngine *engine = qmlEngine(this);
    if (!engine) {
        qWarning() << "load() called before DocumentHandler has QQmlEngine";
        return;
    }

    const QUrl path = QQmlFileSelector::get(engine)->selector()->select(fileUrl);
    const QString fileName = QQmlFile::urlToLocalFileOrQrc(path);
    if (QFile::exists(fileName)) {
        QFile file(fileName);
        if (file.open(QFile::ReadOnly)) {
            QByteArray data = file.readAll();
            QTextCodec *codec = QTextCodec::codecForHtml(data);
            if (QTextDocument *doc = textDocument())
                doc->setModified(false);

            emit loaded(codec->toUnicode(data));
            reset();
        }
    }

    m_fileUrl = fileUrl;
    emit fileUrlChanged();
}

void DocumentHandler::saveAs(const QUrl &fileUrl)
{
    QTextDocument *doc = textDocument();
    if (!doc)
        return;

    const QString filePath = fileUrl.toLocalFile();
    const bool isHtml = QFileInfo(filePath).suffix().contains(QLatin1String("htm"));
    QFile file(filePath);
    if (!file.open(QFile::WriteOnly | QFile::Truncate | (isHtml ? QFile::NotOpen : QFile::Text))) {
        emit error(tr("Cannot save: ") + file.errorString());
        return;
    }
    file.write((isHtml ? doc->toHtml() : doc->toPlainText()).toUtf8());
    file.close();

    if (fileUrl == m_fileUrl)
        return;

    m_fileUrl = fileUrl;
    emit fileUrlChanged();
}

void DocumentHandler::reset()
{
    emit fontFamilyChanged();
    emit alignmentChanged();
    emit boldChanged();
    emit italicChanged();
    emit underlineChanged();
    emit fontSizeChanged();
    emit textColorChanged();
}

QTextCursor DocumentHandler::textCursor() const
{
    QTextDocument *doc = textDocument();
    if (!doc)
        return QTextCursor();

    QTextCursor cursor = QTextCursor(doc);
    if (m_selectionStart != m_selectionEnd) {
        cursor.setPosition(m_selectionStart);
        cursor.setPosition(m_selectionEnd, QTextCursor::KeepAnchor);
    } else {
        cursor.setPosition(m_cursorPosition);
    }
    return cursor;
}

QTextDocument *DocumentHandler::textDocument() const
{
    if (!m_document)
        return nullptr;

    return m_document->textDocument();
}

void DocumentHandler::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
}
