#include "build/utils/easy_image.h"
#include "build/utils/ini_configuration.h"
#include "build/Intro.h"
#include "build/LSystems/2DLSystem.h"
#include "build/3DLines.h"
#include "build/3DTriangles.h"
#include "build/Light.h"

#include <iostream>
#include <string>
#include <chrono>

img::EasyImage generate_image(const ini::Configuration &configuration) {
    const std::string type = configuration["General"]["type"].as_string_or_die();

    if (type == "IntroColorRectangle") return intro::color_rectangle(configuration);
    else if (type == "IntroBlocks") return intro::generate_blocks(configuration);
    else if (type == "IntroLines") return intro::generate_lines(configuration);
    else if (type == "2DLSystem") return LSystem2D::LSystem2D(configuration);
    else if (type == "Wireframe") return Lines3D::wireframe(configuration, false);
    else if (type == "ZBufferedWireframe") return Lines3D::wireframe(configuration, true);
    else if (type == "ZBuffering") return Triangles3D::zBuffer(configuration);
    else if (type == "LightedZBuffering") return Light3D::lightedZBuffering(configuration);
    return {};
}



int main(int argc, char const* argv[]) {
        int retVal = 0;
        try
        {
                std::vector<std::string> args = std::vector<std::string>(argv+1, argv+argc);
                if (args.empty()) {
                        std::ifstream fileIn("filelist");
                        std::string filelistName;
                        while (std::getline(fileIn, filelistName)) {
                                args.push_back(filelistName);
                        }
                }
                auto start = std::chrono::high_resolution_clock::now();
                for(std::string fileName : args)
                {
                    ini::Configuration conf;
                    try {
                        std::ifstream fin(fileName);
                        fin >> conf;
                        fin.close();
                    }
                    catch (ini::ParseException &ex) {
                        std::cerr << "Error parsing file: " << fileName << ": " << ex.what() << std::endl;
                        retVal = 1;
                        continue;
                    }
                    img::EasyImage image = generate_image(conf);
                    if (image.get_height() > 0 && image.get_width() > 0) {
                        std::string::size_type pos = fileName.rfind('.');
                        if (pos == std::string::npos) {
                            //filename does not contain a '.' --> append a '.bmp' suffix
                            fileName += ".bmp";
                        } else {
                            fileName = fileName.substr(0, pos) + ".bmp";
                        }
                        try {
                            std::ofstream f_out(fileName.c_str(),
                                                std::ios::trunc | std::ios::out | std::ios::binary);
                            f_out << image;

                        }
                        catch (std::exception &ex) {
                            std::cerr << "Failed to write image to file: " << ex.what() << std::endl;
                            retVal = 1;
                        }
                    } else {
                        std::cout << "Could not generate image for " << fileName << std::endl;
                    }
                }
                auto end = std::chrono::high_resolution_clock::now();
                auto time = end-start;
                auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(time);
                std::cout << "Time taken to generate images: " << (double) ms.count()/1000.0 << " seconds." <<std::endl;
        }
        catch(const std::bad_alloc &exception)
        {
    		//When you run out of memory this exception is thrown. When this happens the return value of the program MUST be '100'.
    		//Basically this return value tells our automated test scripts to run your engine on a pc with more memory.
    		//(Unless of course you are already consuming the maximum allowed amount of memory)
    		//If your engine does NOT adhere to this requirement you risk losing points because then our scripts will
		    //mark the test as failed while in reality it just needed a bit more memory
                std::cerr << "Error: insufficient memory" << std::endl;
                retVal = 100;
        }
        return retVal;
}
