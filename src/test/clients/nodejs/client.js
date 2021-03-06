var thrift = require('thrift'),
    fs = require('fs');

var ImageService = require('../../../../build/gen-nodejs/ImageService.js'),
    ttypes = require('../../../../build/gen-nodejs/service_types.js');

var connection = thrift.createConnection('localhost', 3800, {transport:thrift.TFramedTransport});
var client = thrift.createClient(ImageService, connection);

client.ping(function(err, response){
    if (err) {
        console.log('ping-error:', err);
    } else {
        console.log('ping-response:', response)
    }
});

function resizeImage(imageData) {
    console.log('resizing image:', imageData);
    var image = new ttypes.TImage({data:imageData,width:100,height:80});
    client.resize(image, function(err, response){
        if (err) {
            console.log('error:', err);
        } else {
            if (response.error) {
                console.log('server error:', err);
                return;
            }
            var fd =  fs.openSync('resized.jpg', 'w');
            var data = response.result.data;
            var buff = new Buffer(data);
            console.log('result:', buff);
            fs.write(fd, buff, 0, buff.length, 0, function(err,written){
                if (err) {
                    console.log('error in writing result to file');
                } else {
                    console.log('result has been written to file. bytes:', written)
                }
            });
        }
    });
}

function cartoonizeImage(imageData) {
    console.log('cartoonizing image:', imageData);
    var image = new ttypes.TImage({data:imageData,width:100,height:80});
    client.cartoonize(image, function(err, response){
        if (err) {
            console.log('error:', err);
        } else {
            if (response.error) {
                console.log('server error:', err);
                return;
            }
            var fd =  fs.openSync('cartoonize.jpg', 'w');
            var data = response.result.data;
            var buff = new Buffer(data);
            console.log('result:', buff);
            fs.write(fd, buff, 0, buff.length, 0, function(err,written){
                if (err) {
                    console.log('error in writing result to file');
                } else {
                    console.log('result has been written to file. bytes:', written)
                }
            });
        }
    });
}

fs.readFile('girl.jpg', function(err, data){
    if (err) {
        console.log('error in reading test file')
    } else {
        resizeImage(data);
        cartoonizeImage(data);
        connection.end();
    }
});


connection.on('error', function(err) {
  console.error(err);
});

