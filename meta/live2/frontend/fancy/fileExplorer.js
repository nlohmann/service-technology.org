
// scope wrapper
FILE_EXPLORER = (function(){

var fileNames = [];
var jqObj;
var fileForm;
function setId(id) {
    jqObj = $('#'+id);

    fileForm = $('<form>');
    fileForm.appendTo(jqObj);
}

function show(callback) {
    
    jqObj.unbind('hide');
    if(typeof callback == 'function') {
        jqObj.on('hide', callback);
    }
    jqObj.modal('show');
}

function select_modal(callback) {
    var modalBody = jqObj.find('div.modal-body').empty();
    var fileUl = $('<ul class="unstyled">').appendTo(modalBody);
    fileForm.find('input[type=file]').each(function(i) {
        $this = $(this);
        var prettyVal =$this.prettyVal();
        if(!prettyVal) {
            $this.remove();
            return true;
        }
        var li = $('<li><span data-dismiss="modal" class="btn btn-link">' + prettyVal +'</span></li>');
        li.children('span.btn').click(function(){callback($(this).text());});
        li.appendTo(fileUl);
    });
    // item link to add a new file
    var linkNew = $('<span class="btn">add new...</span>');
    // if clicked, call add function
    linkNew.click(function(){
            add(
                // if add clicked and something is returned, call callback
                function(v){
                    jqObj.modal('hide');
                    callback(v);
            });
        }
    );
    linkNew.insertAfter(fileUl);
    show();
}
function select_dropdown(callback) {
    $this = $(this);
    $(this).parent().children('ul').remove();
    var ddUl = $('<ul class="dropdown-menu" role="menu">').insertAfter($(this));
    /* fileForm.find('input[type=file]').each(function(i) {
        $this = $(this);
        var prettyVal =$this.prettyVal();
        if(!prettyVal) {
            $this.remove();
            return true;
        }
        */
    for(var i = 0, c = null; c = fileNames[i]; ++i) {
        var li = $('<li><span class="btn btn-link">' + c +'</span></li>');
        li.children('span.btn').click(function(){callback($(this).text());});
        li.appendTo(ddUl);
    } // );
    // item link to add a new file
    var linkNew = $('<li class="divider"></li><li class="btn btn-link"><i class="icon-upload"></i> new File</li>');
    // if clicked, call add function
    linkNew.click(function(){ add(callback); });
    linkNew.appendTo(ddUl);
    $(this).dropdown();
    // show();
}

function add(callback) {
    var newFile = $('<input type="file" name="file">').addClass('fileHideSafari');    
    newFile.attr('id', newId()).appendTo(fileForm);
    newFile.click();
    // callback is only called, if value is assigned
    newFile.change(function() {
        // check file name
        var $this = $(this);
        $this.attr('data-stlive-current', 'true');
        var newVal = $this.prettyVal();

        // in case we don't have a value, remove it
        if(!newVal) {
            $this.remove();
        }

        // check if file name is already in use
        fileForm.find('input[type=file]').each(function() {
            var $cur = $(this);
            if($cur.attr('data-stlive-current')) {
                $cur.removeAttr('data-stlive-current');
                return true;
            }
            if($cur.prettyVal() == newVal) {
                // if filename in use, ask to overwrite
                if(window.confirm('this filename is already in use.\nOverwrite the old '+newVal+'?')) {
                    $this.remove();
                } else {
                    $cur.remove();
                }
            }
        });
        fileNames.push(newVal);
        // call callback
        callback(newVal);
    });
}
function collectFileInputs() {
    return jqObj.find('input[type=file]').each(function(){
        if(!$(this).prettyVal()) {
            $(this).remove();
        }
    });
}

var counter = 0;
function newId() {
    return 'file_input_'+ ++counter;
}

// public interface
return {
    show: show,
    setId: setId,
    add: add,
    select_modal: select_modal,
    select_dropdown: select_dropdown,
    collectFileInputs: collectFileInputs
}

// scope wrapper
})();
