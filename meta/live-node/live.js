var http = require('http');
var spawn = require('child_process').spawn;
var exec = require('child_process').exec;

// process pool
var processpool = new Array();

// variables for memory consumption check
var maxMem = 0;
var maxPid = 0;
var maxName = '';

// kill loop for memory consumption
setInterval(function() {

    for (p in processpool) {
        var pi = processpool[p].pid;

        exec('ps -p ' + pi + ' -o rss -o comm -o pid | grep ' + pi, function(error,stdout,stderr) {
            var dataarray = stdout.split(' ');

            if (dataarray[0] > maxMem) {
                maxMem = dataarray[0];
                maxPid = pi;
                maxName = dataarray[1];
            }
        });
    }

    if (maxPid != 0 && require('os').freemem() < 512*1024*1024) {
        console.log('[' + maxPid + '] terminating ' + maxName + ': memory limit reached');
        try {
            process.kill(maxPid, 'SIGINT');
        } catch(e) {
            console.error('[' + maxPid + '] error: ' + e['message']);
        }
        maxMem = 0;
        maxPid = 0;
        maxName = '';
    }
}, 2000);


var server = http.createServer(function (req, res) {
  // extract HTTP POST query and tool name from request
  var post = require('url').parse(req.url, true);
  var tool = post['pathname'].substr(1);

  // generate a parameter array from HTTP POST query
  var parameters = new Array();
  for (i in post['query']) {
      parameters.push(post['query'][i])
  }

  // give first feedback
  res.writeHead(200, {'Content-Type': 'text/plain'});
  res.write('Executing ' + tool + '...\n');


  // start tool
  var child = spawn(tool, parameters, {'cwd': '/tmp'});
  var pid = child.pid;
  processpool.push(child);

  console.log('[' + child.pid + '] started ' + tool + ' (process #' + processpool.length + ')');

  // redirect stdout to output
  child.stdout.on('data', function(data) {
      res.write(data);
  });

  // redirect stderr to output
  child.stderr.on('data', function(data) {
      res.write(data);
  });

  // close connection once the tool terminates
  child.on('exit', function(code) {
      processpool.splice(processpool.indexOf(child), 1);
      clearTimeout(killprocess);

      console.log('[' + pid + '] terminated ' + tool + ': exit code ' + code + ' (' + processpool.length + ' processes left)');
      res.end(tool + ' terminated ' + tool + ' with code ' + code + '\n');
  });

  // kill tool after 60 seconds of execution
  var killprocess = setTimeout(function() {
      console.log('[' + maxPid + '] terminating ' + maxName + ': time limit reached');
      child.kill('SIGINT');
  }, 60000);
});

server.listen(1337);
console.log('Server running at http://127.0.0.1:1337/');
