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
        l.setAttribute('for', this.params['argname']);
    }
    l.className = 'control-label';
    l.appendChild(d.createTextNode( this.params['name'] ));

    // controls
    var ctrl = d.createElement('div');
    ctrl.className = 'controls';
    ctrl.appendChild(this.toElementSimple());
    if(this.params.desc) {
        var desc = make('<div><small><em>'+this.params.desc+'</em></small></div>');
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
    var out = this.buildHtmlString('out');
    console.log(out);
    return make(out);
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
        console.log(pName + " is  arrray ");
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

Input.prototype.toElement = function() {
    var el;
    if(this.useLabel) {
        el = this.toLabelledElement();
    } else {
        el = this.toElementSimple();
    }
    return el;
}

// add an Input
function registerInput(name, expParams, html, useLabel) {
    // create new class
    Input[name] = function(p, id) {
        Input.call(this, p, id);
        this.expectedParams = this.expectedParams.concat(expParams);
    }
    Input[name].prototype = new Input();

    // set for output creation
    Input[name].prototype.html = html;
    Input[name].prototype.useLabel = useLabel;
}

function make(html) {
    var d = document.createElement('span');
    d.innerHTML = html;
    return d;
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
    for(var i = 0, c = null; c = this.inputData[i]; ++i) {
        if(typeof Input[c.type] != 'function') {
            console.log('unknown input: '+c.type + ' type is ' + typeof Input[c.type]);
            continue;
        }
        this.inputs[i] = new Input[c.type](c,'input'+i); //overwrite by object
        if(!this.inputs[i].checkParams()) {
            continue;
        }
        form.appendChild(this.inputs[i].toElement());
    }
    dom.appendChild(form);
}

// public interface
return {
    registerInput: registerInput,
    InputSetManager: InputSetManager
}

// wrapper for hiding scope
})();
