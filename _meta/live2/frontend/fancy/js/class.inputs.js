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
            console.log(this);
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

    el.setAttribute('data-stlive-dirty', 'dirty');

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

// this object holds each class, which represents an input element
var inputClasses = {};


// create a new Input-Class
function registerInput(name, expParams, html) {
    // create new class
    inputClasses[name] = function(p, id) {
        Input.call(this, p, id);
        this.expectedParams = this.expectedParams.concat(expParams);
    }
    inputClasses[name].prototype = new Input();

    // set for output creation
    inputClasses[name].prototype.html = html;
}

/**
 * helper function to create html-elements from string
 */
function make(html) {
    var d = document.createElement('span');
    d.innerHTML = html;
    return d.firstChild;
}

$.ajax({dataType: "json", url: 'data/inputSet.json', global:false, async: false, success: function(is){
    INPUT_SET = is;
    // load the input set, defined in inputSet.json
    for(var i = 0, c = null; c = INPUT_SET[i]; ++i) {
      registerInput(c.type, c.required, c.html);
    }
}});

// public interface
return inputClasses;

// wrapper for hiding scope
})();
