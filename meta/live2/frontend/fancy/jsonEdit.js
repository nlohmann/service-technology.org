

var values = {};

var jqIn;
var args = [];

var currentArgs = [];
var prefix = '';

function createTypeahead(jqObj, inputs) {
    jqIn = jqObj;
    var inputsLen = inputs.length;
    var reDash = /^\-+/;
    for(var i = 0; i < inputsLen; ++i) {
        var c = inputs[i];
        if(c.argname) {
            var n = c.argname.replace(reDash, '');
            args.push(n);
            values[n] = {
                argname : c.argname,
                isFlag : c.type == 'checkbox',
                options: c.options,
                default: c.default
            }
        }
        if(c.shortArgname && c.shortArgname != '-') {
            var n = c.shortArgname.replace(reDash, '');
            args.push(n);
            values[n] = {
                argname : c.argname,
                isFlag : c.type == 'checkbox',
                options: c.options,
                default: c.default
            };
        }
        var optLen;
        if(c.options && (optLen = c.options.length)) {
            for(var j = 0; j < optLen; ++j) {
                // assume argname is given as string
                var n = c.argname.replace(reDash, '');
                args.push(n + '=' + c.options[j]);
            }
        }
        if(c.default && c.type!='checkbox' && c.default.indexOf('<') == -1) {
            // assume argname is given as string
            var n = c.argname.replace(reDash, '');
            args.push(n + '=' + c.default);
        }
    }
    resetTypeahead();
    jqObj.closest('form').submit(
            function(){
                jqObj.val(addParam(jqObj.val()));
                return false;
            }
    );
}

function source(q) {

    var fst = q.charAt(0) == '-';
    var snd = q.charAt(1) == '-';

    var lead = '';
    if(fst) { leader = '-';
        if(snd) leader = '--';
    } else {
        return args;
    }

    var leadArgs = args.map( function(s){return leader + s;} );
    return currentArgs.concat(leadArgs);
}

function resetTypeahead() {
    jqIn.typeahead({source:source, updater:updater});
}

function updater(leading_dashes_item) {
    var item = leading_dashes_item.replace(/^\-+/, '');;

    var ioEq = item.indexOf('=');
    if( item.indexOf('=') > -1) {
        var keyVal = item.split('=');
        var p = values[keyVal[0]].argname + '=' + keyVal[1];
        currentArgs = [];

        // addParam(p);
        // return '';
        return p;

    } else {
        var val = values[item];
        if(!val) {
            alert('somewhere something went terribly wrong');
        }
        if(val.isFlag) {
            //addParam(val.argname);
            //return '';
            return val.argname;
        }
        var cleanArgname = val.argname;
        if(val.options) {
            var ca = [];
            var oLen = val.options.length;
            console.log(val.options);
            for(var i = 0; i < oLen; ++i) {
                var c = val.options[i];
                ca.push(cleanArgname + '=' + c);
            }
            if(val.default && val.default.indexOf('<') == -1) {
                ca.push(cleanArgname + '=' + val.default);
            }
            // currentArgs = ca;
        }
        return cleanArgname + '=';
    }
    alert('unreachable code ???');
}

function addParam(p) {
    if(!p) return '';
    if(p.charAt(p.length -1) == '=') return p;
    var jqTa = jqIn.parent().parent().find('textarea'); // shortcut from global scope
    var text = jqTa.val();
    var re1 = /^\-+/;
    var re2 = /\=.+$/;
    try{
        var inObj = JSON.parse(text);
        var pLen = inObj.parameters.length;
        var pInserted = false;
        for (var i = 0; i < pLen; ++i) {
            var cur = inObj.parameters[i];
            var oldArg = cur.replace(re1, '').replace(re2, '');
            var newArg = p.replace(re1, '').replace(re2, '');
            if(oldArg.toUpperCase() == newArg.toUpperCase()) {
                inObj.parameters[i] = p;
                pInserted = true;
                break;
            }
        }
        if(!pInserted) {
            inObj.parameters.push(p);
        }
        text = JSON.stringify(inObj, null, 4);
        jqTa.val(text);
    }
    catch(e) { // stuff in textare is no valid json
        text = text + '\n' + p;
        jqTa.val(text);
        jqTa.change();

        // possible extension:
        // 1st try: regex for argname
        // 2nd try: try to use regex /\]\s*\}\s*$/
        //          and insert param before
        // 3rd try the latest of the match above
    }
    return '';
}

function watchJsonBox(jqObj) {
    var alrt = $('<p style="margin-top:1em;margin-bottom:0">');
    alrt.addClass('alert alert-error');
    alrt.html('<h4>No valid JSON</h4><div/>');
    alrt.hide();
    alrt.insertBefore(jqObj);

    jqObj.change(function() {
        var s = jqObj.val();
        try {
            JSON.parse(s);
            jqObj.parent('div').removeClass('error');
            alrt.fadeOut();
        } catch(e) {
            jqObj.parent('div').addClass('error');
            alrt.find('div').text(e);
            alrt.fadeIn();
        }
    });
}