# Device-runtime
This repository holds the c implementation of the Device Runtime. The Device Runtime provides a SDK to enable the devices to communicate with the COGNIT platform to perform the task offloading. This component communicates with the Provisioning Engine to request/delete/update a Serverless Runtime and, then, it communicates with the provided Serverless Runtime to perform the offloading of functions and the uploading of content to the data-service.

## Building from sources

`TODO`

	mkdir build && cd build && cmake .. && make

Note: ensure gtest sources are installed in your system to build the tests


## User's manual
### Configuration
The configuration for your Cognit Device Runtime can be found in `cognit/test/config/cognit.yml`, with an example for running the tests.

### Examples
There are several folders that might be interesting for a user that is getting acquainted with Cognit:
In the `examples/` folder one can find the minimal example for running a minimal example making use of the Cognit module.

### Tests
The `cognit/test/`  folder holds the tests.