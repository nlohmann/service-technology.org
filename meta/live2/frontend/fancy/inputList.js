//// define Inputset

// wrapper for hiding scope
(function(){

// small helper function for checkboxes
$.fn.setChecked = function(v) { this.attr('checked', v); }

var inputs = [
  { // TEXT INPUT
    type: 'text',
    required: ['name', 'argname'],
    html: {
        out: '<input data-stlive-argname="${argname}" name="${argname}" id="${id}" class="input-large" type="text" >',
        useLabel: true
    },
    valueOperations: {
        selector: 'input#${id}',
        updateSelector: 'input#${id}',
        updateCall: 'val',
        updateParams: '${newValue}'
    }
  },
  { // TITLE
    type: 'title',
    required: ['name', 'shortDesc'],
    html: {
        out: '<h1 style="margin-bottom:0px;">${name}</h1><div class="muted"><small>${shortDesc}</small></div><hr />',
        useLabel: false
    },
  },
  { // SECTION
    type: 'section',
    required: ['name'],
    html: {
 /*       out: '<legend><span id="${id}" title="${name}" data-content="${sectiondescription}">${name}</span></legend>' +
         // the img-tag is a hack to include javascript
         // loading the icons, because they are loaded anyway
         '<img src="bootstrap/img/glyphicons-halflings.png" style="display:none;" onload="'+
         // inserting the popover if there is text
         'var e=$(\'#${id}\');' +
         'if(e.attr(\'data-content\'))e.popover().addClass(\'btn-link\');' + 
         '" />'
*/
         out: ' ', // <p class="well-small well"><small><em>${sectiondescription}</small></em></p>'
        useLabel: false
    },
  },
  {
    // FLAG (TRUE/FALSE)
    type: 'checkbox',
    required: ['name', 'argname'],
    html: {
       out: '<input data-stlive-default="${default}" type="checkbox" data-stlive-argname="${argname}" name="${argname}" id="${id}" ${checked}>',
       useLabel: true
    },
    valueOperations: {
        selector: 'input#${id}:checked',
        updateSelect: 'input#${id}:unchecked',
        // TODO: need to define this call...
        updateCall: 'setChecked', // this is an extra jQuery Plugin, see above
        updateParam: '${newValue}'
    }
  },
  { // SUBMIT BUTTON
     type: 'submit',
     required: ['name'],
     html: {
         out: '<hr><input type="submit" class="btn btn-primary" value="${name}"/>',
         useLabel: false
     },
  },
  { // ENUM
    type: 'select',
    required: ['name', 'argname', 'options'],
    html : {
        options : '<option value="${options}">${options}</option>',
        out : '<select name="${argname}" id="${id}" data-stlive-argname="${argname}">${options}</select>',
        useLabel: true
    },
    valueOperations: {
        selector: 'input#${id}'
    }
  },
  { // ENUM SMALL
    type: 'radio_buttons',
    required: ['name', 'argname', 'options', 'default'],
    html : {
    // each optons is a button
    // need some js to active the changes
        options : '<button class="btn" data-stlive-argname="${argname}" onclick="$(this).button(\'toggle\').change();return false;" type="button" name="${argname}" value="${options}">${options}</button>',
    // creating a button group
        out : '<div class="btn-group" data-toggle="buttons-radio"><span class="btn active" onclick="$(this).button(\'toggle\').change();return false;">${default}</span>${options}</div>',
    useLabel: true
    },
    //command : {
      //  out : '-${argname}="${value}"'
    //},
    valueOperations: {
        selector: 'button[data-stlive-argname=${argname}].active'
    }
  },
  {
    type: 'file',
    required:  ['name', 'argname'],
    html : {
    // first the hidden field which eventually holds the file
    // (will be copied to json-form later
        out : '<input style="display:none;" type="file" name="${id}_hidden" id="${id}_hidden" ' + 
              'onchange="if(this.style.display==\'none\')$(\'#\\\\${id}\').val(this.value).change();">'+
          // the visible button just triggers the hidden file input
              '<div class="input-prepend" ' +
          'onclick="if(this.disabled)return false;document.getElementById(\'${id}_hidden\').click()">' + 
          '<p type="button" class="add-on btn" value="Select File">Select File</p>' +
          // and here we let the user see the filename (readOnly)
          // the parameter-name is taken by this input element
            '<input id="${id}" data-stlive-argname="${argname}" type="text" style="color:#000;cursor:default;border:none;background-color:transparent;" class="input-medium forceDisabled" disabled="disabled"></div>',
        useLabel: true
    },
    valueOperations: {
        selector: 'input#${id}'
    }
  }
];

// now register the set
for(var i = 0, c = null; c = inputs[i]; ++i) {
  INPUTS.registerInput(c.type, c.required, c.html, c.valueOperations);
}

// wrapper for hiding scope
})();
