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
#include <leptonica/allheaders.h>
#include <string.h>
#endif

using namespace std;

static const char* BLUR_KERNEL = " 5 10 20 10 5 " " 10 20 50 20 10 " " 10 70 140 70 10 " " 10 20 50 20 10 " " 5 10 20 10 5 ";

//static const char* BLUR_KERNEL_3 = " 20 50 20 " " 70 140 70 " " 20 50 20 ";
static const char* BLUR_KERNEL_3 = " 10 10 10 " " 10 10 10 " " 10 10 10 ";
//static const char* BLUR_KERNEL_3 = " 51 51 51 " " 51 51 51 " " 51 51 51 ";

PIX* pixEdgeFilter(PIX* pixs, l_int32 orientflag);
PIX* readPixRequest(TImageResponse& _return, const TImage& request);

ImageServiceImpl::ImageServiceImpl(const int threads) {
}

ImageServiceImpl::~ImageServiceImpl() {
}

int8_t ImageServiceImpl::handlePing() {
    cout << "ping\n";
    return 0;
}

void ImageServiceImpl::handleResize(TImageResponse& _return, const TImage& request) {
    if (request.dimension.width < 1 || request.dimension.height < 1)  {
        _return.__set_error("Invalid resize dimension");
        return;
    }

    PIX* pix = readPixRequest(_return, request);
    if (pix == NULL) {
        _return.__set_error("Could not create image from data");
        return;
    }
    l_float32 pw = pixGetWidth(pix);
    l_float32 ph = pixGetHeight(pix);
    l_float32 s = min(request.dimension.width / pw, request.dimension.height / ph);
    //cout << "Resizing image"<<"w:"<<pw<<", h:"<<ph<<", ratio:"<<s<<"\n";
    PIX* scaledPix = pixScale(pix, s, s);
    if (scaledPix != NULL) {
        l_uint8* bytes = NULL;
        size_t size = 0;

        if (0 == pixWriteMem(&bytes, &size, scaledPix, pix->informat)) {
            TImage result;

            TDimension resizeDimension;
            resizeDimension.__set_width(scaledPix->w);
            resizeDimension.__set_height(scaledPix->h);
            result.__set_dimension(resizeDimension);
            //cout<<resizeDimension.width<<", h:"<<resizeDimension.height<<"\n";
            
            std::vector<int8_t> v(bytes, bytes + size);
            result.__set_data(v);

            _return.__set_result(result);

            TDimension originalDimension;
            originalDimension.__set_width(pw);
            originalDimension.__set_height(ph);
            _return.__set_originalDimension(originalDimension);            
            //cout << "new bytes:" << size << ", original bytes:" << request.data.size() << "\n";
        }
        pixDestroy(&scaledPix);
    }
    pixDestroy(&pix);
}

