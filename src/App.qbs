import qbs.Process
import qbs.Probes

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
                "find_package(KF6 6.0 REQUIRED COMPONENTS I18n Config KIO)\n" +
                "",

                "KF6::ConfigCore KF6::I18n KF6::KIOCore KF6::KIOGui KF6::KIOFileWidgets KF6::KIOWidgets",
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
    cpp.cxxFlags: spice.cflags
    cpp.linkerFlags: spice.libs

	Group {
		files: ["../data/**"]
		fileTags: "qt.core.resource_data"
		Qt.core.resourceSourceBase: "../data/"
		Qt.core.resourcePrefix: "/"
	}

    Qt.qml.importName: "org.kde.ilowawa"
    Qt.qml.importVersion: "1.0"

	Probes.PkgConfigProbe {
		id: spice
		name: "spice-client-glib-2.0"	
	}
	qbsModuleProviders: ["qbspkgconfig", "Qt"]
	Depends { name: "Qt"; submodules: ["core", "gui", "widgets", "quick", "quickcontrols2", "qml", "sql", "concurrent"] }
}
