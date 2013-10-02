#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/param.h>
#include <unistd.h>

#if !defined(LEPTONICA_ALLHEADERS_H)
#   include <leptonica/allheaders.h>
#endif

/**
 * Sample Leptonica code, from: http://www.pocketmagic.net/2013/06/cartoon-yourself-using-leptonica/#.Ui92nyJDuKl
 */

static const char* BLUR_KERNEL = " 5 10 20 10 5 " " 10 20 50 20 10 " " 10 70 140 70 10 " " 10 20 50 20 10 " " 5 10 20 10 5 ";

//static const char* BLUR_KERNEL_3 = " 20 50 20 " " 70 140 70 " " 20 50 20 ";
static const char* BLUR_KERNEL_3 = " 10 10 10 " " 10 10 10 " " 10 10 10 ";
//static const char* BLUR_KERNEL_3 = " 51 51 51 " " 51 51 51 " " 51 51 51 ";

PIX* pixEdgeFilter(PIX* pixs,l_int32  orientflag);

main(int argc, char *argv[]) {
	printf("Leptonica Cartooniser\nConvert images to cartoon v0.1\n");
	if (argc < 2) {
		printf("\nUsage: cartoon <input image>\n\n");
		return -1;
	}
	char* filename = argv[1];
	printf("0.Reading file: %s\n", filename);
	PIX* pix = pixRead(filename);
	if (!pix) {
		printf("Error reading file\n");
		return 1;
	}
	printf("1.Image dimensions: (w:%d,h:%d,depth:%d)\n", pix->w, pix->h, pix->d);

	PIX* gray;
	if (pix->d > 8) {
		gray = pixConvertRGBToGrayMinMax(pix, L_CHOOSE_MAX);
		if (!gray) {
			printf("Error in converting to grayscale\n");
			pixDestroy(&pix);
			return 2;
		} else {
			pixWrite("gray-step2.jpg", gray, IFF_JFIF_JPEG);
			printf("2.Converted to grayscale.\n");
		}

	}

	//edge detection
	//PIX* edges = pixSobelEdgeFilter(gray,  L_ALL_EDGES);
	PIX* edges = pixEdgeFilter(gray,  L_ALL_EDGES);
	//PIX* edges = pixTwoSidedEdgeFilter(gray,  L_ALL_EDGES);

	if (!edges) {
		printf("Error in detecting edges\n");
		pixDestroy(&gray);
		pixDestroy(&pix);
		return 2;
	}
	pixDestroy(&gray);

	pixWrite("edges-step3.jpg", edges, IFF_JFIF_JPEG);
	printf("3.Edges detected.\n");

	PIX* inverted = pixInvert(NULL, edges);
    pixWrite("inverted-step4.jpg", inverted, IFF_JFIF_JPEG);
    printf("4.Edges inverted.\n");
    pixDestroy(&edges);

    //no direct bluring function, so create a bluring kernel first
    L_KERNEL* bKernel = kernelCreateFromString(5,5,2,2,BLUR_KERNEL);
    //L_KERNEL* bKernel = kernelCreateFromString(3,3,1,1,BLUR_KERNEL_3);
    if (!bKernel) {
    	printf("Could not create blur kernel\n");
    	return 3;
    }
    PIX* blur = pixConvolve(inverted, bKernel, 8, 1);
    kernelDestroy(&bKernel);
    pixWrite("blur-step5.jpg", blur, IFF_JFIF_JPEG);
    printf("5.Edges blured.\n");
    //PIX* bOrig = pixRead("5.jpg");
	//PIX* cartoon = pixBlendHardLight(NULL, pix, bOrig, 0, 0, 150/255.0);
	//pixDestroy(&bOrig);
    PIX* cartoon = pixBlendHardLight(NULL, pix, blur, 0, 0, 150/255.0);
	pixWrite("cartoon-step6.jpg", cartoon, IFF_JFIF_JPEG);
	printf("6.Blending done.\n");
    pixDestroy(&blur);
    pixDestroy(&inverted);
    pixDestroy(&cartoon);
	pixDestroy(&pix);
	return 0;
}



