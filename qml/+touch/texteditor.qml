
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.0

import io.qt.examples.texteditor 1.0

// TODO:
// - make designer-friendly

ApplicationWindow {
    id: window
    visible: true
    title: document.fileName + " - Text Editor Example"

    header: ToolBar {
        leftPadding: 5

        RowLayout {
            anchors.fill: parent
            spacing: 0

            ToolButton {
                id: doneEditingButton
                font.family: "fontello"
                text: "\uE80A" // icon-ok
                opacity: !textArea.readOnly ? 1 : 0
                onClicked: textArea.readOnly = true

                Material.foreground: Material.accent
            }

            Label {
                text: qsTr("Text Editor Example")
                font.bold: true
                font.pixelSize: 20
                elide: Label.ElideRight
                Layout.fillWidth: true
            }

            ToolButton {
                font.family: "fontello"
                text: "\uF142" // icon-ellipsis-vert
                onClicked: menu.open()

                Menu {
                    id: menu

                    MenuItem {
                        text: qsTr("About")
                        onTriggered: aboutDialog.open()
                    }
                }
            }
        }
    }

    DocumentHandler {
        id: document
        document: textArea.textDocument
        cursorPosition: textArea.cursorPosition
        selectionStart: textArea.selectionStart
        selectionEnd: textArea.selectionEnd
        // textColor: TODO
        Component.onCompleted: document.load("qrc:/texteditor.html")
        onLoaded: {
            textArea.text = text
        }
        onError: {
            errorDialog.text = message
            errorDialog.visible = true
        }
    }

    Flickable {
        id: flickable
        flickableDirection: Flickable.VerticalFlick
        anchors.fill: parent

        TextArea.flickable: TextArea {
            id: textArea
            textFormat: Qt.RichText
            wrapMode: TextArea.Wrap
            readOnly: true
            persistentSelection: true
            // Different styles have different padding and background
            // decorations, but since this editor is almost taking up the
            // entire window, we don't need them.
            leftPadding: 6
            rightPadding: 6
            topPadding: 0
            bottomPadding: 0
            background: null

            onLinkActivated: Qt.openUrlExternally(link)
        }

        ScrollBar.vertical: ScrollBar {}
    }

    footer: ToolBar {
        visible: !textArea.readOnly && textArea.activeFocus

        Material.primary: "#E0E0E0"
        Material.elevation: 0

        Flickable {
            anchors.fill: parent
            contentWidth: toolRow.implicitWidth
            flickableDirection: Qt.Horizontal
            boundsBehavior: Flickable.StopAtBounds

            Row {
                id: toolRow

                ToolButton {
                    id: boldButton
                    text: "\uE800" // icon-bold
                    font.family: "fontello"
                    // Don't want to close the virtual keyboard when this is clicked.
                    focusPolicy: Qt.NoFocus
                    checkable: true
                    checked: document.bold
                    onClicked: document.bold = !document.bold
                }
                ToolButton {
                    id: italicButton
                    text: "\uE801" // icon-italic
                    font.family: "fontello"
                    focusPolicy: Qt.NoFocus
                    checkable: true
                    checked: document.italic
                    onClicked: document.italic = !document.italic
                }
                ToolButton {
                    id: underlineButton
                    text: "\uF0CD" // icon-underline
                    font.family: "fontello"
                    focusPolicy: Qt.NoFocus
                    checkable: true
                    checked: document.underline
                    onClicked: document.underline = !document.underline
                }

                ToolSeparator {}

                ToolButton {
                    id: alignLeftButton
                    text: "\uE803" // icon-align-left
                    font.family: "fontello"
                    focusPolicy: Qt.NoFocus
                    checkable: true
                    checked: document.alignment == Qt.AlignLeft
                    onClicked: document.alignment = Qt.AlignLeft
                }
                ToolButton {
                    id: alignCenterButton
                    text: "\uE804" // icon-align-center
                    font.family: "fontello"
                    focusPolicy: Qt.NoFocus
                    checkable: true
                    checked: document.alignment == Qt.AlignHCenter
                    onClicked: document.alignment = Qt.AlignHCenter
                }
                ToolButton {
                    id: alignRightButton
                    text: "\uE805" // icon-align-right
                    font.family: "fontello"
                    focusPolicy: Qt.NoFocus
                    checkable: true
                    checked: document.alignment == Qt.AlignRight
                    onClicked: document.alignment = Qt.AlignRight
                }
                ToolButton {
                    id: alignJustifyButton
                    text: "\uE806" // icon-align-justify
                    font.family: "fontello"
                    focusPolicy: Qt.NoFocus
                    checkable: true
                    checked: document.alignment == Qt.AlignJustify
                    onClicked: document.alignment = Qt.AlignJustify
                }
            }
        }
    }

    RoundButton {
        id: editButton
        font.family: "fontello"
        text: "\uE809" // icon-pencil
        width: 48
        height: width
        // Don't want to use anchors for the y position, because it will anchor
        // to the footer, leaving a large vertical gap.
        y: parent.height - height - 12
        anchors.right: parent.right
        anchors.margins: 12
        visible: textArea.readOnly
        highlighted: true

        onClicked: {
            textArea.readOnly = false
            // Force focus on the text area so the cursor and footer show up.
            textArea.forceActiveFocus()
        }
    }

    Dialog {
        id: aboutDialog
        standardButtons: Dialog.Ok
        modal: true
        x: parent.width / 2 - width / 2
        y: parent.height / 2 - height / 2

        contentItem: Label {
            text: qsTr("Qt Quick Controls 2 - Text Editor Example")
        }
    }
}
