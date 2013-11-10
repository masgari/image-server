/**
* Main service thrift definition file.
*/

struct TDimension {
    1: required i16 width;
    2: required i16 height;
}

struct TImage {
    1: required list<byte> data;
    2: optional TDimension dimension;
}

struct TImageResponse {
    1: optional TImage result;
    2: optional TDimension originalDimension;
    3: optional string error;
}

service ImageService {
    /**
    * Check server health status
    * @return 0 if server is okay
    */
    byte ping(),

    TImageResponse resize(1:TImage request),
    
    /**
    * Convert input image to cartoon
    */
    TImageResponse cartoonize(1:TImage request)
}