/*int matrix_h[9] = {1,0,-1,2,0,-2,1,0,-1}, //sobel
	matrix_v[9] = {1,2,1,0,0,0,-1,-2,-1};*
int matrix_h[9] = {-1,-1,-1,2,2,2,-1,-1,-1},
	matrix_v[9] = {-1,2,-1,-1,2,-1,-1,2,-1}; //canny
	*/
int matrix_h[9] = {1,0,-1,60,0,-60,1,0,-1},
	matrix_v[9] = {1,60,1,0,0,0,-1,-60,-1};


PIX * pixEdgeFilter(PIX     *pixs,l_int32  orientflag)
{
l_int32    w, h, d, i, j, wplt, wpld, gx, gy, vald;
l_int32    val1, val2, val3, val4, val5, val6, val7, val8, val9;
l_uint32  *datat, *linet, *datad, *lined;
PIX       *pixt, *pixd;

    PROCNAME("pixSobelEdgeFilter");

    if (!pixs)
        return (PIX *)ERROR_PTR("pixs not defined", procName, NULL);
    pixGetDimensions(pixs, &w, &h, &d);
    if (d != 8)
        return (PIX *)ERROR_PTR("pixs not 8 bpp", procName, NULL);
    if (orientflag != L_HORIZONTAL_EDGES && orientflag != L_VERTICAL_EDGES &&
        orientflag != L_ALL_EDGES)
        return (PIX *)ERROR_PTR("invalid orientflag", procName, NULL);

        /* Add 1 pixel (mirrored) to each side of the image. */
    if ((pixt = pixAddMirroredBorder(pixs, 1, 1, 1, 1)) == NULL)
        return (PIX *)ERROR_PTR("pixt not made", procName, NULL);

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
            if (j == 0) {  /* start a new row */
                val1 = GET_DATA_BYTE(linet, j);
                val2 = GET_DATA_BYTE(linet + wplt, j);
                val3 = GET_DATA_BYTE(linet + 2 * wplt, j);
                val4 = GET_DATA_BYTE(linet, j + 1);
                val5 = GET_DATA_BYTE(linet + wplt, j + 1);
                val6 = GET_DATA_BYTE(linet + 2 * wplt, j + 1);
                val7 = GET_DATA_BYTE(linet, j + 2);
                val8 = GET_DATA_BYTE(linet + wplt, j + 2);
                val9 = GET_DATA_BYTE(linet + 2 * wplt, j + 2);
            } else {  /* shift right by 1 pixel; update incrementally */
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
                vald = L_ABS(	matrix_h[0]*val1 + matrix_h[1]*val2 + matrix_h[2]*val3 +
								matrix_h[3]*val4 + matrix_h[4]*val5 + matrix_h[5]*val6 +
								matrix_h[6]*val7 + matrix_h[7]*val8 + matrix_h[8]*val9) >> 3;
            else if (orientflag == L_VERTICAL_EDGES)
                vald = L_ABS(	matrix_v[0]*val1 + matrix_v[1]*val2 + matrix_v[2]*val3 +
								matrix_v[3]*val4 + matrix_v[4]*val5 + matrix_v[5]*val6 +
								matrix_v[6]*val7 + matrix_v[7]*val8 + matrix_v[8]*val9) >> 3;
            else {  /* L_ALL_EDGES */
                gx = L_ABS(	matrix_v[0]*val1 + matrix_v[1]*val2 + matrix_v[2]*val3 +
								matrix_v[3]*val4 + matrix_v[4]*val5 + matrix_v[5]*val6 +
								matrix_v[6]*val7 + matrix_v[7]*val8 + matrix_v[8]*val9) >> 3;

				gy = L_ABS(	matrix_h[0]*val1 + matrix_h[1]*val2 + matrix_h[2]*val3 +
								matrix_h[3]*val4 + matrix_h[4]*val5 + matrix_h[5]*val6 +
								matrix_h[6]*val7 + matrix_h[7]*val8 + matrix_h[8]*val9) >> 3;
				vald = L_MIN(255, gx + gy);
            }
            SET_DATA_BYTE(lined, j, vald);
        }
    }

    pixDestroy(&pixt);
    return pixd;
}
