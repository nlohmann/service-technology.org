// wrap scope
var GGO_CONVERSION = (function(){

// the parser object
var PARSER; 
// overwrite funcion later
function parser_ready(){}
function buildParser(input) {
    PARSER = PEG.buildParser(input, {trackLineAndColumn: true});
    parser_ready();
}
$.get('ggoGrammar.pegjs', buildParser, "text");


function parse(ggoString, toolName) {
    var ggo = ggoString;
    var inputList = [];
    try {
        var parsed = PARSER.parse(ggo);
    } catch(e) {
        alert('Could not parse the ggo file: \n' + e);
        return [];
    }
    
    // create the title
    // search the purpose
    var purpose = '';
    for(var j = 0; j < 4; j++) {
        var p = parsed[j].purpose;
        if(p) {
            purpose = p.replace(/(\\n)+/gi, "<br />");
            break;
        }
    } 
    inputList.push({
        type: "title",
        name: toolName.toUpperCase(),
        shortDesc: purpose
    });

    for(var i = 0, c = null; c = parsed[i]; ++i) {
        var conv = convertItem(c, toolName);
    if(conv && conv.type == "section") {
        var l = inputList.length;
        if (l > 0) {
        var t = inputList[l-1];
        // if we have two section,
        // forget the empty section
        if (t.type == 'section'){
            inputList[l-1] = conv;
            continue;
        }
        }
    }
        if(conv)
            inputList.push(conv);
    }
    console.log(inputList);
    return inputList;
}


function convertItem(item, toolName) {
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

    // purpose to title
    if(item.purpose) return false;

    // filter section
    if(item.section) {
    var sd = convText(item.sectiondesc);
    // escape html entities
    sd = $("<div/>").text(sd).html();
        return {
            type:'section',
            name: convText(item.section),
            sectiondescription: sd
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
    if(!PARSER) {
        // if parser is not yet loaded,
        // overwrite the parser_ready function
        parser_ready = function() { initParse(toolName, callback); };
    }

    var req = 'ggos/' + toolName + '.ggo';
    $.get(req,
            function(i) {var t=parse(i, toolName); callback(t);},
            'text').fail(function() { alert("Tool could not be loaded, try index.html?<toolname> (e.g. wendy)"); });
}

// public interface
return {
    initParse: initParse
};

// wrap scope
})();
