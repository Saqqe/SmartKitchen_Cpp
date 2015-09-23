#include <dlib/svm_threaded.h>
#include <dlib/string.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_processing.h>
#include <dlib/data_io.h>
#include <dlib/cmd_line_parser.h>


#include <iostream>
#include <fstream>


using namespace std;
using namespace dlib;


//const char * const svmPath = "/home/saqib/DevFolder/ownDev/SmartKitchen_Cpp/ObjectDetector/SVMFolder";


int main(int argc, char** argv)
{
    try
    {
        command_line_parser cmdParser;
        cmdParser.parse(argc, argv);

        typedef scan_fhog_pyramid<pyramid_down<6> > image_scanner_type;

        if(cmdParser.number_of_arguments() > 0)
        {
            ifstream fin("milk.svm", ios::binary);
            if(!fin)
            {
                cout << "Could not find the file!" << endl;
                return EXIT_FAILURE;
            }

            object_detector<image_scanner_type> detector;
            dlib::array<array2d<unsigned char> > images;
            deserialize(detector, fin);

            int counter = 0;
            int result;
            string picName = "";
            string saveTO = "";
            string unKownPicsPath = "/home/saqib/ShareWithWindows/UnkownPics/";
            //Load images!
            images.resize(cmdParser.number_of_arguments());
            for(unsigned long i = 0; i < images.size(); ++i)
            {
                load_image(images[i], cmdParser[i]);

                // Run the detector on images[i]
                const std::vector<rectangle> rects = detector(images[i]);
                //cout << "\nNumber of detections: "<< rects.size() << endl;
                if(rects.size() > 0)
                {
                    counter++;
                }
                else
                {
                    picName = cmdParser[i];
                    size_t found = picName.find_last_of("/");

                    saveTO = unKownPicsPath + picName.substr(found+1);



                    cout << picName.substr(found+1) << endl;
                    cout << saveTO << endl;
                }
            }//End of for-loop
            cout << "\n" << counter << "/" << images.size() << endl;
        return EXIT_SUCCESS;
        }//End of number_of_arguments > 0 check

        if(cmdParser.number_of_arguments() == 0)
        {
            return EXIT_FAILURE;
        }

    }
    catch(exception& e)
    {
        cout << "\nException thrown" << endl;
        cout << e.what() << endl;
        cout << "\nTry the -h option! " << endl;
    }

}
