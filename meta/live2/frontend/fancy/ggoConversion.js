
var PARSER; 
function buildParser(input) {
    PARSER = PEG.buildParser(input, {trackLineAndColumn: true});
}
$.get('ggoGrammar.pegjs', buildParser, "text");


function parse(ggoString) {
    var ggo = ggoString;
    var inputList = [];
    try {
        var parsed = PARSER.parse(ggo);
    } catch(e) {
        alert('Could not parse the ggo file: \n' + e);
        return [];
    }
    for(var i = 0, c = null; c = parsed[i]; ++i) {
        var conv = convertItem(c);
        if(conv)
            inputList.push(convertItem(c));
    }
    console.log(inputList);
    return inputList;
}


function convertItem(item) {
    // no hidden items

    if(item.unamed_opts_file) {
        return {
            type: 'file',
            argname : '',
            name : 'Standard File Input'
        };
    }

    if(item.hidden) return null
    
    // convert text to html
    function convText(t) {
        if(!t) return '';
        return t.replace(/\\n/gi, '<br />');
    }

    // filter section
    if(item.section) {
        return {
            type:'section',
            name: convText(item.section),
            sectiondescription: convText(item.sectiondesc)
        };
    }

    // set short, if not set
    if(item.short == '-') {
        item.short = '-' + item.long;
    }

    // generate default text input
    var input = {
        type: 'text',
        argname: '--' + item.long,
        name: item.long,
        desc: convText(item.desc)
    };
    input['default'] = '';
    if(typeof item['default'] != 'undefined') {
        input['default'] = item['default'];
    }

    // convert flags to checkboxes
    if(item.flag) {
        input.type = 'checkbox';
        if(item.flag.match(/^(true|1|yes|on)$/i)) {
            input.checked = 'checked="checked"';
            input['default'] = 'on';
        } else {
            input.checked = '';
            input['default'] = 'off';
        }
    }

    //convert enums to select
    if(item.values && item.values.length > 0) {
        input.type='select';
        if(item.values.length < 4) {
            input.type='radio_buttons';
        }
        input.options = [];
        valLen = item.values.length;
        for(var iVal = 0; iVal < valLen; ++iVal) {
            var c = item.values[iVal];
            // dont list default here,
            // default is handled extra
            if(c != item.default) {
                input.options.push(c);
            }
        }
        if(!input.default) {
            input.default = '<i class="icon-minus-sign"></i>';
        }
        
    }

    // FILES
    if (item.typestr == 'FILE' || item.typestr == 'FILENAME') {

        // assume:
        // (1)  if we find the word 'out' or 'output', we have an output file
        // (2)  an output file cannot be uploaded
        // (3)  an optional output file doesnt have to be selected
        //
        //
        // // TODO: wrong assume

        re = /out(\.| |put)/gi;
        if(!input.desc.match(re)) {
            input.type='file';
        } else if (item.optional) {
            if(item.argoptional) {
                input.type="checkbox";
                input['default'] = 'off';
            } else {
                input.type="text";
            }
        }
    }
    return input;
}

function initParse(toolName, callback) {
    // polling for PARSER.. ugly coding style..
    if(!PARSER) {
        //global function for timeout
        TO = function() { window.initParse(toolName, callback); };
        setTimeout('TO', 100);
    }

    var req = 'ggos/' + toolName + '.ggo';
    $.get(req,
            function(i) {var t=parse(i); callback(t);},
            'text').fail(function() { alert("Tool could not be loaded, try index.html?<toolname> (e.g. wendy)"); });
}

