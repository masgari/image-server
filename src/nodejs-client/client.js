var thrift = require('thrift'),
    fs = require('fs');

var ImageService = require('../../build/gen-nodejs/ImageService.js'),
    ttypes = require('../../build/gen-nodejs/service_types.js');

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
    var image = new ttypes.TImage({data:imageData,width:100,height:80});
    console.log('image:', image);
    client.resize(image, function(err, response){
        if (err) {
            console.log('error:', err);
        } else {
            console.log('response:', response)

        }
        //connection.end();
    });

}
fs.readFile('girl.jpg', function(err, data){
    if (err) {
        console.log('error in reading test file')
    } else {
        resizeImage(data);
    }
});


connection.on('error', function(err) {
  console.error(err);
});

