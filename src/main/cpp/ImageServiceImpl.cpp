/* 
 * File:   ImageServiceImpl.cpp
 * Author: mamad
 * 
 * Created on 22 September 2013, 23:09
 */

#include <stdlib.h>
#include "ImageServiceImpl.h"
#include "service_types.h"

#if !defined(LEPTONICA_ALLHEADERS_H)
#   include <leptonica/allheaders.h>
#include <string.h>
#endif

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
    if ( 1 == 1) {
        _return.error = std::string("Just for test");
        cout << _return.error << " <<\n";
        return;
    }
    if (request.data.length() < 1) {
        _return.error = "Invalid data";
        return;
    }
    cout << "resize request, w:"<< request.width << ", h:" << request.height <<"\n";
    l_uint8* data = (l_uint8*)request.data.data();
    PIX* pix = pixReadMem(data, request.data.length());
    if (pix != NULL) {
        cout<< "Resizing image\n";
        l_float32 pw = pixGetWidth(pix);
        l_float32 ph = pixGetHeight(pix);
        l_float32 s = min(request.width / pw, request.height / ph);
        PIX* scaledPix = pixScale(pix, s, s); 
        if (scaledPix != NULL) {
            l_uint8 *bytes = NULL;
            size_t size = 0;
            
            if (0 == pixWriteMem(&bytes, &size, scaledPix, pix->informat)) {
                _return.result.width = scaledPix->w;
                _return.result.height = scaledPix->h;
                _return.result.data.assign((char*)bytes, size);
                _return.error= "No Error";
                cout<<"new size:"<<_return.result.data.length()<<", original size:"<< request.data.length() <<"\n";
            }
            pixDestroy(&scaledPix);
        }        
        pixDestroy(&pix);
    } else {
        _return.error = "Could not create image from data";
    }
}


