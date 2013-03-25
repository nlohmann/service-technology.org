//// define Inputset

// wrapper for hiding scope
(function(){

var inputs = [
  { // TEXT INPUT
    type: 'text',
    required: ['name', 'argname'],
    html: {
        out: '<input data-stlive-argname="${argname}" name="${argname}" id="${argname}" class="input-large" type="text" >'
    },
    useLabel: true
  },
  { // TITLE
    type: 'title',
    required: ['name', 'shortDesc'],
    html: {
        out: '<h1 style="margin-bottom:0px;">${name}</h1><div class="muted"><small>${shortDesc}</small></div><hr />'
    },
    useLabel: false
  },
  { // SECTION
    type: 'section',
    required: ['name'],
    html: {
        out: '<legend><span id="${id}" title="${name}" data-content="${sectiondescription}">${name}</span></legend>' +
         // the img-tag is a hack to include javascript
         // loading the icons, because they are loaded anyway
         '<img src="bootstrap/img/glyphicons-halflings.png" style="display:none;" onload="'+
         // inserting the popover if there is text
         'var e=$(\'#${id}\');' +
         'if(e.attr(\'data-content\'))e.popover().addClass(\'btn-link\');' + 
         '" />'
    },
    useLabel: false
  },
  {
    // FLAG (TRUE/FALSE)
    type: 'checkbox',
    required: ['name', 'argname'],
    html: {
        out : '<input data-stlive-default="${default}" type="checkbox" data-stlive-argname="${argname}" name="${argname}" id="${argname}" ${checked}>'
    },
    useLabel: true
  },
  { // SUBMIT BUTTON
     type: 'submit',
     required: ['name'],
     html: {
        out: '<hr><input type="submit" class="btn btn-primary" value="${name}"/>'
     },
     useLabel: false
  } ,
  { // ENUM
    type: 'select',
    required: ['name', 'argname', 'options'],
    html : {
        options : '<option value="${options}">${options}</option>',
        out : '<select name="${argname}" id="${argname}" data-stlive-argname="${argname}">${options}</select>'
    },
    useLabel: true
  },
  { // ENUM SMALL
    type: 'radio_buttons',
    required: ['name', 'argname', 'options', 'default'],
    html : {
    // each optons is a button
    // need some js to active the changes
        options : '<button class="btn" data-stlive-argname="${argname}" onclick="$(this).button(\'toggle\').change();return false;" type="button" name="${argname}" value="${options}">${options}</button>',
    // creating a button group
        out : '<div class="btn-group" data-toggle="buttons-radio"><span class="btn active" onclick="$(this).button(\'toggle\').change();return false;">${default}</span>${options}</div>'
    },
    //command : {
      //  out : '-${argname}="${value}"'
    //},
    useLabel: true
  },
  {
    type: 'file',
    required:  ['name', 'argname'],
    html : {
    // first the hidden field which eventually holds the file
    // (will be copied to json-form later
        out : '<input style="display:none;" type="file" name="${id}_hidden" id="${id}_hidden" ' + 
              'onchange="$(\'#\\\\${id}\').val(this.value).change();">'+
          // the visible button just triggers the hidden file input
              '<div class="input-prepend" ' +
          'onclick="if(this.disabled)return false;document.getElementById(\'${id}_hidden\').click()">' + 
          '<p type="button" class="add-on btn" value="Select File">Select File</p>' +
          // and here we let the user see the filename (readOnly)
          // the parameter-name is taken by this input element
            '<input id="${id}" data-stlive-argname="${argname}" type="text" style="color:#000;cursor:default;border:none;background-color:transparent;" class="input-medium forceDisabled" disabled="disabled"></div>'
    },
    useLabel: true
  }
];

// now register the set
for(var i = 0, c = null; c = inputs[i]; ++i) {
  INPUTS.registerInput(c.type, c.required, c.html, c.useLabel);
}

// wrapper for hiding scope
})();
