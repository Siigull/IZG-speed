/*!
 * @file
 * @brief This file contains class for command line parsing.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#pragma once

#include <ArgumentViewer/ArgumentViewer.h>
#include <iostream>
#include <string>


/**
 * @brief This class parses command line arguments
 */
class Arguments{
  public:
    Arguments(){}
    /**
     * @brief Constructor
     *
     * @param argc number of arguments
     * @param argv[] arguments
     */
    Arguments(int argc,char*argv[]);

    std::shared_ptr<argumentViewer::ArgumentViewer>args                                       ; ///< argument viewer
    std::vector<int32_t>                           windowSize                                 ; ///< window size
    std::string                                    modelFile           = "../tests/model.glb" ; ///< models file
    std::string                                    imageFile           = "../test/image.jpg"  ; ///< image file
    uint32_t                                       method              = 0                    ; ///< start with this method
    bool                                           runPerformanceTests                        ; ///< should we run performance tests
    bool                                           runConformanceTests                        ; ///< sould we run conformance tests
    bool                                           takeScreenShot                             ; ///< should we take a screnshot
    bool                                           stop                = false                ; ///< should we immediately stop
    bool                                           getNofExamples      = false                ; ///< should the application print the number of examples
    uint32_t                                       perfTests                                  ; ///< number of frames in performance tests
    int                                            selectedTest                               ; ///< selected conformance test
    bool                                           upToTest                                   ; ///< run tests up to selected test
    float                                          mseThreshold                               ; ///< threshold for image test
    int32_t                                        testToBreak                                ; ///< if you want to forcefully break test, set it to test id
    int32_t                                        ignoreUpToTest                             ; ///< if you want to ignore tests, set this to first test you do not want to ignore
    int32_t                                        verboseMemoryOutput                        ; ///< if you want to see deep information about memory during test, set this to true
    size_t                                         idToBreak                                  ; ///< internal usage
    size_t                                         lineToBreak                                ; ///< internal usage
};

