/**
* Main service thrift definition file.
*/

struct TImage {
    1: required list<byte> data;
    2: optional i16 width;
    3: optional i16 height;
}

struct TImageResponse {
    1: optional TImage result;
    2: optional string error;
}

service ImageService {
    /**
    * Check server health status
    * @return 0 if server is okay
    */
    byte ping(),

    TImageResponse resize(1:TImage request)
}