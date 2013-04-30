
function errorHandler() {
    // hide loading modal
    $('#output_container').modal("hide");

    var e = 'An error occured when the Request was sent to server.';
    var h = 'Error in Request';

    $('#myAlert h3').text(h);
    $('#myAlert p').text(e);
    $('#myAlert').modal();
}


/*function myAlert(headLine, text) {
    var modal =$('<div />');
    var modheader = $('<div/>').addClass('modal-header');
    var close = $('<button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>');
    var header = $('<h3>' + headLine + '</h3>');
    var msg = $('<div>' + text + '</div>').addClass('modal-body');
    var footer =$('<div />').addClass('modal-footer');

    close.appendTo(modheader);
    header.appendTo(modheader);
    modheader.appendTo(modal);
    msg.appendTo(modal);

    close.clone().removeClass('close').addClass('btn').text('close').appendTo(footer);
    footer.appendTo(modal);

    modal.addClass('modal').modal();
    modal.appendTo('body');
} */ 

function beforeHandler() {
    $("#output_contents").hide();
    $('#output_container').modal();
    $("#output_loading_info").fadeIn();
}

function completeHandler(r) {
    var stdout = r.result.output.stdout.join('\n') || '(empty)';
    var stderr = r.result.output.stderr.join('\n') || '(empty)';
    var files = r.result.output.files;
    delete r.result.output;
    var meta = dumpObject(r.meta);
    var formatOut = dumpObject(r);
    $('#stdout').html(stdout);
    $('#stderr').html(stderr);
    $('#meta').html(meta);

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
        f += '<h2>'+h+'</h2><pre>'+t+'</pre>';
    }
    $('#files_output_container').html(f);
    $("#output_loading_info").fadeOut();
    $('#output_contents').fadeIn();

}

function progressHandlingFunction(s) {
    return true;
}

$('#outForm').submit(function(){
    var formData = new FormData(this);
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
        processData: false
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
