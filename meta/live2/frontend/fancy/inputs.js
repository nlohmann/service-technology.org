// wrapper for hiding scope
var INPUTS = (function(){

// Input abstract class
function Input(params,id) {
    this.params = params;
    if(id) {
        this.params.id = id;
    }
    this.expectedParams = [
        'type'
    ];

}

Input.prototype.checkParams = function () {
    for(var i = 0, c = null; c = this.expectedParams[i]; ++i) {
        if(typeof this.params[c] == 'undefined') {
            console.log("Parameter "+c+" not given...");
            return false;
        }
    }
    return true;
}

Input.prototype.toLabelledElement = function(element) {
    var d = document;
    // Label
    var l = d.createElement('Label');

    // expecting argname to be the id
    // of the input...
    if(this.params['argname']) {
        l.setAttribute('for', this.params['id']);
    }
    l.className = 'control-label';
    l.appendChild(d.createTextNode( this.params['name'] ));

    // controls
    var ctrl = d.createElement('div');
    ctrl.className = 'controls';
    ctrl.appendChild(this.toElementSimple());
    if(this.params.desc) {
        var desc = make('<div class="help-block"><small><em>'+this.params.desc+'</em></small></div>');
        ctrl.appendChild(desc);
    }

    // control-group
    var lEl = d.createElement('div');
    lEl.className = "control-group";

    // append label & control
    lEl.appendChild(l);
    lEl.appendChild(ctrl);

    return lEl;
}
Input.prototype.toElementSimple = function() {
    var out = '<span>' + this.buildHtmlString('out') + '</span>';
    var elSmpl = make(out);
    this.dom = elSmpl;
    return elSmpl;
}

Input.prototype.getValue = function() {
    var el = document.getElementById(this.params.id);
    return el? el.value : false;
}

Input.prototype.updateValue = function(val) {

    // default is empty string
    if(!val) val = '';
    if(val == this.params.default) val = '';

    // find the element
    var el = document.getElementById(this.params.id);
    if(!el) return false;
    el.value = '' + val;

    // trigger onchange
    if(typeof el.onchange == 'function') {
        el.onchange();
    }
}

/*
 * builds the html-string for the input
 */
Input.prototype.buildHtmlString = function(pName) {

    // templater regex
    var re = /\$\{([a-zA-Z]+\w*)\}/i;
    var html = this.html[pName];

    // sanity checks
    if(!html) {
        if(typeof this.params[pName] != undefined) {
            return this.params[pName];
        }
        else {
            console.log("Param " + param + " not defined, leave empty");
            return '';
        }
    }

    // if this is an array of values
    // recall function with every value
    if(Array.isArray(this.params[pName])) {
        var htmlArr = '';
        var a = this.params[pName].slice() // clone;
        for (var i = 0, c = null; c = a[i]; ++i) {
            this.params[pName] = c;
            htmlArr += this.buildHtmlString(pName);
        }
        this.params[pName] = a;
        return htmlArr;
    }

    // now replace all placeholders
    while(re.test(html)) {
        var p = RegExp.$1;
        var val = '';

        // no recursion, just replace if
        // looking for current param
        if(p == pName) {
            val = this.params[pName];
        } else {
            val = this.buildHtmlString(p);
        }
        html = html.replace(re, val);
    }

    return html;
}

Input.prototype.toElement = function(section) {
    var el;
    if(this.html && this.html.useLabel) {
        el = this.toLabelledElement();
    } else {
        el = this.toElementSimple();
    }
    if(section != 'topSec') {
        var c = el.className;
        c += ' tab-pane '+section;
        el.className = c;
    }
    return el;
}


// create a new Input-Class
function registerInput(name, expParams, html) {
    // create new class
    Input[name] = function(p, id) {
        Input.call(this, p, id);
        this.expectedParams = this.expectedParams.concat(expParams);
    }
    Input[name].prototype = new Input();

    // set for output creation
    Input[name].prototype.html = html;
}

/**
 * helper function to create html-elements from string
 */
function make(html) {
    var d = document.createElement('span');
    d.innerHTML = html;
    return d.firstChild;
}


// constructor
function InputSetManager() {
    this.inputs = [];
    this.inputsByArgname = {};    
    this.unmatchedParams = [];    
}

InputSetManager.prototype.SetInputs = function(inputs) {
    this.inputData = inputs;
}

// TODO: a lot of magic html creation is done is this function...
// TODO: maybe better realise this as Inputs ???
InputSetManager.prototype.Create = function(dom) {
    var form = document.createElement('form');
    form.className = 'form-horizontal toolForm';
    var curWell = document.createElement('div');
    curWell.className = 'stBox';
    form.appendChild(curWell);

    var tabLinks = make('<ul class="nav nav-pills" />');

    this.curSec = 'topSec';
    for(var i = 0, c = null; c = this.inputData[i]; ++i) {
        // check if we have an input for that
        if(typeof Input[c.type] != 'function') {
            console.log('unknown input: '+c.type + ' type is ' + typeof Input[c.type]);
            continue;
        }
        // create input object
        this.inputs[i] = new Input[c.type](c,'input'+i); //overwrite by object
        if(!this.inputs[i].checkParams()) {
            continue;
        }
        // if we have a section, create a new tab and add to tab
        if(c.type == 'section') {
            var firstSec = this.curSec == 'topSec';
            this.curSec = 'sec'+i;
            if(firstSec) {
                form.appendChild(make("<h5 style=\"margin-bottom:-20px;\">more Options:</h5>"));
                var well = make('<div class="tab-content stBox"></div>');
                well.appendChild(tabLinks);
                well.appendChild(make('<hr />'));
                form.appendChild(well);
                curWell = well;
            }
            var tLink = make('<li><a href=".sec' + i + '" data-toggle="tab">' + c.name + '</a></li>');
            tabLinks.appendChild(tLink);

        }
        curWell.appendChild(this.inputs[i].toElement(this.curSec));

        // if we have an argname, put reference to argname list
        if (typeof this.inputs[i].params.argname != 'undefined' && this.inputs[i].params.argname) {
            this.inputsByArgname[this.inputs[i].params.argname] = this.inputs[i];
        }
    }
    dom.appendChild(form);
    this.form = form;
}

/** update values in form
 * params is a list of parameters as they could in command line
 */
InputSetManager.prototype.update = function(params) {
    var pLen = params.length;
    var unmatchedParamsByArgname = {};

    this.unmatchedParams = [];
    // first order by argname
    var paramsByArgname = {};
    for(var i=0; i < pLen; ++i) {

        params[i] = params[i] + '';
        // this a value without a key (standard file input)
        if(params[i].match(/^[^\-][^\=]+$/gi)) { // stupid special case
            if(!paramsByArgname.$) {
                paramsByArgname.$ = params[i];
            } else {
                this.unmatchedParams.push(params[i]);
            }
            continue;
        }

        var c = params[i].replace(/^\-+/, '');
        var cArr = c.split('=');
        var argName = cArr[0];
        var val = cArr.length > 1 ? cArr[1] : true;
        if(!paramsByArgname[argName]) {
            paramsByArgname[argName] = val;
            unmatchedParamsByArgname[argName] = params[i];
        } else {
            this.unmatchedParams.push(params[i]);
        }
    }

    // iterate through all inputs
    var iLen = this.inputs.length;
    for(var j=0; j<iLen; ++j) {
        var c = this.inputs[j];
        var val = false;

        // is this a key-less input?
        if(c && c.params && c.params.argname === '') {
            if(paramsByArgname.$) {
                c.updateValue(paramsByArgname.$);
                unmatchedParamsByArgname.$ = null;
            } else {
                c.updateValue("");
            }
            continue;
        }

        // check, if input and value are existing
        if(c && c.params && c.params.name && paramsByArgname[c.params.name]) {
            val = paramsByArgname[c.params.name];
            unmatchedParamsByArgname[c.params.name] = null;
        }
        // call the update for the input
        if(c) {
            c.updateValue(val);
        }
    }
    for(var unmatchedInd in paramsByArgname) {
        if(unmatchedParamsByArgname[unmatchedInd]) {
            this.unmatchedParams.push(unmatchedParamsByArgname[unmatchedInd]);
        }
    }
}

// the callback function is called when something is changed
// the callback gets a list of parameters as parameters
InputSetManager.prototype.onChange = function(callback) {
    // cloning this
    // not that bad, as we have to create a closure anyway
    var t = this;
    $(this.form).find('input, select').change(function(){ 
        var p = InputSetManager.prototype.getValues.call(t);
        callback(p);
    });
    // initialize empty
    callback([]);
}

InputSetManager.prototype.getValues = function() {
    var params = [];
    var iLen = this.inputs.length;
    for(var j=0; j<iLen; ++j) {
        var c = this.inputs[j];
        var v = c.getValue();
        if(v) {
            var p = c.params.argname;
            // flag has no value
            if(c.params.type != 'checkbox') {
                // if just standard file (just value, no key)
                if(p !== '') {
                    p += '=';
                }
                p += v;
            }
            params.push(p);
        }
    }
    params = params.concat(this.unmatchedParams);
    return params;
}
// public interface
return {
    registerInput: registerInput,
    InputSetManager: InputSetManager
}

// wrapper for hiding scope
})();
