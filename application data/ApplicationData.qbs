Product {
	name: "application-data"

	Group {
		files: ["22/org.kde.Ilowawa.svg"]
		qbs.install: true
		qbs.installDir: "share/icons/hicolor/22x22/apps"
	}
	Group {
		files: ["48/org.kde.Ilowawa.svg"]
		qbs.install: true
		qbs.installDir: "share/icons/hicolor/48x48/apps"
	}
	Group {
		files: ["org.kde.Ilowawa.desktop"]
		qbs.install: true
		qbs.installDir: "share/applications"
	}
}
