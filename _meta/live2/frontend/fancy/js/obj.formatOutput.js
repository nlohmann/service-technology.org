// scope wrapper
(function(){
//

var noSent = 0;
var noRec = 0;

function errorHandler(err) {
    ++noRec;
    // hide loading modal
    $('#output_container').modal("hide");

    var e = 'An error occured when the Request was sent to server';
    var h = 'Error in Request';

    $('#myAlert h3').text(h);
    $('#myAlert p').text(e);
    $('#myAlert').modal();
}

function beforeHandler() {
    $("#output_contents").hide();
    $('#output_container').modal();
    $("#output_loading_info").fadeIn();
}

function completeHandler(r) {
    console.log('server response: ' + r);
    if(noSent != this.number) {
        console.log('received request number ' + noRec + ', but number ' + noSent + ' was already sent');
    }
    var stdout = r.result.output.stdout.join('\n') || '(empty)';
    var stderr = r.result.output.stderr.join('\n') || '(empty)';
    var files = r.result.output.files;
    delete r.result.output;
    var meta = dumpObject(r.meta);
    var formatOut = dumpObject(r);
    $('#stdout').text(stdout);
    $('#stderr').text(stderr);
    $('#meta').text(meta);

    var timeDiff = r.result.time.end - r.result.time.begin;
    var code = r.result.exit.code;
    var signal = r.result.exit.signal;

    //now build the info sentence
    var s = 'The tool exited in <b>' + timeDiff + 'ms</b><br>';
        s+= 'with Code : <b>' + code + '</b><br>';
        s+= 'and Signal: <b>' + signal + '</b><br>';

    $('#info').html(s);

    var f = '';
    // now update the files
    for(var i=0, c=null; c = files[i]; ++i) {
        var h = c.filename;
        var t = $.base64.decode(c.content);
        var a = '<a ' +
            ' download="'+h+'"' +
            ' href="data:Application/octet-stream,' + encodeURIComponent(t) + '"' +
            '>download ' + h + '</a>';
        var viz = '';
        if(h.match(/\.dot$/i)) {
            var vizId = VIZ.create(t);
            viz = ' <a href="javascript:VIZ.show(' + vizId + ')">Show vizualization</a> &nbsp; | &nbsp; ';
        }
        var encodedT = $('<div/>').text(t).html();
        f += '<h2>'+h+'</h2>' + viz + a +  '<pre>'+ encodedT +'</pre>';
    }

    $('#files_output_container').html(f);
    $('#output_loading_info').fadeOut();
    $('#output_contents').fadeIn();

}

function progressHandlingFunction(s) {
    return true;
}

$('#outForm').submit(function(){

    var files = FILE_EXPLORER.getFiles();
    var formData = new FormData(this);
    for (var i in files) {
        formData.append(i, files[i]);
    }

/*
    var links = FILE_EXPLORER.getLinks();
    var links_length = links.length;
    for (i = 0; i < links_length; ++i) {
        console.log('external link no: ' + i + '  --> ' + links[i]);
        formData.append('external_links[' + i + ']', links[i]);
    }
*/

    console.log(formData);
    $.ajax({
        url: this.action,  //server script to process data
        type: 'POST',

        //Ajax events
        beforeSend: beforeHandler, //function(){$('#output_container').fadeOut();},
        success: completeHandler,
        error: errorHandler,
        // Form data
        data: formData,
        dataType : "json",
        //Options to tell JQuery not to process data or worry about content-type
        cache: false,
        contentType: false,
        processData: false,
        context: { number: ++noSent }
    });
    return false;
});



function dumpObject(obj, indent)
{
  var result = "";
  if (indent == null) indent = "";

  for (var property in obj)
  {
    var value = obj[property];
    if (typeof value == 'string')
      value = "'" + value + "'";
    else if (typeof value == 'object')
    {
      if (value instanceof Array)
      {
        // Just let JS convert the Array to a string!
        value = "[ " + value + " ]";
      }
      else
      {
        // Recursive dump
        // (replace "  " by "\t" or something else if you prefer)
        var od = dumpObject(value, indent + "  ");
        // If you like { on the same line as the key
        //value = "{\n" + od + "\n" + indent + "}";
        // If you prefer { and } to be aligned
        value = "\n" + indent + "{\n" + od + "\n" + indent + "}";
      }
    }
    result += indent + "'" + property + "' : " + value + ",\n";
  }
  return result.replace(/,\n$/, "");
}


// scope wrapper
})();
