/* 
 * File:   ImageServiceImpl.cpp
 * Author: mamad
 * 
 * Created on 22 September 2013, 23:09
 */

#include <stdlib.h>
#include "ImageServiceImpl.h"

using namespace std;

ImageServiceImpl::ImageServiceImpl(const int threads) {
}

ImageServiceImpl::~ImageServiceImpl() {
}

int8_t ImageServiceImpl::handlePing() {
    cout << "ping\n";
    return 0;
}

void ImageServiceImpl::handleResize(TImageResponse& _return, const TImage& request) {
    cout << "resize, w:"<< request.width << ", h:" << request.height <<"\n";
}


