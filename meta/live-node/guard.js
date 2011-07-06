// megabytes of memory that must be free all the time
var MEMORYLIMIT = 2096;

var exec = require('child_process').exec;

// interval to enforce memory limit
setInterval(function() {
    if (require('os').freemem() < MEMORYLIMIT*1048576) {
        exec('killall lola-sumo-statepredicate', function(error,stdout,stderr) {
            console.log("killed");
        });
    }
}, 2000);
