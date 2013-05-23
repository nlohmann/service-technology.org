
// scope wrapper
FILE_EXPLORER = (function(){

// var files;
// var fileNames = [];
var filesByFilename = {};
var onchangeFunctions = [];

// a form, where the file inputs are
var fileForm = $('<form>');
fileForm.appendTo('body');

function onchange(callback) {
    if(typeof callback == 'function') {
        onchangeFunctions.push(callback);
    } else {
        // create the list of filenames
        var fileNames = [];
        for(i in filesByFilename) {
            fileNames.push(i);
        }
        // go through all callbacks
        for(var i = 0, c = null; c = onchangeFunctions[i]; ++i) {
            c(fileNames);
        }
    }
}

/* this function creates a dropdown menu to select a file
 * after files is selected (or new file for upload selected),
 * the callback function is called with filename as argument
 *
 * must be run on html-object scope, where the drop-down should appear
 */
function select_dropdown(callback) {
    $this = $(this);
    // remove old dropdowns
    $(this).parent().children('ul').remove();
    // dropdown unordered list
    var ddUl = $('<ul class="dropdown-menu" role="menu">').insertAfter($(this));

    // item link to add a new file
    var linkNew = $('<li class="btn btn-link"><i class="icon-upload"></i> new File</li><li class="divider"></li>');
    // if clicked, call add function
    linkNew.click(function(){ add(callback); });
    linkNew.appendTo(ddUl);

    // now all known file names
    for(var i in filesByFilename) {
        var li = $('<li><span class="btn btn-link">' + i +'</span></li>');
        li.children('span.btn').click(function(){callback($(this).text());});
        li.appendTo(ddUl);
    } 
    $(this).dropdown();
}

// returns a function, which is called after a change
// and checks, whether files already exists
function checkAddedFile(newFile, callback) {

        // in case we don't have a value, remove it
        if(!newFile.name) {
            alert('TODO');
            $this.remove();
        }

        // check if file name is already in use
        if( !filesByFilename[newFile.name]
            || window.confirm('this filename is already in use.\nOverwrite the old ' + newFile.name + '?')
          ) {
                filesByFilename[newFile.name] = newFile;
            }
        
        // call callback
        callback(newFile.name);
        // trigger event functions
        onchange();
}

function dropFile(evt, callback) {
    var newFile = evt.dataTransfer.files[0];
    checkAddedFile(newFile, callback);
}

// select a new file for upload, and after that call callback with filename
function add(callback) {
    var newFile = $('<input type="file" name="file">').addClass('fileHideSafari');
    newFile.appendTo(fileForm);
    var id = newId();
    newFile.attr('id', id);
    // reselect element to be sure it is there, even in safari
    
    $('#'+id).click();
    // callback is only called, if value is assigned
    newFile.change(
            function(e) {
                var newFile = this.files[0];
                checkAddedFile(newFile, callback);
            }
    );
}

// little helper as we need unique ids
var counter = 0;
function newId() {
    return 'file_input_'+ ++counter;
}

// here the files can be accessed, for request
function getFiles() {
    return filesByFilename;
}

function validateInputs(inputs) {
    return function() {
        var iLen = inputs.length;

        // all existing alerts should fade out and then be removed
        var alrtContainer = $('#alertContainer').empty();
        for(var i = 0; i < iLen; ++i) {
            var cur = inputs[i];
            var curInput = $('#' + cur.params.id);
            if(cur.params.type != 'file') continue;

            // reset style
            var controlGroup = curInput.closest('.control-group').removeClass('warning');
            controlGroup.attr('title', '');
            controlGroup.tooltip('destroy');

            // check, if we have such a file
            var curVal = cur.getValue();
            if(curVal && !filesByFilename[curVal]) {
                // put a red box around
                var param = curInput.attr('data-stlive-argname');
                if(!param) param = 'File Input';
                var msg = 'filename <b><i>' + curVal + '</i></b> was specified in parameter <b>' + param + '</b> but no such file was uploaded yet';
                // first show alert on top
                var alrt = $('<div>' + msg + '</div>').addClass('alert').hide();
                alrt.appendTo(alrtContainer);
                alrt.fadeIn();

                // now show tooltip above input
                controlGroup.addClass('warning');
                controlGroup.attr('title', msg);
                controlGroup.tooltip( {html: true });
            }
        }
    }
}

// public interface
return {
    add: add,
    select_dropdown: select_dropdown,
    getFiles: getFiles,
    onchange: onchange,
    validateInputs: validateInputs,
    dropFile: dropFile
}

// scope wrapper
})();
