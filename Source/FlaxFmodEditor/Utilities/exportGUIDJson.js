function exportBanksToJson() {
    
    // Get a flat list of all banks in the project.
    var banks = studio.project.model.Bank.findInstances();
    
    // An array to hold the bank data.
    var bankData = [];
    
    // Iterate through the list of all banks.
    for (var i = 0; i < banks.length; i++) {
        var bank = banks[i];
        
        // Push the bank's path and GUID to our data array.
        bankData.push({
            "Path": bank.getPath(),
            "Guid": bank.id.toString(),
        });
    }

    var outputPath = exportData(bankData, "fmod_banks_export");

    // Log a success message to the FMOD Studio console.
    console.log("FMOD bank data exported to: " + outputPath);
}

function exportEventsToJson() {
    
    // Get a flat list of all events in the project.
    var events = studio.project.model.Event.findInstances();
    
    // An array to hold the event data.
    var eventData = [];
    
    // Iterate through the list of all events.
    for (var i = 0; i < events.length; i++) {
        var event = events[i];
        
        // Push the event's path and GUID to our data array.
        eventData.push({
            "Path": event.getPath(),
            "Guid": event.id.toString(),
        });
    }

    var outputPath = exportData(eventData, "fmod_events_export");

    // Log a success message to the FMOD Studio console.
    console.log("FMOD event data exported to: " + outputPath);
}


function exportBusesToJson() {

    // Get all buses
    var allBuses = studio.project.model.MixerGroup.findInstances();
    allBuses = allBuses.concat(studio.project.model.MixerReturn.findInstances());
    allBuses = allBuses.concat(studio.project.workspace.mixer.masterBus);

    // An array to hold the bus data.
    var busData = [];
    
    // Iterate through the list of all buses.
    for (var i = 0; i < allBuses.length; i++) {
        var bus = allBuses[i];
        
        // Push the bus's path and GUID to our data array.
        busData.push({
            "Path": bus.getPath(),
            "Guid": bus.id.toString(),
        });
    }

    var outputPath = exportData(busData, "fmod_bus_export");

    // Log a success message to the FMOD Studio console.
    console.log("FMOD bus data exported to: " + outputPath);
}

function exportSnapshotsToJson() {

    // Get a flat list of all snapshots in the project.
    var snapshots = studio.project.model.Snapshot.findInstances();

    // An array to hold the snapshots data.
    var snapshotData = [];

    // Iterate through the list of all banks.
    for (var i = 0; i < snapshots.length; i++) {
        var snapshot = snapshots[i];

        // Push the bank's path and GUID to our data array.
        snapshotData.push({
            "Path": snapshot.getPath(),
            "Guid": snapshot.id.toString(),
        });
    }

    var outputPath = exportData(snapshotData, "fmod_snapshots_export");

    // Log a success message to the FMOD Studio console.
    console.log("FMOD snapshot data exported to: " + outputPath);
}

function exportVCAsToJson() {
    // Get all VCAs
    var allVCA = studio.project.model.MixerVCA.findInstances();

    // An array to hold the vca data.
    var vcaData = [];
    
    // Iterate through the list of all vca.
    for (var i = 0; i < allVCA.length; i++) {
        var vca = allVCA[i];
        
        // Push the vca's path and GUID to our data array.
        vcaData.push({
            "Path": vca.getPath(),
            "Guid": vca.id.toString(),
        });
    }

    var outputPath = exportData(vcaData, "fmod_vca_export");

    // Log a success message to the FMOD Studio console.
    console.log("FMOD vca data exported to: " + outputPath);
}

function exportData(data, fileName) {
    // Convert the vca data array to a JSON string with 4 spaces for indentation.
    var dataJsonOutput = JSON.stringify(data, null, 4);

    var outputPath = studio.project.filePath;
    var projectName = outputPath.substr(outputPath.lastIndexOf("/") + 1, outputPath.length);
    outputPath = outputPath.substr(0, outputPath.lastIndexOf("/") + 1) + fileName + ".json";

    var textFile = studio.system.getFile(outputPath);

    // Define the path for the output file in the project's root directory.
    textFile.open(studio.system.openMode.WriteOnly | studio.system.openMode.Truncate);
    textFile.writeText(dataJsonOutput);
    textFile.close();

    return outputPath;
}

// Execute the main function.
exportBanksToJson();
exportEventsToJson();
exportBusesToJson();
exportSnapshotsToJson();
exportVCAsToJson();
alert("FMOD project export completed");