void ImageServiceImpl::handleCartoonize(TImageResponse& _return, const TImage& request) {
    if (request.dimension.width < 1 || request.dimension.height < 1)  {
        _return.__set_error("Invalid resize dimension");
        return;
    }
    PIX* pix = readPixRequest(_return, request);
    if (pix == NULL) {
        _return.__set_error("Could not create image from data");
        return;
    }
    cout << "Cartoonizing image\n";
    cout << "First resizing image\n";

    l_float32 pw = pixGetWidth(pix);
    l_float32 ph = pixGetHeight(pix);
    l_float32 s = min(request.dimension.width / pw, request.dimension.height / ph);
    PIX* scaled = pixScale(pix, s, s);
    l_int32 informat = pix->informat;
    pixDestroy(&pix);
    pix = NULL;
    printf("1.Image dimensions: (w:%d,h:%d,depth:%d)\n", scaled->w, scaled->h, scaled->d);

    PIX* gray;
    if (scaled->d > 8) {
        gray = pixConvertRGBToGrayMinMax(scaled, L_CHOOSE_MAX);
        if (!gray) {
            printf("Error in converting to grayscale\n");
            pixDestroy(&scaled);
            return;
        } else {
            //pixWrite("gray-step2.jpg", gray, IFF_JFIF_JPEG);
            printf("2.Converted to grayscale.\n");
        }

    }

    //edge detection
    //PIX* edges = pixSobelEdgeFilter(gray,  L_ALL_EDGES);
    PIX* edges = pixEdgeFilter(gray, L_ALL_EDGES);
    //PIX* edges = pixTwoSidedEdgeFilter(gray,  L_ALL_EDGES);

    if (!edges) {
        printf("Error in detecting edges\n");
        pixDestroy(&gray);
        pixDestroy(&scaled);
        return;
    }
    pixDestroy(&gray);

    //pixWrite("edges-step3.jpg", edges, IFF_JFIF_JPEG);
    printf("3.Edges detected.\n");

    PIX* inverted = pixInvert(NULL, edges);
    //pixWrite("inverted-step4.jpg", inverted, IFF_JFIF_JPEG);
    printf("4.Edges inverted.\n");
    pixDestroy(&edges);

    //no direct bluring function, so create a bluring kernel first
    L_KERNEL* bKernel = kernelCreateFromString(5, 5, 2, 2, BLUR_KERNEL);
    //L_KERNEL* bKernel = kernelCreateFromString(3,3,1,1,BLUR_KERNEL_3);
    if (!bKernel) {
        printf("Could not create blur kernel\n");
        return;
    }
    PIX* blur = pixConvolve(inverted, bKernel, 8, 1);
    kernelDestroy(&bKernel);
    //pixWrite("blur-step5.jpg", blur, IFF_JFIF_JPEG);
    printf("5.Edges blured.\n");
    //PIX* bOrig = pixRead("5.jpg");
    //PIX* cartoon = pixBlendHardLight(NULL, pix, bOrig, 0, 0, 150/255.0);
    //pixDestroy(&bOrig);
    PIX* cartoon = pixBlendHardLight(NULL, scaled, blur, 0, 0, 150 / 255.0);
    if (cartoon != NULL) {
        //pixWrite("cartoon-step6.jpg", cartoon, IFF_JFIF_JPEG);
        printf("6.Blending done.\n");
        
        //return results
        l_uint8* bytes = NULL;
        size_t size = 0;
        if (0 == pixWriteMem(&bytes, &size, cartoon, informat)) {
            TImage result;

            TDimension resizeDimension;
            resizeDimension.__set_width(cartoon->w);
            resizeDimension.__set_height(cartoon->h);
            result.__set_dimension(resizeDimension);

            std::vector<int8_t> v(bytes, bytes + size);
            result.__set_data(v);

            _return.__set_result(result);

            TDimension originalDimension;
            originalDimension.__set_width(pw);
            originalDimension.__set_height(ph);
            _return.__set_originalDimension(originalDimension);            
            
            cout << "cartoon size:" << size << ", original size:" << request.data.size() << "\n";
        }
        pixDestroy(&cartoon);
    }
    
    pixDestroy(&blur);
    pixDestroy(&inverted);
    pixDestroy(&scaled);
}

PIX* readPixRequest(TImageResponse& _return, const TImage& request) {
    if (request.data.size() < 1) {
        _return.__set_error("Invalid data");
        return NULL;
    }
    //cout << "request image w:" << request.dimension.width << ", h:" << request.dimension.height << "\n";
    l_uint8* data = (l_uint8*) & request.data[0];
    try {
        return pixReadMem(data, request.data.size());
    } catch (const std::exception& e) {
        cerr << "Error in reading image\n";
        return NULL;
    } catch (...) {
        cerr << "Unknown error in reading image\n";
        return NULL;
    }
}


/*int matrix_h[9] = {1,0,-1,2,0,-2,1,0,-1}, //sobel
        matrix_v[9] = {1,2,1,0,0,0,-1,-2,-1};*
int matrix_h[9] = {-1,-1,-1,2,2,2,-1,-1,-1},
        matrix_v[9] = {-1,2,-1,-1,2,-1,-1,2,-1}; //canny
 */
int matrix_h[9] = {1, 0, -1, 60, 0, -60, 1, 0, -1},
matrix_v[9] = {1, 60, 1, 0, 0, 0, -1, -60, -1};

