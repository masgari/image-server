var thrift = require('thrift'),
    ImageService = require('./ImageService.js'),
    ttypes = require('./service_types.js');
var util = require('util');
var events = require('events');

var DEFAULTS = {
    host: '127.0.0.1',
    port: '3800'
};

function Client(options) {
    options = options || {};
    this.options = merge(options, DEFAULTS);
}

exports.Client = Client;

util.inherits(Client, events.EventEmitter);


Client.prototype.ping = function(callback) {
    callback(new Error('Nothing yet'), null);
    //callback(null, 0);
};


// main export function
exports.createClient = function(options) {
    return new Client(options);
}


// Helper functions

// callback || noop borrowed from node/lib/fs.js
function noop() {}


function merge(a, b){
    if (a && b) {
        for (var key in b) {
            if (typeof a[key] == 'undefined') {
                a[key] = b[key];
            } else if (typeof a[key] == 'object' && typeof b[key] == 'object') {
                a[key] = merge(a[key], b[key]);
            }
        }
    }
    return a;
}