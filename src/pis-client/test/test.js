var pis = require('../index'),
    should = require('should'),
    assert = require("assert");

should.exist(pis);
should.exist(pis.createClient);
var client = pis.createClient();
should.exist(client);
should.exist(client.ping);
client.ping(function(err, result){
    should.not.exist(err);
    should.exist(result);
    result.should.equal(0);
});