PIX* pixEdgeFilter(PIX* pixs, l_int32 orientflag) {
    l_int32 w, h, d, i, j, wplt, wpld, gx, gy, vald;
    l_int32 val1, val2, val3, val4, val5, val6, val7, val8, val9;
    l_uint32 *datat, *linet, *datad, *lined;
    PIX *pixt, *pixd;

    PROCNAME("pixSobelEdgeFilter");

    if (!pixs)
        return (PIX *) ERROR_PTR("pixs not defined", procName, NULL);
    pixGetDimensions(pixs, &w, &h, &d);
    if (d != 8)
        return (PIX *) ERROR_PTR("pixs not 8 bpp", procName, NULL);
    if (orientflag != L_HORIZONTAL_EDGES && orientflag != L_VERTICAL_EDGES &&
            orientflag != L_ALL_EDGES)
        return (PIX *) ERROR_PTR("invalid orientflag", procName, NULL);

    /* Add 1 pixel (mirrored) to each side of the image. */
    if ((pixt = pixAddMirroredBorder(pixs, 1, 1, 1, 1)) == NULL)
        return (PIX *) ERROR_PTR("pixt not made", procName, NULL);

    /* Compute filter output at each location. */
    pixd = pixCreateTemplate(pixs);
    datat = pixGetData(pixt);
    wplt = pixGetWpl(pixt);
    datad = pixGetData(pixd);
    wpld = pixGetWpl(pixd);
    for (i = 0; i < h; i++) {
        linet = datat + i * wplt;
        lined = datad + i * wpld;
        for (j = 0; j < w; j++) {
            if (j == 0) { /* start a new row */
                val1 = GET_DATA_BYTE(linet, j);
                val2 = GET_DATA_BYTE(linet + wplt, j);
                val3 = GET_DATA_BYTE(linet + 2 * wplt, j);
                val4 = GET_DATA_BYTE(linet, j + 1);
                val5 = GET_DATA_BYTE(linet + wplt, j + 1);
                val6 = GET_DATA_BYTE(linet + 2 * wplt, j + 1);
                val7 = GET_DATA_BYTE(linet, j + 2);
                val8 = GET_DATA_BYTE(linet + wplt, j + 2);
                val9 = GET_DATA_BYTE(linet + 2 * wplt, j + 2);
            } else { /* shift right by 1 pixel; update incrementally */
                val1 = val4;
                val2 = val5;
                val3 = val6;
                val4 = val7;
                val5 = val8;
                val6 = val9;
                val7 = GET_DATA_BYTE(linet, j + 2);
                val8 = GET_DATA_BYTE(linet + wplt, j + 2);
                val9 = GET_DATA_BYTE(linet + 2 * wplt, j + 2);
            }
            if (orientflag == L_HORIZONTAL_EDGES)
                vald = L_ABS(matrix_h[0] * val1 + matrix_h[1] * val2 + matrix_h[2] * val3 +
                    matrix_h[3] * val4 + matrix_h[4] * val5 + matrix_h[5] * val6 +
                    matrix_h[6] * val7 + matrix_h[7] * val8 + matrix_h[8] * val9) >> 3;
            else if (orientflag == L_VERTICAL_EDGES)
                vald = L_ABS(matrix_v[0] * val1 + matrix_v[1] * val2 + matrix_v[2] * val3 +
                    matrix_v[3] * val4 + matrix_v[4] * val5 + matrix_v[5] * val6 +
                    matrix_v[6] * val7 + matrix_v[7] * val8 + matrix_v[8] * val9) >> 3;
            else { /* L_ALL_EDGES */
                gx = L_ABS(matrix_v[0] * val1 + matrix_v[1] * val2 + matrix_v[2] * val3 +
                        matrix_v[3] * val4 + matrix_v[4] * val5 + matrix_v[5] * val6 +
                        matrix_v[6] * val7 + matrix_v[7] * val8 + matrix_v[8] * val9) >> 3;

                gy = L_ABS(matrix_h[0] * val1 + matrix_h[1] * val2 + matrix_h[2] * val3 +
                        matrix_h[3] * val4 + matrix_h[4] * val5 + matrix_h[5] * val6 +
                        matrix_h[6] * val7 + matrix_h[7] * val8 + matrix_h[8] * val9) >> 3;
                vald = L_MIN(255, gx + gy);
            }
            SET_DATA_BYTE(lined, j, vald);
        }
    }

    pixDestroy(&pixt);
    return pixd;
}



