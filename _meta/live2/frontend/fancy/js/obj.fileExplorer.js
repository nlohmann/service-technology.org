
// scope wrapper
FILE_EXPLORER = (function(){

// var files;
// var fileNames = [];
var filesByFilename = {};
var links = {};
var onchangeFunctions = [];

// a form, where the file inputs are
var fileInput = $('<input type="file" multiple>').addClass('fileHideSafari').appendTo('body');

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
    var $this = $(this);
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
    // now all known links
    for(var l in links) {
        var lRead = l;
        if(l.length > 30) {
            lRead = l.slice(0,15) + ' [...] ' + l.slice(l.length-15, l.length);
        }
        var text = '<span class="btn btn-link">' + lRead +'</span>';
        var link = '<a class="btn btn-link btn-mini" style="display:inline;" href="' + l + '" target="_blank"><i class="icon-globe"></i></a>';
        var li = $('<li>' + text + link + '</li>');
        li.children('span.btn').click(function(){callback($(this).next().attr('href'));});
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
    fileInput.unbind('change');
    fileInput.click();
    // callback is only called, if value is assigned
    fileInput.change(
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

// here the links can be accessed, for request
function getLinks() {
    var usedLinks = [];
    for(var i in links) {
        if(links[i]) {
            usedLinks.push(i);
        }
    }
    return usedLinks;
}

function validateInputs(inputs) {
    return function() {
        var iLen = inputs.length;

        for(var curLink in links) {
            links[curLink] = false;
        }

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
            controlGroup.find('.fileInputLinkButton').remove();

            var curVal = cur.getValue();

            // check if link
            if(typeof curVal == 'string' && curVal.match(/(^\"?https?\:)|(^\"?s?ftp\:)|(^\"?svn\:)/i)) {
                // the provided value is a link
                curVal = curVal.replace(/(^\")|(\"$)/g, '');
                var linkBtn = $('<a target="_blank" title="open link" href="' + curVal + '"><i class="icon-globe"></i></a>');
                linkBtn.addClass("btn btn-mini btn-link fileInputLinkButton");
                linkBtn.insertAfter(curInput.next());

                links[curVal] = true;
            }
            // check, if we have such a file, if no link
            else if(curVal && !filesByFilename[curVal]) {
                // put a red box around
                var param = curInput.attr('data-stlive-argname');
                if(!param) param = 'File Input';
                var msg = '<h4>Missing file</h4><p>Filename <b>"' + curVal + '"</b> was specified in parameter <b>' + param + '</b> but no such file was uploaded yet</p>';
                // now show tooltip above input
                controlGroup.addClass('warning');
                controlGroup.attr('title', msg);
                controlGroup.tooltip( {html: true });

                // now create alert
                msg += '<p> <span class="btn" onclick="$(\'#' + cur.params.id + '\').val(\'\').change();"><i class="icon-remove"></i> Remove file name</span>';
                msg += ' <span class="dropdown"><span data-toggle="dropdown" class="dropdown-toggle btn btn-danger" onclick="FILE_EXPLORER.select_dropdown.call(this, function(v){$(\'#' + cur.params.id + '\').val(v).change();})"><i class="icon-folder-open icon-white"></i> Select File...</span></span></p>';
                // first show alert on top
                var alrt = $('<div>' + msg + '</div>').addClass('alert alert-error alert-block').hide();
                alrt.appendTo(alrtContainer);
                alrt.fadeIn();

            }
        }
    }
}

// public interface
return {
    add: add,
    select_dropdown: select_dropdown,
    getFiles: getFiles,
    getLinks: getLinks,
    onchange: onchange,
    validateInputs: validateInputs,
    dropFile: dropFile
}

// scope wrapper
})();
