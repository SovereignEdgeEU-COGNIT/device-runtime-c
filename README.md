# Device-runtime
This repository holds the c implementation of the Device Runtime. The Device Runtime provides a SDK to enable the devices to communicate with the COGNIT platform to perform the task offloading. This component communicates with the Provisioning Engine to request/delete/update a Serverless Runtime and, then, it communicates with the provided Serverless Runtime to perform the offloading of functions and the uploading of content to the data-service.

## Building from sources
Ensure you have updated gcc and clang.
```sh
sudo apt update
sudo apt install gcc clang
```
`Dependencies: libCurl, CMake >= 3.5, GTest.`

**Without Sanitizer:**

	mkdir build && cd build && cmake .. && make
	
**With Sanitizer:**

    mkdir build && cd build && cmake .. -DSANITIZER="" && make

Note: ensure gtest sources are installed in your system to build the tests


## User's manual
### Configuration
The configuration for your Cognit Device Runtime can be added in minimal-offload-example.c where:

```c
    // Initialize the config for the serverless runtime context instance
    t_my_cognit_config.prov_engine_endpoint   = "http://localhost:8080";
    t_my_cognit_config.prov_engine_pe_usr     = "admin";
    t_my_cognit_config.prov_engine_pe_pwd     = "admin";
    t_my_cognit_config.prov_engine_port       = 8080;
```
`Note: Provisioning Engine Endpoint doesn't need "http://"`
### Examples
There are several folders that might be interesting for a user that is getting acquainted with Cognit:
In the `examples/` folder one can find the minimal example for running a minimal example making use of the Cognit module. To run it:

    ./examples/minimal-offload-example

### Tests
The `cognit/test/`  folder holds the tests. To run them:

    ./cognit/test/unit_tests