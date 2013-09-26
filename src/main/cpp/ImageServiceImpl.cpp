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
    if (request.data.size() < 1) {
        _return.__set_error("Invalid data");
        return;
    }
    cout << "resize request, w:"<< request.width << ", h:" << request.height <<"\n";
    l_uint8* data = (l_uint8*)&request.data[0];
    PIX* pix = NULL;
    try {
        pix = pixReadMem(data, request.data.size());
    } catch(const std::exception& e) {
        cerr<<"Error in reading image\n";
        return;
    } catch(...) {
        cerr<<"Unknown error in reading image\n";
        return;
    }
    if (pix != NULL) {
        cout<< "Resizing image\n";
        l_float32 pw = pixGetWidth(pix);
        l_float32 ph = pixGetHeight(pix);
        l_float32 s = min(request.width / pw, request.height / ph);
        PIX* scaledPix = pixScale(pix, s, s); 
        if (scaledPix != NULL) {
            l_uint8* bytes = NULL;
            size_t size = 0;
            
            if (0 == pixWriteMem(&bytes, &size, scaledPix, pix->informat)) {
                TImage result;
                result.__set_width(scaledPix->w);
                result.__set_height(scaledPix->h);
                std::vector<int8_t> v(bytes, bytes+size);
                result.__set_data(v);
                _return.__set_result(result);
                cout<<"new size:"<< size <<", original size:"<< request.data.size() <<"\n";
            }
            pixDestroy(&scaledPix);
        }        
        pixDestroy(&pix);
    } else {
        _return.__set_error("Could not create image from data");
    }
}


