/**
    Main program for object recognition
    main.cpp
    Project grupp: Edlund&Sarker

    @author Saqib Sarker
    @version 0.1 2015-10-06
*/
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

using namespace std;
using namespace dlib;
using json = nlohmann::json;


//const char * const svmPathTEST = "/home/saqib/DevFolder/ownDev/SmartKitchen_Cpp/ObjectDetector/SVMFolder"; // Need to fix THIS PATH!
int status;

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
}

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
}

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
}

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
    cout << "FileName is: " << fileName << endl;
    std::ifstream file(fileName);
    if(file)
    {
        cout << "File found" << endl;
        j = json::parse(file);
        cout << j.dump(4) << endl; // Testing
        file.close();
        return true;
    }
    else
    {
        cout << "File not found" << endl;
        return false;
    }
}

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
        cout << "Saveing this: \n\n" << j.dump(4) << endl; // Testing
        outFile << j.dump(4) << endl;

        outFile.close();
        return true;
    }
    else return false;
}

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
}

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
        Only for testing

        if(cmdParser.option("test"))
        {
            string svmFolder = "SVMFolder";
            directory test(".");

            cout << "\n\ndirectory: " << test.name() << endl;
            cout << "full path: " << test.full_name() << endl;
            cout << "is root:   " << ((test.is_root())?"yes":"no") << endl;

            // get all directories and files in test
            std::vector<directory> dirs = test.get_dirs();
            std::vector<file> files = test.get_files();

            // sort the files and directories
            sort(files.begin(), files.end());
            sort(dirs.begin(), dirs.end());


            cout << "\n\n\n";

            // print all the subdirectories
            for (unsigned long i = 0; i < dirs.size(); ++i)
            {
                cout << "        <DIR>    " << dirs[i].name() << "\n";

                if(svmFolder.compare(dirs[i].name()) == 0)
                {
                    set_current_dir(dirs[i].name());
                    cout << "Current dir: " << get_current_dir() << endl;

                    //Load SVM files!
                    //std::string dir(svmPath);
                    std::vector<std::string> files = std::vector<std::string>();
                    getSVMFiles(get_current_dir(),files);

                    //Test loop
                    for(unsigned int j = 0; j < files.size(); ++j)
                    {
                        cout << files[j] << " j:" << j <<  endl;
                    }
                    cout << files.size() << endl;

                    set_current_dir("..");//Back on level
                }
            }
            // print all the subfiles
            for (unsigned long i = 0; i < files.size(); ++i)
                cout << setw(13) << files[i].size() << "    " << files[i].name() << "\n";


            cout << "\n\nAt the end!! Current dir: " << get_current_dir() << endl;

            return EXIT_SUCCESS;
        }*/

        if(cmdParser.number_of_arguments() > 0)
        {
            //Init variables
            string picName              = "";
            string moveTo               = "";
            //string svmPath              = get_current_dir() +"/SVMFolder";
            string svmFolder            = "SVMFolder";
            string inventoryFileName    = "Inventory.json";
            string unKownPicsPath       = get_current_dir() + "/UnkownPics"; // Need to fix THIS PATH!!
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

            cout << "\n\ndirectory: " << myDir.name() << endl;
            cout << "full path: " << myDir.full_name() << endl;
            cout << "is root:   " << ((myDir.is_root())?"yes":"no") << endl;

            //Fetch SVM files
            //std::string dir(svmPath);
            std::vector<directory> dirs = myDir.get_dirs(); //Fetch all the dirs
            sort(dirs.begin(), dirs.end());
            for(unsigned int i = 0; i < dirs.size(); ++i)
            {
                cout << "Dirs: " << dirs[i].name() << endl;
                if(svmFolder.compare(dirs[i].name()) == 0)
                {
                    set_current_dir(svmFolder);//Setp into svmFolder

                    getSVMFiles(get_current_dir() ,files);

                    set_current_dir(stepOut); //Back out one level
                }
            }//End of for-loop


            //Test loop
            /*for(unsigned int i = 0; i < files.size(); ++i)
            {
                cout << files[i] << " i:" << i <<  endl;
            }
            cout << files.size() << endl;*/

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


            cout << "Itemin: " << itemIn << ", itemout: " << itemOut << endl;

            //Load images
            dlib::array<array2d<unsigned char> > images;
            images.resize(cmdParser.number_of_arguments());
            unsigned int imagesArraySize = images.size();
            //imagesDetected.resize(cmdParser.num_of_arguments());
            for(unsigned int i = 0; i < imagesArraySize; ++i)
            {
                /**
                * TODO: Load images, latest first!
                */
                load_image(images[i], cmdParser[i]);
                //cout << "Pic name: " << cmdParser[i] << endl;
            }//End of imageLoad for-loop


            //Init detector
            object_detector<image_scanner_type> detector;



            //Swap svm files
            for(unsigned int i = 0; i < files.size(); ++i)
            {
                //Load a svm file from the "svmPath" and
                /*const char* command        = svmPath.c_str();
                status                     = chdir(command); // Change to svm folder - down on level
                const char* const fileName = files[i].c_str();*/
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
                    //Move the pics to unkown folder, if it's the last .svm file.
                    else if(i == files.size()-1 && rects.size() == 0)
                    {
                        // Could not detect any object in this Image. Move it to unkown!
                        picName = cmdParser[j];
                        found   = picName.find_last_of("/");
                        moveTo  = "mv " + cmdParser[j] + " " + unKownPicsPath;

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

            //This is true if obj was found in any pic
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
                    moveTo  = "mv " + picName + " " + get_current_dir() +"/dustbin/" + picName.substr(found+1); // Change "mv" to "rm", when testing is done
                    //moveTo  = "rm " + picName;
                    //Linux, move file
                    const char * c = moveTo.c_str();
                    status         = system(c);
                }//End of for-loop
            }//End of if-deletePics

            if(objNotDetected)
            {
                cout << "There is new images in unkwon folder!" << endl;
                // Somehow tell user about this!
            }
            else
            {
                cout << "There is NO new images in unkwon folder!" << endl;
            }

            cout << "Size of images-array: " << images.size() << endl;
            cout << "Found: " << foundWith << endl;
            return EXIT_SUCCESS;
        }//End of number_of_arguments > 0 check

        if(cmdParser.number_of_arguments() == 0)
        {
            cout << "Need a argument!" << endl;
            return EXIT_FAILURE;
        }
    }//End of try-block
    catch(exception& e)
    {
        cout << "\nException thrown" << endl;
        cout << e.what() << endl;
    }
}//End of main()
