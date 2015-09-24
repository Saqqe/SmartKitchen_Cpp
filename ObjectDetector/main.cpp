#include <dlib/svm_threaded.h>
#include <dlib/string.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_processing.h>
#include <dlib/data_io.h>
#include <dlib/cmd_line_parser.h>

#include <sys/types.h>
#include <dirent.h>
#include <string>
#include <errno.h>
#include <iostream>


using namespace std;
using namespace dlib;


const char * const svmPath = "/home/saqib/DevFolder/ownDev/SmartKitchen_Cpp/ObjectDetector/SVMFolder";
int status;

/**
Check given directory for svm files
save the names of the .svm files in the vector!
**/
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

int main(int argc, char** argv)
{
    try
    {
        command_line_parser cmdParser;
        cmdParser.parse(argc, argv);

        typedef scan_fhog_pyramid<pyramid_down<6> > image_scanner_type;

        if(cmdParser.number_of_arguments() > 0)
        {
            //Init
            string picName        = "";
            string saveTO         = "";
            string unKownPicsPath = "/home/saqib/ShareWithWindows/UnkownPics/";
            int objFoundCounter   = 0;
            size_t found;


            //Load SVM files!
            string dir                     = "/home/saqib/DevFolder/ownDev/SmartKitchen_Cpp/ObjectDetector/SVMFolder";
            std::vector<std::string> files = std::vector<std::string>();
            getSVMFiles(dir,files);

            //Test for-loop
            for(unsigned int i = 0; i < files.size(); ++i)
            {
                cout << files[i] << endl;
            }



            //Load images
            dlib::array<array2d<unsigned char> > images;
            images.resize(cmdParser.number_of_arguments());
            for(unsigned int i = 0; i < images.size(); ++i)
            {
                load_image(images[i], cmdParser[i]);
            }//End of imageLoad for-loop

            //Init detector
            object_detector<image_scanner_type> detector;


            //Swap svm files
            for(unsigned int i = 0; i < files.size(); ++i)
            {
                //Load a svm file
                status = chdir(svmPath);
                const char* const fileName = files[i].c_str();
                ifstream fin(fileName, ios::binary);
                if(!fin)
                {
                    cout << "Could not find the file!" << endl;
                    return EXIT_FAILURE;
                }//End of if(!fin)

                //Load the detector!
                deserialize(detector, fin);

                //Try to detect
                for(unsigned int j = 0; j < images.size(); ++j)
                {
                    //try to detect!
                    const std::vector<rectangle> rects = detector(images[j]);

                    //Check if detector found something
                    if(rects.size() > 0)
                    {
                        /**
                        Object detected on THIS image with current SVMFile.
                        Do something with this info!
                        **/

                        cout << "Found obj in: " << cmdParser[j] << " With: " << files[i] << endl;

                    }//End of if
                    /**
                    If last SVM file and no Obj detected, then move this image to "Unkown" folder!
                    **/
                    else if(i == files.size()-1 && rects.size() == 0)
                    {
                        /**
                        picName = cmdParser[j];
                        found   = picName.find_last_of("/");
                        saveTO  = "mv " + picName + " " + unKownPicsPath + picName.substr(found+1);


                        //Linux, move file
                        const char * c = saveTO.c_str();
                        status         = system(c);
                        **/
                        cout << "Moveing!" << files[i] << endl;

                    }//End of Else-if
                    else
                    {
                        cout << "Did not found with: " << files[i] << endl;
                    }

                }//End of try to detect for-loop
            }//End of SVM file swap for-loop


            /*
            for (unsigned int i = 0;i < files.size();i++)
            {
                status = chdir(svmPath);
                const char* const fileName = files[i].c_str();
                ifstream fin(fileName, ios::binary);
                if(!fin)
                {
                    cout << "Could not find the file!" << endl;
                    return EXIT_FAILURE;
                }

                object_detector<image_scanner_type> detector;
                deserialize(detector, fin);

                dlib::array<array2d<unsigned char> > images;

                int counter = 0;

                string picName        = "";
                string saveTO         = "";
                string unKownPicsPath = "/home/saqib/ShareWithWindows/UnkownPics/";

                //Load images!
                images.resize(cmdParser.number_of_arguments());
                for(unsigned long j = 0; j < images.size(); ++j)
                {
                    load_image(images[j], cmdParser[j]);

                    // Run the detector on images[j]
                    const std::vector<rectangle> rects = detector(images[j]);

                    //cout << "\nNumber of detections: "<< rects.size() << endl;
                    if(rects.size() > 0)
                    {
                        counter++;
                    }
                    else
                    {
                        picName = cmdParser[j];
                        found   = picName.find_last_of("/");
                        saveTO  = "mv " + picName + " " + unKownPicsPath + picName.substr(found+1);


                        //Linux, move file
                        const char * c = saveTO.c_str();
                        status         = system(c);


                        cout << picName.substr(found+1) << endl;
                        cout << saveTO << endl;

                    }
                }//End of for-loop, detector!
                cout << files[i];
                cout << "\n" << counter << "/" << images.size() << endl;
            }//End of for-loop, svmFile
            */
            return EXIT_SUCCESS;
        }//End of number_of_arguments > 0 check

        if(cmdParser.number_of_arguments() == 0)
        {
            return EXIT_FAILURE;
        }

    }//End of try-block
    catch(exception& e)
    {
        cout << "\nException thrown" << endl;
        cout << e.what() << endl;
        cout << "\nTry the -h option! " << endl;
    }
}//End of main()
