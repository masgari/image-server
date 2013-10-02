/* 
 * File:   ImageServiceImpl.h
 * Author: mamad
 *
 * Created on 22 September 2013, 23:09
 */

#ifndef IMAGESERVICEIMPL_H
#define	IMAGESERVICEIMPL_H
#include "../../../build/gen-cpp/service_constants.h"
#include "../../../build/gen-cpp/service_types.h"
#include "../../../build/gen-cpp/ImageService.h"


class ImageServiceImpl: virtual public ImageServiceIf  {
public:
    ImageServiceImpl(const int threads);
    ImageServiceImpl();
    virtual ~ImageServiceImpl();
    
  int8_t ping() {return handlePing();};
  void resize(TImageResponse& _return, const TImage& request) {handleResize(_return, request);};
  void cartoonize(TImageResponse& _return, const TImage& request) {handleCartoonize(_return, request);};  
private:
  int8_t handlePing();
  void handleResize(TImageResponse& _return, const TImage& request);
  void handleCartoonize(TImageResponse& _return, const TImage& request);
  
};

#endif	/* IMAGESERVICEIMPL_H */

