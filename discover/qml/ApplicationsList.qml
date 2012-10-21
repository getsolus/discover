import QtQuick 1.1
import org.kde.plasma.components 0.1
import org.kde.qtextracomponents 0.1
import "navigation.js" as Navigation

Item {
    id: parentItem
    property alias count: view.count
    property alias header: view.header
    property alias section: view.section
    property bool preferUpgrade: false
    property alias model: view.model
    property real actualWidth: width
    property real proposedMargin: (view.width-actualWidth)/2

    ListView
    {
        id: view
        anchors {
            top: parent.top
            left: parent.left
            bottom: parent.bottom
            right: scroll.left
        }
        spacing: 3
        snapMode: ListView.SnapToItem
        currentIndex: -1
        
        delegate: ListItem {
                checked: view.currentIndex==index
                width: parentItem.actualWidth
                x: parentItem.proposedMargin
                property real contHeight: height*0.8
                height: nameLabel.font.pixelSize*3
                MouseArea {
                    id: delegateArea
                    anchors.fill: parent
                    onClicked: {
                        view.currentIndex = index
                        Navigation.openApplication(application)
                    }
                    hoverEnabled: true
                    QIconItem {
                        id: icon
                        icon: model.icon; width: contHeight; height: contHeight
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                    }
                    
                    QIconItem {
                        anchors.right: icon.right
                        anchors.bottom: icon.bottom
                        visible: isInstalled && view.model.stateFilter!=2
                        icon: "dialog-ok"
                        height: 16
                        width: 16
                    }
                    Label {
                        id: nameLabel
                        anchors.top: icon.top
                        anchors.left: icon.right
                        anchors.right: ratingsItem.left
                        anchors.leftMargin: 5
                        font.pointSize: commentLabel.font.pointSize*1.7
                        elide: Text.ElideRight
                        text: name
                    }
                    Label {
                        id: commentLabel
                        anchors {
                            bottom: icon.bottom
                            left: icon.right
                            leftMargin: 5
                            right: actionsRow.left
                        }
                        elide: Text.ElideRight
                        text: comment
                        font.italic: true
                        opacity: delegateArea.containsMouse ? 1 : 0.2
                    }
                    Rating {
                        id: ratingsItem
                        anchors {
                            right: parent.right
                            top: parent.top
                        }
                        height: contHeight*.4
                        rating: model.rating
                    }
                    
                    Row {
                        id: actionsRow
                        height: contHeight*0.5
                        spacing: 5
                        anchors {
                            bottom: parent.bottom
                            right: parent.right
                        }
                            
                        Button {
                            text: i18n("Update")
                            id: upgradeButton
                            width: ratingsItem.width
                            visible: model.application.canUpgrade
                            onClicked: resourcesModel.installApplication(model.application)
                        }
                        
                        InstallApplicationButton {
                            id: installButton
                            width: ratingsItem.width
                            height: upgradeButton.height
    //                         property bool isVisible: delegateArea.containsMouse && !installButton.canHide
    //                         opacity: isVisible ? 1 : 0
                            application: model.application
                        }
                    }
                }
            }
    }
    
    ScrollBar {
        id: scroll
        orientation: Qt.Vertical
        flickableItem: view
        anchors {
                top: parent.top
                right: parent.right
                bottom: parent.bottom
        }
    }
}
