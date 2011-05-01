// megabytes of memory that must be free all the time
var MEMORYLIMIT = 512;
// maximal runtime per process in seconds
var TIMELIMIT = 120;

var http = require('http');
var spawn = require('child_process').spawn;
var exec = require('child_process').exec;

// process pool
var processpool = new Array();

// interval to enforce memory limit
setInterval(function() {
    if (require('os').freemem() < MEMORYLIMIT*1048576) {
        exec('ps -o pid -m', function(error,stdout,stderr) {
            var psresult = stdout.split('\n');

            for (i = 0; i < psresult.length; ++i) {
                if (processpool.indexOf(parseInt(psresult[i])) != -1) {
                    console.log('[' + psresult[i] + '] terminating: memory limit (' + MEMORYLIMIT + ' MB) reached');
                    process.kill(psresult[i], 'SIGINT');
                    break;
                }
            }
        });
    }
}, 2000);

// the web server
var server = http.createServer(function (req, res) {
  // extract HTTP POST query and tool name from request
  var post = require('url').parse(req.url, true);
  var tool = post['pathname'].substr(1);

  // generate a parameter array from HTTP POST query
  var parameters = new Array();
  for (i in post['query']) {
      parameters.push(post['query'][i])
  }

  // start tool
  var child;
  try {
      child = spawn(tool, parameters, {'cwd': '/tmp'});
  } catch(e) {
      console.error('Server could not start ' + tool + ': ' + e.message + " - request answered with 503");
      res.writeHead(503, {'Content-Type': 'text/plain'});
      res.end('The server is currently overloaded. Please try again later.\n');
      return;
  }

  var pid = child.pid;
  processpool.push(pid);

  // give first feedback
  res.writeHead(200, {'Content-Type': 'text/plain'});
  res.write('Executing ' + tool + '...\n');
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
      code = (code == null) ? '- (forced termination)' : code;
      processpool.splice(processpool.indexOf(child.pid), 1);
      clearTimeout(killprocess);

      console.log('[' + pid + '] terminated ' + tool + ': exit code ' + code + ' (' + processpool.length + ' processes left)');
      res.end(tool + ' terminated ' + tool + ' with code ' + code + '\n');
  });

  // kill tool after some seconds of execution
  var killprocess = setTimeout(function() {
      console.log('[' + child.pid + '] terminating ' + tool + ': time limit reached');
      child.kill('SIGINT');
  }, TIMELIMIT * 1000);
});

server.listen(1337);
console.log('Server running at http://127.0.0.1:1337/');

// if anything goes wrong: kill all spawned children
process.on('uncaughtException', function (err) {
  console.error('Server experienced uncaught exception: ' + err);
  for (pid in processpool) {
      console.log('[' + pid + '] terminating: global error');
      try {
          process.kill(pid, 'SIGINT');
      } catch(e) {
          console.error('[' + pid + '] error: ' + e['message']);
      }
  }
  
  setTimeout(function() {
      process.exit(1);
  }, 2000);
});
