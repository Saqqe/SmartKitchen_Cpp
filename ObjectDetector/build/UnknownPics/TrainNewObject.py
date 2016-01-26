import json, sys, os, glob, httplib, string, subprocess, time, shutil
import xml.etree.ElementTree as ET
import urllib
import urllib2
import requests

knownPicsFolderPath = "/home/pi/ownDev/SmartKitchen_Cpp/ObjectDetector/build/KnownPics/"

#---------------------------------------------Functions--------------------------------------------------------------------------------

#Deletes .jpg files from the target dir IF it-s tagged with "Nada" in json
def removeNadaItem( jsonIN ):
    tempCWD = os.getcwd()
    os.chdir("/home/pi/ownDev/SmartKitchen_Cpp/ObjectDetector/build/UnknownPics/")
    for item in jsonIN:
        itemName    = string.replace(json.dumps(item['itemName']), '"', '')
        picName     = string.replace(json.dumps(item['picName']), '"', '')
        pics        = glob.glob("*.jpg")
        for pic in pics:
            if itemName == "Nada":
                os.remove(picName)
    os.chdir(tempCWD)
    

def createDirIfNoExist( folderName ):
    tempCWD = os.getcwd()
    newFolder = False
    ##Create a folder if there is non
    if not os.path.exists(knownPicsFolderPath+folderName):
        os.chdir(knownPicsFolderPath)
        os.makedirs(folderName)
        newFolder = True
    os.chdir(tempCWD)
    return newFolder

##Moves pcitures from source to destination, destination must not contain
##files with the same name as source or error will occur.
##Destination directory must exist
def movePictures( pic, itemName ):
    temp = os.getcwd()
    os.chdir("/home/pi/ownDev/SmartKitchen_Cpp/ObjectDetector/build/UnknownPics/")
    destination = knownPicsFolderPath+itemName+"/"
    shutil.move(pic, destination+pic)
    os.chdir(temp)
    return True

##Copy pcitures from source to destination, destination must not contain
##files with the same name as source or error will occur.
##Destination directory must exist
def copyPictures( source ):
    temp = os.getcwd()
    os.chdir("/home/pi/ownDev/SmartKitchen_Cpp/ObjectDetector/build/KnownPics/"+source+"/")
    pics = glob.glob("*.jpg")
    destination = "/home/pi/ownDev/SmartKitchen_Cpp/ObjectDetector/build/InComing/"
    if pics:
        for pic in pics:
            shutil.copyfile(pic, destination+pic)
    os.chdir(temp)

def trainNewDetector( itemName ):
    tempCWD = os.getcwd()
    os.chdir("/home/pi/ownDev/SmartKitchen_Cpp/ObjectDetector/build")
    ls_out = subprocess.check_output(['ls'])
    trainObjectDetector = "train_object_detector"
    #print ls_out
    if( trainObjectDetector in ls_out ):
        os.popen("./train_object_detector -tv -c 5 --flip /home/pi/ownDev/SmartKitchen_Cpp/ObjectDetector/build/KnownPics/"
                 + itemName + "/" + itemName +".xml")
    copyPictures(itemName)
    subprocess.Popen("./main /home/pi/ownDev/SmartKitchen_Cpp/ObjectDetector/build/InComing/*.jpg", shell =True)
    os.chdir(tempCWD)

#----------------------------------------------------------End of functions-------------------------------------------------------------------------

