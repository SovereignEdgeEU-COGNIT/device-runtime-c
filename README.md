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
In the `examples/` folder one can find the minimal example making use of the Cognit module. To run it:

    ./examples/minimal-offload-example

### Configuration
The configuration to connect to the Cognit Frontend is set in a structure in the main file:

```c
    cognit_config_t t_config = {
        .cognit_frontend_endpoint   = "https://cognit-lab-frontend.sovereignedge.eu",
        .cognit_frontend_usr        = "",
        b.cognit_frontend_pwd        = "",
    };
```

### Requirements
The requirements of the application are also defined in a structure in the main file:

```c
    scheduling_t app_reqs = {
        .flavour                     = "FaaS_generic_V2",
        .max_latency                 = 100,
        .max_function_execution_time = 3.5,
        .min_renewable               = 85,
        .geolocation                 = "IKERLAN ARRASATE/MONDRAGON 20500"
    };
```

`Note: This client requires the flavour of the app requirements to be "Faas_generic_V2". `

### HTTP communication
The HTTP requests are performed from the my_http_send_cb() function, which is called by the Cognit module when a request shall be done. The function receives the buffer to send and the rest of the needed parameters to configure the request. 

```c
    int my_http_send_req_cb(const char* c_buffer, size_t size, http_config_t* config)
        {
            ...
        }
```

The minimal example implements this function using libcurl, but users can implement this function with the desired HTTP implementation. Just make your implementation of the function and modify the examples/CMakeLists.txt file to remove the linking of libcurl and add your library.