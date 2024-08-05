#ifndef DOCUMENTHANDLER_H
#define DOCUMENTHANDLER_H

#include <QtDBus/QtDBus>
#include <QFont>
#include <QObject>
#include <QTextCursor>
#include <QUrl>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusMetaType>
//#include "chat_adaptor.h"
//#include "chat_interface.h"

QT_BEGIN_NAMESPACE
class QTextDocument;
class QQuickTextDocument;
QT_END_NAMESPACE

class DocumentHandler : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QQuickTextDocument *document READ document WRITE setDocument NOTIFY documentChanged)
    Q_PROPERTY(int cursorPosition READ cursorPosition WRITE setCursorPosition NOTIFY cursorPositionChanged)
    Q_PROPERTY(int selectionStart READ selectionStart WRITE setSelectionStart NOTIFY selectionStartChanged)
    Q_PROPERTY(int selectionEnd READ selectionEnd WRITE setSelectionEnd NOTIFY selectionEndChanged)

    Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor NOTIFY textColorChanged)
    Q_PROPERTY(QString fontFamily READ fontFamily WRITE setFontFamily NOTIFY fontFamilyChanged)
    Q_PROPERTY(Qt::Alignment alignment READ alignment WRITE setAlignment NOTIFY alignmentChanged)

    Q_PROPERTY(bool bold READ bold WRITE setBold NOTIFY boldChanged)
    Q_PROPERTY(bool italic READ italic WRITE setItalic NOTIFY italicChanged)
    Q_PROPERTY(bool underline READ underline WRITE setUnderline NOTIFY underlineChanged)

    Q_PROPERTY(int fontSize READ fontSize WRITE setFontSize NOTIFY fontSizeChanged)

    Q_PROPERTY(QString fileName READ fileName NOTIFY fileUrlChanged)
    Q_PROPERTY(QString fileType READ fileType NOTIFY fileUrlChanged)
    Q_PROPERTY(QUrl fileUrl READ fileUrl NOTIFY fileUrlChanged)

    Q_PROPERTY(QString myText READ myText WRITE setmyText NOTIFY myTextChanged)
    Q_PROPERTY(QString sessionName READ sessionName NOTIFY sessionNameChanged)

public:
    explicit DocumentHandler(QObject *parent = nullptr);
    //explicit DocumentHandler(QObject *parent = nullptr,bool detached = false, QString session = "");

    QQuickTextDocument *document() const;

    int cursorPosition() const;
    void setCursorPosition(int position);

    int selectionStart() const;
    void setSelectionStart(int position);

    int selectionEnd() const;
    void setSelectionEnd(int position);

    QString fontFamily() const;
    void setFontFamily(const QString &family);

    QColor textColor() const;
    void setTextColor(const QColor &color);

    Qt::Alignment alignment() const;
    void setAlignment(Qt::Alignment alignment);

    bool bold() const;
    void setBold(bool bold);

    bool italic() const;
    void setItalic(bool italic);

    bool underline() const;
    void setUnderline(bool underline);

    int fontSize() const;
    void setFontSize(int size);

    QString fileName() const;
    QString fileType() const;
    QUrl fileUrl() const;


    Q_INVOKABLE QString myText() const;
    QString sessionName() const;

public slots:
    void load(const QUrl &fileUrl);
    void saveAs(const QUrl &fileUrl);
    void setDocument(QQuickTextDocument *document);
    void onMyTextChanged(const QString &text);
    void setmyText(const QString &mytext);
    void undo();
    void redo();
    void onUndo();
    void onRedo();


signals:
    void documentChanged();
    void cursorPositionChanged();
    void selectionStartChanged();
    void selectionEndChanged();

    void fontFamilyChanged();
    void textColorChanged();
    void alignmentChanged();

    void boldChanged();
    void italicChanged();
    void underlineChanged();

    void fontSizeChanged();

    void textChanged();
    void fileUrlChanged();

    void loaded(const QString &text);
    void error(const QString &message);
    void myTextChanged(const QString &text);
    void sessionNameChanged();

private:
    void reset();
    QTextCursor textCursor() const;
    QTextDocument *textDocument() const;
    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    QQuickTextDocument *m_document;
    int m_cursorPosition;
    int m_selectionStart;
    int m_selectionEnd;
    QFont m_font;
    int m_fontSize;
    QUrl m_fileUrl;

    QString m_Text;
    QString m_sessionName;
    void startDBus(const QString &session);
};

#endif // DOCUMENTHANDLER_H