##Loop every 30sec
while True:
    ##Connect to parse.com
    connection = httplib.HTTPSConnection('api.parse.com', 443)
    connection.connect()
    connection.request('GET', '/1/classes/ConfirmedImages/uXFlSqSVMu', '', {
           "X-Parse-Application-Id": "qLDDiNkDY2lDYxj3eMFkXVSqehoCwYwlxlAxSEOp",
           "X-Parse-REST-API-Key": "1KYIj0xgLzXmCyjkfQM5VMMHRHjCfiRD3lo281D4"
         })
    result = json.loads(connection.getresponse().read())
    #print result

    isReady = result['isReady']
    print isReady
    if isReady is True:
        ##Get the fileName and fileURL from json respons!
        url     = result['json']['url']
        name    = result['json']['name']
        
        ##Update to false
        connection.request('PUT', '/1/classes/ConfirmedImages/uXFlSqSVMu', json.dumps({
               "isReady": False
             }), {
               "X-Parse-Application-Id": "qLDDiNkDY2lDYxj3eMFkXVSqehoCwYwlxlAxSEOp",
               "X-Parse-REST-API-Key": "1KYIj0xgLzXmCyjkfQM5VMMHRHjCfiRD3lo281D4",
               "Content-Type": "application/json"
             })
        result = json.loads(connection.getresponse().read())
        
        ##Download the json file
        f = urllib2.urlopen(url)
        data = f.read()
        with open("ConfirmedImages.json", "wb") as code:
            code.write(data)

        pics = glob.glob("*.jpg")

        ##Read in jsonData
        with open("ConfirmedImages.json", "r") as infile:
            jsonIn = json.load(infile)

        newFolders              = []
        newPicsInOldFolder      = []
        movedPicToOldFolderName = []
        isPicsMoved             = False
        itemName                = ""

        ##Del pics tagged with "Nada"
        removeNadaItem( jsonIn )
        
        for pic in pics:
            for item in jsonIn:
                itemName    = string.replace(json.dumps(item['itemName']), '"', '')
                if itemName != "Nada":
                    newFolder   = createDirIfNoExist( itemName )
                    if newFolder is True:
                        newFolders.append(itemName)
                    ##Move pic to right folder!
                    picName = string.replace(json.dumps(item['picName']), '"', '')
                    if( pic == picName ):
                        movePictures(pic, itemName)
                        movedPicToOldFolderName.append(pic)
                        if itemName not in newFolders and itemName not in newPicsInOldFolder:
                            newPicsInOldFolder.append(itemName)    

        ##Check if "newFolders" contains data
        if newFolders:
            #print newFolders
            for folder in newFolders:
                ##Change working dir to first element in "newFolders"
                os.chdir("/home/pi/ownDev/SmartKitchen_Cpp/ObjectDetector/build/KnownPics/"+folder+"/")
                #print os.getcwd()

                pics = glob.glob("*.jpg")
                tree = ET.parse("/home/pi/ownDev/SmartKitchen_Cpp/ObjectDetector/build/KnownPics/template.xml")
                root = tree.getroot()

                itemName = ""
                for item in jsonIn:
                    for pic in pics:
                        if ( pic == item['picName'] ):
                            top         = string.replace(json.dumps(item['y']), '"', '\'')
                            #print "Top: " + top
                            left        = string.replace(json.dumps(item['x']), '"', '\'')
                            #print "lft: " + left
                            width       = string.replace(json.dumps(item['width']), '"', '\'')
                            #print "width: " + width
                            height      = string.replace(json.dumps(item['height']), '"', '\'')
                            #print "height: " + height
                            itemName    = string.replace(json.dumps(item['itemName']), '"', '')
                            #print "itemName: " + itemName
                            filePath    = string.replace(os.path.realpath(pic), '"', '\'')
                            #print "filePath: " + filePath
                            
                            #print "Found name!\n"
                            images = root[2]
                            image = ET.SubElement(images, "image")
                            ##Fullpath
                            image.set('file', filePath)
                            box = ET.SubElement(image, "box")
                            box.set('top', top)
                            box.set('left', left)
                            box.set('width', width)
                            box.set('height', height)
                            label = ET.SubElement(box, "label")
                            label.text = itemName
                            tree.write(itemName+".xml")
                ##Train and create new SVM!
                #print "In newFolders Start train for " + itemName + "\n"
                trainNewDetector(itemName)

        if newPicsInOldFolder:
            #print movedPicToOldFolderName
            for folder in newPicsInOldFolder:
                print folder
                ##Change working dir to first element in "newFolders"
                os.chdir("/home/pi/ownDev/SmartKitchen_Cpp/ObjectDetector/build/KnownPics/"+folder)
                print os.getcwd()

                itemName = ""
                for item in jsonIn:
                    for pic in movedPicToOldFolderName:
                        if ( pic == item['picName'] and folder == item['itemName'] ):
                            top         = string.replace(json.dumps(item['y']), '"', '\'')
                            #print "Top: " + top
                            left        = string.replace(json.dumps(item['x']), '"', '\'')
                            #print "lft: " + left
                            width       = string.replace(json.dumps(item['width']), '"', '\'')
                            #print "width: " + width
                            height      = string.replace(json.dumps(item['height']), '"', '\'')
                            #print "height: " + height
                            itemName    = string.replace(json.dumps(item['itemName']), '"', '')
                            #print "itemName: " + itemName
                            filePath    = string.replace(os.path.realpath(pic), '"', '\'')
                            #print "filePath: " + filePath

                            tree = ET.parse(itemName+".xml")
                            root = tree.getroot()
                            
                            #print "Found name!\n"
                            images = root[2]
                            image = ET.SubElement(images, "image")
                            ##Fullpath
                            image.set('file', filePath)
                            box = ET.SubElement(image, "box")
                            box.set('top', top)
                            box.set('left', left)
                            box.set('width', width)
                            box.set('height', height)
                            label = ET.SubElement(box, "label")
                            label.text = itemName
                            tree.write(itemName+".xml")
                ##Train and create new SVM! and then varify the detector
                #print "In newFolders Start train for " + itemName + "\n"
                trainNewDetector(itemName)
    time.sleep(30)
