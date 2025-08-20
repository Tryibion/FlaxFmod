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
    
    // Convert the event data array to a JSON string with 4 spaces for indentation.
    var jsonOutput = JSON.stringify(eventData, null, 4);

    var outputPath = studio.project.filePath;
    var projectName = outputPath.substr(outputPath.lastIndexOf("/") + 1, outputPath.length);
    outputPath = outputPath.substr(0, outputPath.lastIndexOf("/") + 1) + "fmod_events_export.json";

    var textFile = studio.system.getFile(outputPath);

    // Define the path for the output file in the project's root directory.
    textFile.open(studio.system.openMode.WriteOnly | studio.system.openMode.Truncate);
    textFile.writeText(jsonOutput);
    textFile.close();

    // Log a success message to the FMOD Studio console.
    alert("FMOD event data exported to: " + outputPath);
}

// Execute the main function.
exportEventsToJson();