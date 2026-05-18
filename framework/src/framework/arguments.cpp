#include<framework/arguments.hpp>

#ifndef CMAKE_ROOT_DIR
/**
 * location of project
 */
#define CMAKE_ROOT_DIR ".."
#endif

Arguments::Arguments(int argc,char*argv[]){
  args = std::make_shared<argumentViewer::ArgumentViewer>(argc,argv);


  auto defaultModel = std::string(CMAKE_ROOT_DIR)+"/resources/models/fin.glb";
  auto defaultImage = std::string(CMAKE_ROOT_DIR)+"/resources/images/onceAliveNowForgottenAndDead.png";

  windowSize          = args->geti32v  ("--window-size"        ,{500,500}   ,"size of the window"                                                );
  runPerformanceTests = args->isPresent("-p"                                ,"runs performance tests"                                            );
  runConformanceTests = args->isPresent("-c"                                ,"runs conformance tests"                                            );
  selectedTest        = args->geti32   ("--test"               ,-1          ,"run only this selected test"                                       );
  takeScreenShot      = args->isPresent("-s"                                ,"takes screenshot of app"                                           );
  getNofExamples      = args->isPresent("--getNofExamples"                  ,"returns number of examples"                                        );
  upToTest            = args->isPresent("--up-to-test"                      ,"run all tests up to selected test by --test argument"              );
  method              = args->getu32   ("--method"             ,0           ,"selects a rendering method"                                        );
  modelFile           = args->gets     ("--model"              ,defaultModel,"model file in gltf/glb format"                                     );
  imageFile           = args->gets     ("--img"                ,defaultImage,"texture file for texturedQuadMethod"                               );
  perfTests           = args->getu32   ("-f"                   ,10          ,"number of frames that are tests during performance tests"          );
  mseThreshold        = args->getf32   ("--mse"                ,40          ,"mse threshold for image to image test"                             );
  testToBreak         = args->geti32   ("--breakTest"          ,-1          ,"this will forcefully break test with this number"                  );
  ignoreUpToTest      = args->geti32   ("--ignoreUpToTest"     ,-1          ,"this will tell project to ignore test with number < then specified");
  verboseMemoryOutput = args->geti32   ("--verboseMemoryOutput",0           ,"this will force test to print deep memory informations"            );
  idToBreak           = args->getu32   ("--idToBreak"          ,1000000     ,"internal usages (used to test the tests...)"                       );
  lineToBreak         = args->getu32   ("--lineToBreak"        ,1234567     ,"internal usages (used to test the tests...)"                       );
  



  auto printHelp  = args->isPresent("-h"    ,"prints help");
  printHelp |= args->isPresent("--help","prints help");

  if(printHelp || !args->validate()){
    std::cerr << args->toStr() << std::endl;
    stop = true;
  }

}
