import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.15 as Kirigami
import org.kde.ilowawa 1.0 as Wawa

QQC2.TabButton {
	id: del

	required property int index
	required property Wawa.Document modelData

	Drag.dragType: Drag.Automatic
	Drag.active: dragHandler.active
	Drag.onDragStarted: {
		del.grabToImage(function(result) {
			del.Drag.imageSource = result.url
		})
	}

	DragHandler {
		id: dragHandler
		target: null
	}

	implicitWidth: implicitContentWidth + leftPadding + rightPadding
	leftPadding: Kirigami.Units.largeSpacing
	contentItem: RowLayout {
		QQC2.Label {
			text: "dansa med oss"
			horizontalAlignment: Qt.AlignHCenter

			Layout.fillWidth: true
			Layout.minimumWidth: Kirigami.Units.gridUnit * 3
		}
		QQC2.ToolButton {
			icon.name: "tab-close"
			onClicked: del.modelData.window.closeDocument(del.index)
		}
	}
}
