// wrapper for hiding scope
var INPUT_MANAGER = (function(){

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
        if(typeof INPUTS[c.type] != 'function') {
            console.log('unknown input: '+c.type + ' type is ' + typeof INPUTS[c.type]);
            continue;
        }
        // create input object
        this.inputs[i] = new INPUTS[c.type](c,'input'+i); //overwrite by object
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
        if(params[i].match(/^[^\-]/gi)) { // stupid special case
            if(!paramsByArgname.$) {
                paramsByArgname.$ = params[i];
            } else {
                this.unmatchedParams.push(params[i]);
            }
            continue;
        } else {
            var c = params[i].replace(/^\-+/, '');
            var argName;
            var argVal;
            var fstEq = c.indexOf('=');
            var fstSp = c.indexOf(' ');

            // split at first equal
            if(fstEq >= 0 && (fstEq > fstSp | fstSp < 0)) {
                argName = c.slice(0,fstEq);
                argVal = c.slice(fstEq+1, c.length);
            // split at first space
            } else if (fstSp >= 0 && (fstSp > fstEq | fstEq < 0)) {
                argName = c.slice(0,fstSp);
                argVal = c.slice(fstSp+1, c.length);
            // no param/value
            } else {
                argName = c;
                argVal = true;
            }
            if(!paramsByArgname[argName]) {
                paramsByArgname[argName] = argVal;
                unmatchedParamsByArgname[argName] = params[i];
            } else {
                this.unmatchedParams.push(params[i]);
            }
        }
    }

    // iterate through all inputs
    var iLen = this.inputs.length;
    for (var j=0; j<iLen; ++j) {
        var c = this.inputs[j];
        var val = false;

        // is this a key-less input?
        if (c && c.params && c.params.argname === '') {
            if(paramsByArgname.$) {
                c.updateValue(paramsByArgname.$);
                unmatchedParamsByArgname.$ = null;
            } else {
                c.updateValue("");
            }
            continue;
        }

        // check, if input and value are existing
        if (c && c.params && c.params.name && paramsByArgname[c.params.name]) {
            val = paramsByArgname[c.params.name];
            unmatchedParamsByArgname[c.params.name] = null;
        } else if (c && c.params && c.params.shortArgname && paramsByArgname[c.params.shortArgname]) {
            val = paramsByArgname[c.params.shortArgname];
            unmatchedParamsByArgname[c.params.shortArgname] = null;
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
        callback.call(this, p);
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
return InputSetManager;

// wrapper for hiding scope
})();
