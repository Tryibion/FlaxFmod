/* -------------------------------------------
   FMOD Studio Script Example:
   Export GUIDs as a C, C++, or C# header file
   -------------------------------------------
 */

writeHeaderFile("cs");

studio.menu.addMenuItem({ name: "FMOD Examples\\Export GUIDs Header",
    subMenuItems: function() {
        return [
            {
                name: "C Header File",
                execute: function() {
                    writeHeaderFile("c");
                }
            },
            {
                name: "C++ Header File",
                execute: function() {
                    writeHeaderFile("cpp");
                }
            },
            {
                name: "C# File",
                execute: function() {
                    writeHeaderFile("cs");
                }
            }
        ];
    }
});

function writeHeaderFile(headerType) {
    var headerFileName;
    var headerLanguage;
    if (headerType === "cpp") {
        headerFileName = "fmod_studio_guids.hpp";
        headerLanguage = "C++ header";
    } else if (headerType === "c") {
        headerFileName = "fmod_studio_guids.h";
        headerLanguage = "C header";
    } else {
        headerFileName = "fmod_studio_guids.cs";
        headerLanguage = "C#";
    }

    var outputPath = studio.project.filePath;
    var projectName = outputPath.substr(outputPath.lastIndexOf("/") + 1, outputPath.length);
    outputPath = outputPath.substr(0, outputPath.lastIndexOf("/") + 1) + headerFileName;

    var textFile = studio.system.getFile(outputPath);
    if (!textFile.open(studio.system.openMode.WriteOnly)) {    
        alert("Failed to open file {0}\n\nCheck the file is not read-only.".format(outputPath));
        console.error("Failed to open file {0}.".format(outputPath));
        return;
    }
    
    textFile.writeText("/*\r\n    " + headerFileName + " - FMOD Studio API\r\n\r\n    Generated GUIDs for project '" + projectName +"'\r\n*/\r\n\r\n");
    if (headerType === "cpp" || headerType === "c") {
        textFile.writeText("#ifndef FMOD_STUDIO_GUIDS\r\n");
        textFile.writeText("#define FMOD_STUDIO_GUIDS\r\n\r\n");
        textFile.writeText("#include \"fmod_common.h\"\r\n\r\n");
    }


    if (headerType === "cpp" || headerType === "cs") {
        textFile.writeText("namespace FSPRO\r\n{\r\n");
    }
    
    writeAllGUIDs(textFile, headerType);
    
    if (headerType === "cpp" || headerType === "cs") {
        textFile.writeText("}\r\n\r\n");
    }

    if (headerType === "cpp" || headerType === "c") {
        textFile.writeText("#endif");
    }
    textFile.close();

    alert("{0} file successfully created at:\n\n{1}".format(headerLanguage, outputPath));
    console.log("Header file successfully created at: " + outputPath);
}

function writeAllGUIDs(textFile, headerType) {
    var allEvents = studio.project.model.Event.findInstances();
    var allBuses = studio.project.model.MixerGroup.findInstances();
    allBuses = allBuses.concat(studio.project.model.MixerReturn.findInstances());
    allBuses = allBuses.concat(studio.project.workspace.mixer.masterBus);
    var allVCAs = studio.project.model.MixerVCA.findInstances();
    var allSnapshots = studio.project.model.Snapshot.findInstances();
    var allBanks = studio.project.model.Bank.findInstances({includeDerivedTypes: true});
    printGUID(allEvents, textFile, headerType, "Event");
    printGUID(allBuses, textFile, headerType, "Bus");
    printGUID(allVCAs, textFile, headerType, "VCA");
    printGUID(allSnapshots, textFile, headerType, "Snapshot");
    printGUID(allBanks, textFile, headerType, "Bank");
}

function printGUID(managedObjects, textFile, headerType, managedObjectType) {
    if (managedObjects.length === 0) {
        return;
    }

    managedObjects.sort(function(a, b) {
        var pathA = a.getPath().toUpperCase();
        var pathB = b.getPath().toUpperCase();

        if (pathA < pathB) {
            return -1;
        }
        if (pathA > pathB) {
            return 1;        
        }
        return 0;
    });

    if (headerType === "cpp") {
        textFile.writeText("    namespace " + managedObjectType + "\r\n    {\r\n");
    } else if (headerType === "cs") {
        textFile.writeText("    public class " + managedObjectType + "\r\n    {\r\n");
    }

    var whitespace = "";
    var identifierPrefix = "";
    var guidType = "";
    var guidConstructor = "";
    
    if (headerType === "cpp") {
        whitespace = "        ";
        guidType = "static const FMOD_GUID ";
    } else if (headerType === "cs"){
        whitespace = "        ";
        guidType = "public static readonly FMOD.GUID ";
        guidConstructor = "new FMOD.GUID ";
    } else {
        guidType = "static const FMOD_GUID ";
        identifierPrefix = managedObjectType + "_";
    }

    managedObjects.forEach(function(object) {
        if (object.isOfExactType("Event")) {
            if (object.banks.length === 0) {
                return;
            }
        }

        var identifier = identifierPrefix + identifierForObject(object);
        identifier = identifier.replace(/(^[0-9])/g, "_$1"); // Don't allow identifier to start with a number

        var guid = guidStringToStruct(object.id, headerType);
        textFile.writeText(whitespace + guidType + identifier + " = " + guidConstructor + guid + ";\r\n");
    });

    if (headerType === "cpp" || headerType === "cs") {
        textFile.writeText("    }\r\n\r\n");
    } else {
        textFile.writeText("\r\n");
    }
}

function identifierForObject(managedObject) {
    var path = managedObject.getPath();
    if (managedObject.isOfExactType("MixerMaster")) {
        path = managedObject.name;
    }

    path = path.replace(/^(.*):\/|[\(\)]/g, '');
    path = path.replace(/[^\w]/g, "_");
    return path;
}

function toInt32(unsignedValue) {
    // signed right shift treats its operands as signed 32-bit integers
    return unsignedValue >> 0;
}

function parseInt32FromHex(hex) {
    return toInt32(parseInt(hex, 16));
}

function guidStringToStruct(guidString, headerType) {
    var guidContents = guidString.replace(/[{}-]/g, ""); 

    // for example {1f687138-e06c-40f5-9bac-57f84bbcedd3}
    var groupedGuid = [
        guidContents.substring(0, 8),                 // the first 8 hexadecimal digits of the GUID   - 1f687138
        guidContents.substring(8, 12),                // the first group of 4 hexadecimal digits      - e06c
        guidContents.substring(12, 16),               // the second group of 4 hexadecimal digits     - 40f5
        guidContents.substring(16, 32).match(/.{2}/g) // array of 8 bytes                             - [9b, ac, 57, f8, 4b, bc, ed, d3]
    ];

    function groupToStructure(group) {
        if (headerType === "cs") {
            var formatted = [
                "Data1 = " + parseInt32FromHex(group[0]),
                "Data2 = " + parseInt32FromHex(group[2] + group[1]),
                "Data3 = " + parseInt32FromHex(group[3][3] + group[3][2] + group[3][1] + group[3][0]),
                "Data4 = " + parseInt32FromHex(group[3][7] + group[3][6] + group[3][5] + group[3][4]),
            ];
            return formatted.join(", ");
        } else {
            return group.map(function(item) {
                if (item instanceof Array) {
                    return "{" + groupToStructure(item) + "}";
                } else {
                    return '0x' + item;
                }
            }).join(", ");
        }
    }
    
    return "{ " + groupToStructure(groupedGuid) + " }";
}
