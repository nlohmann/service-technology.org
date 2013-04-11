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
        // console.log("param "+c+" is okay..");
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
    if(!this.valueOps) {
        return undefined;
    }
    if(!this.valueselector_interpreted) {
        // value Selector is build
        if(this.valueOps.selector) {
            var re = /\$\{(.+)\}/g;
            var vsi = this.valueOps.selector;
            while(vsi.match(re)) {
                var v = this.params[RegExp.$1];
                vsi = vsi.replace(re,v);
            }
            this.valueselector_interpreted = vsi;
        } else {
            return false;
        }
    }
    return $(this.dom).find(this.valueselector_interpreted).val();
}

Input.prototype.buildHtmlString = function(pName) {
    //console.log('buildHTML STring for: ' + this.params[pName] + ' (' + pName + ')');

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

Input.prototype.updateValue = function(val) {
// TODO: update value of element...
// Problem: how to do this in general
//
//
//  Problem solved for checkboxes...
    if(!this.valueOps) {
        return undefined;
    }


    // TODO
    if(!this.valueUpdateSelector_interpreted) {
        // value Selector is build
        if(this.valueOps.updateSelector) {
            var re = /\$\{(.+)\}/g;
            var vsi = this.valueOps.updateSelector;
            while(vsi.match(re)) {
                var v;
                if(RegExp.$1 == 'newValue') {
                    v = val;
                } else {
                    v = this.params[RegExp.$1];
                }
                vsi = vsi.replace(re,v);
            }
            this.valueUpdateSelector_interpreted = vsi;
        } else {
            return false;
        }
    }
    console.log(this.valueOps.updateCall);
    var el = $(this.dom).find(this.valueUpdateSelector_interpreted);
    el[this.valueOps.updateCall](val);

    return;
if(this.params.type == 'checkbox') {
    var cb = this.dom.getElementsByTagName("input")[0];
    if(val === true || val == "on") {
        cb.checked = "checked";
    } else {
        console.log("remove attr");
        cb.removeAttribute("checked");
        cb.checked = null;
    }
}

}

// add an Input
function registerInput(name, expParams, html, valueOps) {
    // create new class
    Input[name] = function(p, id) {
        Input.call(this, p, id);
        this.expectedParams = this.expectedParams.concat(expParams);
    }
    Input[name].prototype = new Input();

    // set for output creation
    Input[name].prototype.html = html;
    Input[name].prototype.valueOps = valueOps;
}

function make(html) {
    var d = document.createElement('span');
    d.innerHTML = html;
    return d.firstChild;
}


///// Input Set Manager
//InputSetManager = {
//    instanceID : 0,
//    instances : []
//};

// global function
// catch up event and send to the
// input set manager
// 15.11.2012: event handling disabled
//function inputEvent(m) {
//    var mId = m.split('#')[0];
//    InputSetManager[mId].handleEvent(m);
//}

// constructor
function InputSetManager() {
    this.inputs = [];
//    this.id = ++(inputSetManager.instanceID);
//    InputSetManager.instances[this.id]=this;
    this.inputsByArgname = {};    
}

InputSetManager.prototype.SetInputs = function(inputs) {
    this.inputData = inputs;
}

// InputSetManager.handleEvent = function(m) {
//    var iid = m.split('#').top();
    //forward to input
//    this.inputs[iid].handleEvent(m);
//}

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
                // this.curSec += ' active';
                var well = make('<div class="tab-content stBox"><h5>more Options:</h5></div>');
                well.appendChild(tabLinks);
                well.appendChild(make('<hr />'));
                form.appendChild(well);
                curWell = well;
            }
            var tLink = make('<li ' + (firstSec&&false?'class="active"':'') + '><a href=".sec' + i + '" data-toggle="tab">' + c.name + '</a></li>');
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

InputSetManager.prototype.update = function(params) {
    console.log("called update");
    var pLen = params.length;
    var paramsByArgname = {};
    for(var i=0; i < pLen; ++i) {
        var c = params[i].replace(/^\-+/, '');
        var cArr = c.split('=');
        var argName = cArr[0];
        console.log(cArr);
        var val = cArr[1] ? cArr[1] : true;
        paramsByArgname[argName] = val;
    }
    var iLen = this.inputs.length;
    for(var j=0; j<iLen; ++j) {
        var c = this.inputs[j];
        var val = false;
        if(c && c.params && c.params.name && paramsByArgname[c.params.name]) {
            val = paramsByArgname[c.params.name];
            paramsByArgname[c.params.name] = null;
        }
        if(c) {
            c.updateValue(val);
        }
    }
}

InputSetManager.prototype.onChange = function(callback) {
    // this is dirty: cloning this
    var t = this;
    $(this.form).change(function(){
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
                // if just standard file
                if(p !== '') {
                    p += '=';
                }
                p += v;
            }
            params.push(p);
        }
    }
    return params;
}
// public interface
return {
    registerInput: registerInput,
    InputSetManager: InputSetManager
}

// wrapper for hiding scope
})();
