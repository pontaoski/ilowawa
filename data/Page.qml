import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.15 as Kirigami
import QtQml.Models 2.15
import org.kde.ilowawa 1.0 as Wawa

QQC2.Page {
	id: page

	required property Wawa.Document document

	contentItem: ColumnLayout {
		Wawa.Spice {
			id: spice
			url: feld.text
		}
		QQC2.TextField {
			id: feld
		}
		QQC2.Button {
			onClicked: spice.start()
		}
	}
}