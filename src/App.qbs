import qbs.Process

QtApplication {
	name: "org.kde.Ilowawa"
	files: [
		"*.cpp",
		"*.h",
	]
    cpp.cxxLanguageVersion: "c++17"
    install: true

    Probe {
        id: mu
        property string src: product.sourceDirectory
        property var linkerFlags
        property var includeDirs
        configure: {
            var proc = new Process()
            var exitCode = proc.exec("bash", [mu.src + "/extract_flags.sh",
                "find_package(KF5 5.70 REQUIRED COMPONENTS I18n Config KIO)\n" +
                "",

                "KF5::ConfigCore KF5::I18n KF5::KIOCore KF5::KIOFileWidgets KF5::KIOWidgets",
            ])
            if (exitCode != 0) {
                console.error(proc.readStdOut())
                throw "extracting flags from CMake libraries failed"
            }
            var stdout = proc.readStdOut()
            stdout = stdout.split("====")
            linkerFlags = stdout[0].split("\n").filter(function(it) { return Boolean(it) && !it.contains("rpath") && (it.startsWith("/") || it.startsWith("-l")) }).map(function(it) { return it.replace("-Wl,", "") })
            includeDirs = stdout[1].split("\n").filter(function(it) { return Boolean(it) && !it.contains("rpath") && (it.startsWith("/") || it.startsWith("-l")) }).map(function(it) { return it.replace("-Wl,", "") })
        }
    }
    cpp.driverLinkerFlags: mu.linkerFlags
    cpp.includePaths: mu.includeDirs.concat([sourceDirectory])
    cpp.defines: ["QT_NO_KEYWORDS"]

	Group {
		files: ["../data/**"]
		fileTags: "qt.core.resource_data"
		Qt.core.resourceSourceBase: "../data/"
		Qt.core.resourcePrefix: "/"
	}

    Qt.qml.importName: "org.kde.ilowawa"
    Qt.qml.importVersion: "1.0"

    Depends { name: "spice-client-glib-2.0" }
	Depends { name: "Qt"; submodules: ["core", "gui", "widgets", "quick", "quickcontrols2", "qml", "sql", "concurrent"] }
}
