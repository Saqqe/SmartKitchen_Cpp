#!/usr/bin/python

import json, os, glob, httplib, time
from jsonobject import *

##Jsonobject
class Image(JsonObject):
    picName = StringProperty()
    picURL  = StringProperty()
    x       = IntegerProperty()
    y       = IntegerProperty()
    width   = IntegerProperty()
    height  = IntegerProperty()

##Connect to parse.com
connection = httplib.HTTPSConnection('api.parse.com', 443)
connection.connect()

##Keys for Parse.com
parseAppID      = "qLDDiNkDY2lDYxj3eMFkXVSqehoCwYwlxlAxSEOp"
parseRESTApiKey = "1KYIj0xgLzXmCyjkfQM5VMMHRHjCfiRD3lo281D4"

os.chdir("UnknownPics")

pics = {}
pics = glob.glob("*.jpg")

jsonObjs = []
#print pics

for x in pics:
    #print x
    ##Upload the pics
    connection.request('POST', '/1/files/'+x, open(x, 'rb').read(),
        {
           "X-Parse-Application-Id": parseAppID,
           "X-Parse-REST-API-Key": parseRESTApiKey,
           "Content-Type": "image/jpeg"
         })
    imageUploadResult = json.loads(connection.getresponse().read())
    #print result
    ##Save the url's for the pics in a array. [picLocalName : picUrl]
    jsonObjs.append( Image( picName = x, picURL = imageUploadResult['url'],
                            x = 0, y = 0, width = 0, height = 0 ).to_json() )

##Save the array in a json file
with open("ImagesURLS.json", "w") as outfile:
        json.dump(jsonObjs, outfile)

##Upload the json file
connection.request('POST', '/1/files/ImagesURLS.json', open("ImagesURLS.json", "rb").read(), {
       "X-Parse-Application-Id": parseAppID,
       "X-Parse-REST-API-Key": parseRESTApiKey,
       "Content-Type": "text/json"
     })

jsonUploadResult        = json.loads(connection.getresponse().read())
jsonFileName            = jsonUploadResult['name']
jsonFileURL             = jsonUploadResult['url']

##Associate the json file with the parseObject
connection.request('PUT', '/1/classes/UnknownImage/QjestMNLzB', json.dumps({
       "name": "ImageFileNames",
       "json": {
         "name": jsonFileName,
         "__type": "File"
       }
     }), {
       "X-Parse-Application-Id": parseAppID,
       "X-Parse-REST-API-Key": parseRESTApiKey,
       "Content-Type": "application/json"
     })
associateResult = json.loads(connection.getresponse().read())

##Notify the Android user
connection.request('POST', '/1/push', json.dumps({
       "where": {
         "deviceType": "android"
       },
       "data": { 
              "action": "com.example.saqibfredrik.smartkitchen.CUSTOM_NOTIFICATION",
              "url": jsonFileURL
            }
     }), {
       "X-Parse-Application-Id": "qLDDiNkDY2lDYxj3eMFkXVSqehoCwYwlxlAxSEOp",
       "X-Parse-REST-API-Key": "1KYIj0xgLzXmCyjkfQM5VMMHRHjCfiRD3lo281D4",
       "Content-Type": "application/json"
     })
androidPushResult = json.loads(connection.getresponse().read())
