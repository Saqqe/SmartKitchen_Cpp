#!/usr/bin/python

import sys, json,httplib

def close():
    #Close connection
    connection.close()
    #print "Server updated"
    sys.exit()

#Connect to parse.com and GET data from "LagerStatus", with REST API
connection = httplib.HTTPSConnection('api.parse.com', 443)
connection.connect()

#Keys for Parse.com
parseAppID      = "qLDDiNkDY2lDYxj3eMFkXVSqehoCwYwlxlAxSEOp"
parseRESTApiKey = "1KYIj0xgLzXmCyjkfQM5VMMHRHjCfiRD3lo281D4"

#CMD "parse" get the FIRST argument. argv[0] is program name
toDo = sys.argv[1]

if(toDo == "updateLocalInventory"):
    connection.request('GET', '/1/classes/LagerStatus', '', {
           "X-Parse-Application-Id": parseAppID,
           "X-Parse-REST-API-Key": parseRESTApiKey
         })
    #Parse sends data in json-format
    serverResult = json.loads(connection.getresponse().read())

    #Close connection to parse.com
    connection.close()

    #Retrive data we are interested in
    json_data = serverResult["results"][0]
    data = {}
    for key, value in json_data.items():
        #Get rid of unnecessary
        if "updatedAt" not in key and "objectId" not in key and "createdAt" not in key:
            data[key] = value

    #Save data in "Inventory.json" file in json format.
    with open("Inventory.json", "w") as outfile:
            json.dump(data, outfile)

    #print "Local updated!"
    sys.exit()

if(toDo == "updateServerInventory"):
    #Read local Inventory status
    with open("Inventory.json", "r") as jsonFile:
        parsedJson = json.load(jsonFile)

    #Update server with the new data
    connection.request('PUT', '/1/classes/LagerStatus/Qvd9aQNSO0', json.dumps(parsedJson), 
                {
                    "X-Parse-Application-Id": parseAppID,
                    "X-Parse-REST-API-Key": parseRESTApiKey,
                    "Content-Type": "application/json"
                })
#Exit program
close();
