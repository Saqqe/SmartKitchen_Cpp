import json, sys, os, glob, httplib, string
import xml.etree.ElementTree as ET
import urllib
import urllib2
import requests

##Connect to parse.com
connection = httplib.HTTPSConnection('api.parse.com', 443)
connection.connect()
connection.request('GET', '/1/classes/ConfirmedImages/uXFlSqSVMu', '', {
       "X-Parse-Application-Id": "qLDDiNkDY2lDYxj3eMFkXVSqehoCwYwlxlAxSEOp",
       "X-Parse-REST-API-Key": "1KYIj0xgLzXmCyjkfQM5VMMHRHjCfiRD3lo281D4"
     })
result = json.loads(connection.getresponse().read())
#print result

##Get the fileName and fileURL from json respons!
url     = result['json']['url']
name    = result['json']['name']

##Download the json file
f = urllib2.urlopen(url)
data = f.read()
with open("ConfirmedImages.json", "wb") as code:
    code.write(data)

#import elementtree.ElementTree as ET
tree = ET.parse("template.xml")
# if you need the root element, use getroot
root = tree.getroot()

##Read in jsonData
with open("ConfirmedImages.json", "r") as infile:
    jsonIn = json.load(infile)

#print jsonIn
pics = glob.glob("*.jpg")
#print pics

##Write to xmlFile
for i in pics:
    for j in jsonIn:
        if ( i == j['picName']):
            top         = string.replace(json.dumps(j['y']), '"', '\'')
            left        = string.replace(json.dumps(j['x']), '"', '\'')
            width       = string.replace(json.dumps(j['width']), '"', '\'')
            height      = string.replace(json.dumps(j['height']), '"', '\'')
            itemName    = string.replace(json.dumps(j['itemName']), '"', '')
            filePath    = string.replace(os.path.realpath(i), '"', '\'')
            #print "Found name!\n"
            images = root[2]
            image = ET.SubElement(images, "image")
            ##Fullpath
            image.set('file', filePath)
            box = ET.SubElement(image, "box")
            box.set('top', '123')
            box.set('left', left)
            box.set('width', width)
            box.set('height', height)
            label = ET.SubElement(box, "label")
            label.text = itemName
            tree.write(itemName+".xml")

##TODO: Train and create new svn!
