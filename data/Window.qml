import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.15 as Kirigami
import org.kde.ilowawa 1.0 as Wawa

QQC2.ApplicationWindow {
	id: window

	visible: true
	title: i18n("Wawa - Wawa")

	required property Wawa.Window window

	header: WindowTabBar {
		id: tabBar
		window: window.window
	}

	StackLayout {
		currentIndex: tabBar.currentIndex
		anchors.fill: parent

		Repeater {
			model: window.window.documents
			delegate: Page {
				required property Wawa.Document modelData

				document: modelData
			}
		}
	}
}
