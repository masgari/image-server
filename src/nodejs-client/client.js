var thrift = require('thrift');

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

var image = ttypes.TImage({data:0,width:100,height:100});
client.resize(image, function(err, response){
    if (err) {
        console.log('error:', err);
    } else {
        console.log('response:', response)

    }
    connection.end();
});

connection.on('error', function(err) {
  console.error(err);
});

