# Device-runtime
This repository holds the c implementation of the new architecture for Device Runtime. The Device Runtime provides a SDK to enable the devices to communicate with the COGNIT platform to perform the task offloading. This component communicates with the Cognit Frontend to send the app requirements and get the endpoint of an Edge Cluster. Then, it communicates with the provided Edge Cluster to perform the offloading of functions and the uploading of content to the data-service.

## Building from sources
Ensure you have updated gcc and clang.
```sh
sudo apt update
sudo apt install gcc clang
```

There are different points to compile using CMake:

**Without Sanitizer:**

	mkdir build && cd build && cmake .. && make
	
**With Sanitizer:**

    mkdir build && cd build && cmake .. -DSANITIZER="" && make

`Note:` By default the example program is compiled, ensure libCurl is installed in your system to build the example. If you only want to compile the library use:

    cmake -DCOGNIT_BUILD_EXAMPLES=OFF ..

**To compile with your own toolchain**

In order to compile the project with a custom toolchain (for Arm systems, for example), you will need to add to the main CMake the follows:
    
    set(CMAKE_SYSTEM_NAME custom_arm)
    set(CMAKE_SYSTEM_PROCESSOR arm)
    set(CMAKE_C_COMPILER <path_to_toolchain_gcc_bin>)
    set(CMAKE_CXX_COMPILER <path_to_toolchain_g++_bin>)
    set(CMAKE_SYSROOT path_to_toolchain)
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)


## User's manual
### Examples
In the `examples/` folder one can find the minimal example for running a minimal example making use of the Cognit module. To run it:

    ./examples/minimal-offload-example

### Configuration
The configuration to connect to the Cognit Frontend is set in the cognit-template.yml file:

```c
    api_endpoint: "https://cognit-lab-frontend.sovereignedge.eu" # no port needed
credentials: "your_username:your_password"
local_endpoint: "localhost:5555"

```
`Note: The "local_endpoint" field is used only for local testing purposes. If this field is not included (or is commented out), the Device Runtime will offload the functions to the Edge Cluster provided by the Cognit Frontend. Otherwise, the Device Runtime will connect to the Cognit Frontend but, afterward, will ignore the provided Edge Cluster and offload (or call) the functions to the local endpoint specified in the configuration file. This requires a Serverless Runtime to be running on the specified endpoint.`


