// Format was used from QT Example on QT Website
// http://doc.qt.io/qt-5/qtquick-layouts-layouts-qml.html

// By: Milton Young

//--------------------------------------Imports----------------------------------------------//
import QtQuick 2.6
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.3
//------------------------------------End of Imports-----------------------------------------//


Window {
    id: mainWindow
    visible: true
    width: 640
    height: 650
    color: "lightgray"
    title: qsTr("CSE434 Messenger")

    property int margin: 11
        //width: mainLayout.implicitWidth + 2 * margin
        //height: mainLayout.implicitHeight + 2 * margin
        minimumWidth: mainLayout.Layout.minimumWidth + 2 * margin
        minimumHeight: mainLayout.Layout.minimumHeight + 2 * margin


    ColumnLayout {
        id: mainLayout
        anchors.fill: parent
        anchors.margins: margin


        GroupBox {
            id: configClientBox
            title: "Configure Client"
            Layout.fillWidth: true

            GridLayout {
                id: gridLayout
                rows: 5
                flow: GridLayout.TopToBottom
                anchors.fill: parent

                Label { id: configErrorLabel; color: "red"; Layout.columnSpan: 3}
                Label { text: "Client IP Address" }
                Label { text: "Client Port Number" }
                Label { text: "Server IP Address" }
                Label { text: "Server Port Number" }

                TextField { id: clientIPTF; text: "127.0.0.1"}
                TextField { id: clientPortTF; text: "31000"}
                TextField { id: serverIPTF; text: "127.0.0.1" }
                TextField { id: serverPortNumberTF; text: "32000" }

                Button {
                    id: configClientButton
                    text: "Configure Client"
                    onClicked:{
                        if(clientIPTF.text == "" || clientPortTF.text == "" || serverIPTF.text == "" || serverPortNumberTF.text == "")
                        {
                            configErrorLabel.text = "Please fill in all fields";
                        }
                        else
                        {
                            myObject.initializeSocketConnection(clientIPTF.text, clientPortTF.text,serverIPTF.text, serverPortNumberTF.text );
                            configErrorLabel.text = "";
                        }
                    }
                }
            }
        }

        GroupBox {
            id: loginBox
            title: "Login to Server"
            Layout.fillWidth: true

            RowLayout {
                id: rowLayout
                anchors.fill: parent
                Label { text: "Username" }
                TextField {
                    id: usernameTF
                    placeholderText: "johnnyBoy1234"
                    Layout.fillWidth: true
                }
                Label { text: "Password" }
                TextField {
                    id: passwordTF
                    placeholderText: "wingardiumleviosa"
                    Layout.fillWidth: true
                    echoMode: TextInput.Password
                }
                Button {
                    id: loginButton
                    text: "Log In"
                    onClicked:{
                        if(usernameTF.text == "" || passwordTF.text == "")
                        {
                            configErrorLabel.text = "Please fill in all fields";
                        }
                        else
                        {
                            myObject.logIn(usernameTF.text, passwordTF.text);
                            configErrorLabel.text = "";
                            passwordTF.text = "";
                        }
                    }

                }
            }
        }
        GroupBox {
            id: sendMessageBox
            title: "Send a Message"
            Layout.fillWidth: true

            RowLayout {
                id: sendMessageRowLayout
                anchors.fill: parent
                Label { text: "Send To" }
                TextField {
                    id: destClientTF
                    placeholderText: "harrypotter23"
                    Layout.fillWidth: true
                }
                Label { text: "Message" }
                TextField {
                    id: messageTF
                    placeholderText: "He who shall not be named..."
                    Layout.fillWidth: true
                }
                Button {
                    id: sendMessageButton
                    text: "Send Message"
                    onClicked:{
                        if(destClientTF.text == "" || messageTF.text == "")
                        {
                            configErrorLabel.text = "Please fill in all fields";
                        }
                        else
                        {
                            myObject.sendMessage(destClientTF.text, messageTF.text);
                            configErrorLabel.text = "";
                            messageTF.text = "";
                        }
                    }
                }
            }
        }
        GroupBox {
            id: logoutBox
            title: "Logout"
            Layout.fillWidth: true

            RowLayout {
                id: logoutRowLayout
                anchors.fill: parent
                Label { text: "Username" }
                TextField {
                    id: logoutClientTF
                    placeholderText: "harrypotter23"
                    Layout.fillWidth: true
                }
                Button {
                    id: logoutButton
                    text: "Logout"
                    onClicked:{
                        if(logoutClientTF.text == "")
                        {
                            configErrorLabel.text = "Please fill in all fields";
                        }
                        else
                        {
                            myObject.logOut(logoutClientTF.text);
                            configErrorLabel.text = "";
                            logoutClientTF.text = "";
                        }
                    }
                }
            }
        }

        TextArea {
            id: responseTA
            text: "Messages:"
            Layout.minimumHeight: 30
            Layout.fillHeight: true
            Layout.fillWidth: true

            Connections{
                target: myObject
                onNewResponseReceived:{
                    responseTA.append(response);
                }
            }
        }

    }
}

