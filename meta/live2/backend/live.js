#!/usr/bin/env node

// megabytes of memory that must be free all the time
var MEMORYLIMIT = 512;

// maximal runtime per process in seconds
var TIMELIMIT = 120;

// enforce constrained path
process.env.PATH = require('path').resolve('tools/bin');
WORKDIR = require('path').resolve('workdir');

// includes
var spawn = require('child_process').spawn;
var exec = require('child_process').exec;
var express = require('express');
var fs = require('fs');

// configure the express app
var app = express();
app.configure(function(){
  app.use(express.bodyParser());
});
app.use(express.errorHandler({ showStack: true, dumpExceptions: true }));

// process pool
var processpool = new Array();
var processcount = 0

// interval to enforce memory limit
setInterval(function() {
    // if memory limit is underrun, start killing processes
    if (require('os').freemem() < MEMORYLIMIT*1048576) {
        // call ps for a list of running processes
        exec('ps -o pid -m', function(error,stdout,stderr) {
            var psresult = stdout.split('\n');
            for (i = 0; i < psresult.length; i++) {
                // if we find a process that is in the process pool, kill it
                if (processpool.indexOf(parseInt(psresult[i])) != -1) {
                    console.log('[' + psresult[i] + '] terminating: memory limit (' + MEMORYLIMIT + ' MB) reached');
                    process.kill(psresult[i], 'SIGINT');
                    break;
                }
            }
        });
    }
}, 2000);


app.post('/', function(req, res){
    // increase timelimit
    req.connection.setTimeout((TIMELIMIT + 30) * 1000);

//    console.log(req.headers)
//    console.log(req.body);
//    console.log(req.files);

    // create working dir and copy all files into it
    var currentDate = new Date();
    workdir = WORKDIR + '/' + require('dateformat')(currentDate, "yyyy-mm/dd-HHMMss-L");
    require('mkdirp')(workdir, function (err) {
        if (req.files == undefined) {
            return;
        }

        files = new Array();

        for (i in req.files) {
            if (req.files[i] instanceof Array) {
                for (var j = 0; j < req.files[i].length; ++j) {
                    files.push(req.files[i][j]);
                }
            } else {
                files.push(req.files[i]);
            }
        }
        
        // iterate the array
        for (var i = 0; i < files.length; i++) {
            if (files[i].size > 0) {
                fs.renameSync(files[i].path, workdir + '/' + files[i].filename);
            }
        }
    });

    // shameless advertisement
    res.setHeader("X-Powered-By", "service-technology.org");

    var user_data;
    try {
        // collect POST-request
        user_data = JSON.parse(req.body.data)
        console.log(user_data);
        console.log('Tool: ' + user_data.tool);
        console.log('Parameters: ' + user_data.parameters);
    } catch(e) {
        // there was a problem (malformed JSON or missing entries)
        res.writeHead(500, { 'Content-Type': 'application/json' });
        res.end('{"error": {"name": "' + e.name + '", "message": "' + e.message + '"}}');
        return;
    }

    // archive the request
    fs.writeFile(workdir + '/request.json', JSON.stringify(user_data), function(err) {});

    // start tool
    var child;

    var resp = new Object;
    resp.result = new Object;
    resp.result.exit = new Object;
    resp.result.time = new Object;
    resp.result.output = new Object;

    // save some meta-data
    resp.meta = new Object;
    resp.meta.arch = process.arch
    resp.meta.platform = process.platform
    resp.meta.uptime = process.uptime()
    resp.meta.processes = processcount

    try {
        child = spawn(user_data.tool, user_data.parameters, {'cwd': workdir});
        resp.result.time.begin = (new Date()).getTime();
        processcount++;
    } catch(e) {
        // the tool could not be started
        res.writeHead(500, { 'Content-Type': 'application/json' });
        res.end('{"error": {"name": "' + e.name + '", "message": "' + e.message + '"}}');
        return;
    }

    var pid = child.pid;
    processpool.push(pid);

    console.log('[' + child.pid + '] started ' + user_data.tool + ' (process #' + processpool.length + ')');

    resp.result.output.stdout = "";
    resp.result.output.stderr = "";

    // collect output on stdout
    child.stdout.on('data', function(data) {
        resp.result.output.stdout += data;
    });

    // collect output on stderr
    child.stderr.on('data', function(data) {
        resp.result.output.stderr += data;
    });

    // close connection once the tool terminates
    child.on('exit', function(code, signal) {
        resp.result.time.end = (new Date()).getTime();

        // store exit code and kill signal
        resp.result.exit.signal = signal;
        resp.result.exit.code = code;
        
        resp.result.output.stdout = resp.result.output.stdout.split("\n")
        resp.result.output.stderr = resp.result.output.stderr.split("\n")
        resp.result.output.stdout.pop();
        resp.result.output.stderr.pop();
        
        // remove tool from process pool and switch of timout
        processpool.splice(processpool.indexOf(pid), 1);
        clearTimeout(killprocess);

        console.log('[' + pid + '] terminated ' + user_data.tool + ': exit code ' + code + ' (' + processpool.length + ' processes left)');

        // prepare response
        var r = JSON.stringify(resp);
        
        // archive a copy
        fs.writeFile(workdir + '/response.json', r, function(err) {});
        
        res.header('Content-Type', 'application/json');
        res.end(r);
    });

    // kill tool after some seconds of execution
    var killprocess = setTimeout(function() {
        resp.result.exit.comment = "Terminated due to time limit";
        child.kill('SIGINT');
    }, TIMELIMIT * 1000);
});

app.listen(1337);
console.log('Listening on port 1337');
