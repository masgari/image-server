var thrift = require('thrift'),
    ImageService = require('./ImageService.js'),
    ttypes = require('./service_types.js');

function PISClient(options, callback) {
    var self = this;
    if (!options) {
        var options = {};
    }

}

module.exports = PISClient;

PISClient.prototype.ping = function(callback) {
    callback(new Error('Nothing yet'));
}

