/**
* Main program for object recognition
* main.cpp
* Project grupp: Edlund&Sarker
*
* @author Saqib Sarker
* @version 0.1 2015-10-06
*/

#include <dlib/image_loader/load_image.h>
#include <dlib/svm_threaded.h>
#include <dlib/string.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_processing.h>
#include <dlib/data_io.h>
#include <dlib/cmd_line_parser.h>

#include "json.hpp"
#include <sys/types.h>
#include <dirent.h>
#include <string>
#include <errno.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <ratio>
#include <chrono>

using namespace std;
using namespace dlib;
using namespace std::chrono;
using json = nlohmann::json;

int status;

/*!
* @updateInventory
*
* @param action "local" will GET data from server and update local .json
*               "server" will PUT data from .json to server
*
* @return bool  True if command was run
*/
bool updateInventory(string action)
{
    if(system(NULL) && file_exists("ServerInventoryHandler.py"))
    {
        int i;
        if(action.compare("local") == 0)
        {
            i = system("./ServerInventoryHandler.py updateLocalInventory");
            return true;
        }
        else if (action.compare("server") == 0)
        {
            i = system("./ServerInventoryHandler.py updateServerInventory");
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}//End of updateInventory

/*!
* @unknownImagesHandling
*
* @return void
*/
void unknownImagesHandling()
{
    if(system(NULL) && file_exists("ServerUnknownImagesHandler.py"))
    {
        int i;
        i = system("./ServerUnknownImagesHandler.py");
    }
}//End of unknownImagesHandling

/*!
* @getSVMFiles
*
* @param dir        Path to check
* @param vector     Address for a vector
*
* @disp             Fills the vector with .svm files found at the given path.
*
* @return int
*/
int getSVMFiles (string dir, std::vector<std::string> &files)
{
    DIR *dp;
    struct dirent *dirp;
    size_t found;
    string temp;

    if((dp  = opendir(dir.c_str())) == NULL) {
        cout << "Error(" << errno << ") opening " << dir << endl;
        return errno;
    }

    while ((dirp = readdir(dp)) != NULL) {

        temp  = string(dirp->d_name);
        found = temp.find_last_of(".");

        if(temp.substr(found+1) == "svm")
        {
            files.push_back(string(dirp->d_name));
        }
    }
    closedir(dp);
    return 0;
}//End of getSVMFiles

/*!
* @ remove_extension
*
* @param fileName   Address to the filename
*
* @return string    Sends back the filename without a extension
*/
std::string remove_extension(const std::string& fileName)
{
    size_t lastdot = fileName.find_last_of(".");
    if (lastdot == std::string::npos)
    {
        return fileName;
    }
    else
    {
        return fileName.substr(0, lastdot);
    }
}//End of remove_extension

/*!
* @findKeyInJson
*
* @param key    The key to look for
* @param &j     Address of a json object, in which to look for
*
* @return bool  True if found, else false.
*/
bool findKeyInJson(string key, json &j)
{
    if(j.find(key) != j.end())
    {
        return true;
    }
    else
        return false;
}//End of findKeyInJson

/*!
* @increaseValueByOneJson
*
* @param key    The key to look for
* @param &j     Address of a json object, in which to look for
*
* @return bool  True if key was found and value was increased, else false.
*/
bool increaseValueByOneJson(string key, json &j)
{
    if(findKeyInJson(key, j))
    {
        int temp = j.at(key);
        ++temp;
        j.at(key) = temp;
        return true;
    }
    else
        return false;
}//End of increaseValueByOneJson

/*!
* @decreaseValueByOneJson
*
* @param key    The key to look for
* @param &j     Address of a json object, in which to look for
*
* @return bool  True if key was found and value was decreased, else false.
*/
bool decreaseValueByOneJson(string key, json &j)
{
    if(findKeyInJson(key, j))
    {
        int temp = j.at(key);
        if(temp > 0)
        {
            --temp;
            j.at(key) = temp;
        }
        return true;
    }
    else
        return false;
}//End of decreaseValueByOneJson

/*!
* @openJsonFile
*
* @param fileName    File to open
* @param &j          Address of a json object
*
* @return bool       True if fileName was found and information as parsed into the json object, else false.
*/
bool openJsonFile(string fileName, json& j)
{
    if(updateInventory("local"))
    {
        std::ifstream file(fileName);
        if(file)
        {
            //cout << "File found" << endl;
            j = json::parse(file);
            cout << "Data inside: " << fileName << endl;
            cout << j.dump(4) << endl; // Testing
            file.close();
            return true;
        }
        else return false;
    }
    else return false;
}//End of openJsonFile

/*!
* @saveJsonToFile
*
* @param fileName    File to open
* @param &j          Address of a json object
*
* @return bool       True if json was saved in file, else false.
*/
bool saveJsonToFile(string fileName, json& j)
{
    std::ofstream outFile(fileName);
    if(outFile.is_open() && !j.empty())
    {
        //cout's for demo!
        cout << "Saveing this to: " << fileName << endl;
        cout << j.dump(4) << endl;
        outFile << j.dump(4) << endl;

        outFile.close();
        updateInventory("server");
        return true;
    }
    else return false;
}//End of saveJsonToFile

/*!
* @addNewItemJson
*
* @param itemName    Item to add
* @param amount      Amount of the item
* @param &j          Address of a json object
*
* @return bool       True if item as added, else false.
*/
bool addNewItemJson(string itemName, int amount, json& j)
{
    if(findKeyInJson(itemName, j))
    {
        return false;
    }
    else
    {
        j[itemName] = amount;

        if(findKeyInJson(itemName, j))
            return true;
        else
            return false;
    }
}//End of addNewItemJson

//Write to log file
void write_text_to_log_file( const std::string &text )
{
    std::ofstream log_file(
        "log_file.txt", std::ios_base::out | std::ios_base::app );
    log_file << text << std::endl;
}//End of write_text_to_log_file

std::string get_extension(const std::string& fileName)
{
    size_t lastdot = fileName.find_last_of(".");
    return fileName.substr(lastdot+1);
}//End of remove_extension


/**
* @main
*
* @parm argc
* @parm argv
*
* @return int
**/
int main(int argc, char** argv)
{
    try
    {
        //Init Command line parser
        command_line_parser cmdParser;


        /**
        * Setup options, THIS TAG IS FOR TESTING STUFF ONLY!!
        * Use --test from cmd
        */
        cmdParser.add_option("test", "Only for testing stuff!");

        //Start parsing cmd
        cmdParser.parse(argc, argv);


        /**
        * TODO: Multi object in/out handler!
        * Create temp folders
        */

        typedef scan_fhog_pyramid<pyramid_down<6> > image_scanner_type;

        /**
        * Only for testing
        */
        if(cmdParser.option("test"))
        {
            unknownImagesHandling();

            return EXIT_SUCCESS;
        }//End of if-test


        //REAL PROGRAM!
        if(cmdParser.number_of_arguments() > 0)
        {
            //Start time
            high_resolution_clock::time_point progStart = high_resolution_clock::now();

            //Init variables
            string picName              = "";
            string moveTo               = "";
            //string svmPath              = get_current_dir() +"/SVMFolder";
            string svmFolder            = "SVMFolder";
            string inventoryFileName    = "Inventory.json";
            string unknownPicsPath       = get_current_dir() + "/UnknownPics"; // Need to fix THIS PATH!!
            string incomingObj          = "InComing";
            string outgoingObj          = "OutGoing";
            string foundWith            = "";
            string itemName             = "";
            string stepOut              = ".."; //Back one level command
            bool objNotDetected         = false;
            bool deletePics             = false;
            bool itemIn                 = false;
            bool itemOut                = false;
            int objFoundCounter         = 0;
            size_t found;
            dlib::array<array2d<unsigned char> > imagesDetected;
            std::vector<std::string> files = std::vector<std::string>();
            directory myDir("."); // Currecnt dir is where the program is!

            //cout << "\n\ndirectory: " << myDir.name() << endl;
            //cout << "full path: " << myDir.full_name() << endl;
            //cout << "is root:   " << ((myDir.is_root())?"yes":"no") << endl;

            //Fetch SVM files
            //std::string dir(svmPath);
            std::vector<directory> dirs = myDir.get_dirs(); //Fetch all the dirs
            sort(dirs.begin(), dirs.end());
            for(unsigned int i = 0; i < dirs.size(); ++i)
            {
                //cout << "Dirs: " << dirs[i].name() << endl;
                if(svmFolder.compare(dirs[i].name()) == 0)
                {
                    set_current_dir(svmFolder);//Setp into svmFolder

                    getSVMFiles(get_current_dir() ,files);

                    set_current_dir(stepOut); //Back out one level
                }
            }//End of for-loop

            //Check file name, determain incoming or outgoing
            int in  = cmdParser[0].find(incomingObj);
            int out = cmdParser[0].find(outgoingObj);
            if(in > 0)
            {
                itemIn = true;
            }
            if(out > 0)
            {
                itemOut = true;
            }

            //For demo!
            if(itemIn)
            {
                cout << "Object is on the way in" << endl;
            }
            if(itemOut)
            {
                cout << "Object is on the way out" << endl;
            }
            //cout << "Itemin: " << itemIn << ", itemout: " << itemOut << endl;

            //Load images
            dlib::array<array2d<unsigned char> > images;
            images.resize(cmdParser.number_of_arguments());
            unsigned int imagesArraySize = images.size();
            //imagesDetected.resize(cmdParser.num_of_arguments());
            for(unsigned int i = 0; i < imagesArraySize; ++i)
            {
                load_image(images[i], cmdParser[i]);
                //cout << "Pic name: " << cmdParser[i] << endl;
            }//End of imageLoad for-loop

            //Init detector
            object_detector<image_scanner_type> detector;

            //Swap svm files
            for(unsigned int i = 0; i < files.size(); ++i)
            {
                set_current_dir(svmFolder);
                const char* const fileName = files[i].c_str();
                ifstream fin(fileName, ios::binary);
                set_current_dir(stepOut);
                //cout << "\n\nCWD in for-loop " << get_current_dir() << "\t\t Root? "<< myDir.is_root() << "\n\n" <<  endl;
                if(!fin)
                {
                    cout << "Could not find the SVM file!" << endl;
                    return EXIT_FAILURE;
                }//End of if(!fin)

                //Load the detector!
                deserialize(detector, fin);

                //Try to detect
                for(unsigned int j = 0; j < imagesArraySize; ++j)
                {
                    //try to detect!
                    const std::vector<rectangle> rects = detector(images[j]);

                    //Check if detector found something
                    if(rects.size() > 0)
                    {
                        // Object in image is detected! update "LagerStatus"
                        foundWith    = remove_extension(files[i]) + " in pic: " + cmdParser[j];
                        foundWith[0] = toupper(foundWith[0]);
                        itemName     = remove_extension(files[i]);
                        itemName[0]  = toupper(itemName[0]);

                        //Remove/Delete pics!
                        if(!deletePics)
                        {
                            deletePics = true;
                        }
                        break;
                    }
                    //Move the pics to unknown folder, if it's the last .svm file.
                    else if(i == files.size()-1 && rects.size() == 0)
                    {
                        // Could not detect any object in this Image. Move it to unkown!
                        picName = cmdParser[j];
                        found   = picName.find_last_of("/");
                        moveTo  = "mv " + cmdParser[j] + " " + unknownPicsPath;

                        //Linux, move file
                        const char * c = moveTo.c_str();
                        status         = system(c);

                        if(!objNotDetected)
                            objNotDetected = true;
                    }//End of else if
                }//End of try to detect for-loop
                if(deletePics)
                    break;
            }//End of SVM file swap for-loop

            /*const char* c = ".."; // back one level
            status  = chdir(c);
            cout << "\n\nCWD after SVM for loop: " << get_current_dir() << "\t\t Root? " << myDir.is_root() <<  "\n\n" <<  endl;*/

            //This is true if object was found in any pic
            if(deletePics)
            {
                json j;
                if(openJsonFile(inventoryFileName, j))
                {
                    if(findKeyInJson(itemName, j))
                    {
                        if(itemIn)
                        {
                            increaseValueByOneJson(itemName, j);
                        }
                        else if(itemOut)
                        {
                            decreaseValueByOneJson(itemName, j);
                        }
                    }
                    else
                        addNewItemJson(itemName, 1, j);

                    saveJsonToFile(inventoryFileName, j);
                }//End of if(openJsonFile)

                //Delete pics, for test we are moveing them to "dustbin" folder!
                for(unsigned int j = 0; j < cmdParser.number_of_arguments(); ++j)
                {
                    picName = cmdParser[j];
                    found   = picName.find_last_of("/");
                    //moveTo  = "mv " + picName + " " + get_current_dir() +"/dustbin/" + picName.substr(found+1); // Change "mv" to "rm", when testing is done
                    moveTo  = "rm " + picName;
                    //Linux, move file
                    const char * c = moveTo.c_str();
                    status         = system(c);
                }//End of for-loop
            }//End of if-deletePics

            if(objNotDetected)
            {
                cout << "There is new images in unknown folder!" << endl;
                unknownImagesHandling();
                // Somehow tell user about this!
            }
            else
            {
                cout << "No new images in unknown folder!" << endl;
            }

            cout << "Size of images-array: " << images.size() << endl;
            cout << "Found: " << foundWith << endl;

            //Stop time
            high_resolution_clock::time_point progStop = high_resolution_clock::now();
            //Count the time diff
            duration<double> time_span = duration_cast<duration<double>>(progStop - progStart);
            cout << "It took: " << time_span.count() << " seconds!" << endl;

            std::stringstream stringStream;

            stringStream << "It took: " << time_span.count() << " seconds! Ran program with: " << get_extension(cmdParser[0]) << endl;

            string log_msg = stringStream.str();

            write_text_to_log_file(log_msg);

            return EXIT_SUCCESS;
        }//End of number_of_arguments > 0 check

        if(cmdParser.number_of_arguments() == 0)
        {
            cout << "Need an argument!" << endl;
            return EXIT_FAILURE;
        }
    }//End of try-block
    catch(exception& e)
    {
        cout << "\nException thrown" << endl;
        cout << e.what() << endl;
    }
}//End of main()
