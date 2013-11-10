var pis = require('../index'),
    should = require('should'),
    fs = require('fs'),
    assert = require("assert");

module.exports = {
    setUp: function (callback) {
        var testSelf = this;
        should.exist(pis.createClient);
        this.client = pis.createClient();
        should.exist(this.client);

        this.client.on('connect', function () {
            testSelf.connected = true;
        });
        this.client.on('error', function (e) {
            console.log('Error in client:', e);
            callback(e);
        });

        this.client.on('end', function () {
            console.log('client ended.');
        });

        should.exist(this.client.connect);

        this.client.connect(function (err) {
            throw err;
        });
        callback();
    },

    tearDown: function (callback) {
        should.exist(this.client.end);
        if (this.connected) {
            this.client.end(function (err) {
                console.log('Error in tearDown:', err);
                callback(err);
            });
        }
        callback();
    },

    testPing: function (test) {
        should.exist(this.client.ping);
        this.client.ping(function (err, result) {
                test.equals(err, null, 'No error should be thrown.' + err);
                test.equals(result, 0, 'Ping result must be 0');
                test.done();
            }
        );
    },

    testResize: function(test) {
        should.exist(this.client.resize);
        var buff = new Buffer('abc');
        var tester = test;
        this.client.resize(buff, 100, 100, function(err, resizedData, dimension, originalDimension){
            should.exist(err);
            console.log('error', err, '\n');
            tester.done();
        });
    },

    testResizeRealImage : function(test) {
        //test real image
        var pisClient = this.client;
        var tester = test;
        fs.readFile('girl.jpg', function(fsErr, data){
            if (fsErr) {
                console.log('error in reading test image file.\n');
                tester.done();
            } else {
                tester.ok(data);
                tester.ok(data.length > 0);
                pisClient.resize(data, 100, 80, function(err, resizeData, dimension, originalDimension){
                    should.not.exist(err);
                    tester.ok(resizeData, 'Resized data should not be null.');
                    tester.ok(resizeData.length > 0, 'Resized data length should not be 0.');
                    tester.ok(dimension, 'Resized dimension should not be null.');
                    tester.ok(dimension.width > 0, 'Resized dimension.width should not be 0.');
                    tester.ok(dimension.width > 0, 'Resized dimension.height should not be 0.');
                    tester.ok(originalDimension, 'Original image dimension should not be null.');
                    tester.done();
                });
            }
        });
    },

    testCartooniseImage : function(test) {
        var pisClient = this.client;
        var tester = test;
        fs.readFile('girl.jpg', function(fsErr, data){
            if (fsErr) {
                console.log('error in reading test image file.\n');
                tester.done();
            } else {
                tester.ok(data);
                tester.ok(data.length > 0);
                pisClient.cartoonize(data, 100, 80, function(err, returnData, dimension, originalDimension){
                    should.not.exist(err);
                    tester.ok(returnData, 'Cartoonized data should not be null.');
                    tester.ok(returnData.length > 0, 'Cartoonized data length should not be 0.');
                    tester.ok(dimension, 'Resized dimension should not be null.');
                    tester.ok(dimension.width > 0, 'Resized dimension.width should not be 0.');
                    tester.ok(dimension.width > 0, 'Resized dimension.height should not be 0.');
                    tester.ok(originalDimension, 'Original image dimension should not be null.');
                    tester.done();
                });
            }
        });
    }

}
