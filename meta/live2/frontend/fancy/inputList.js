//// define Inputset


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
        out: '<h1>${name}<small>${shortDesc}</small></h1>'
    },
    useLabel: false
  },
  { // SECTION
    type: 'section',
    required: ['name'],
    html: {
        out: '<legend><a href="#" onclick="$(this).popover(\'toggle\');return false;" data-title="${name}" data-content="${sectiondescription}" data-trigger="manual" >${name}</a></legend>'
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
        options : '<button class="btn" data-stlive-argname="${argname}" onclick="$(this).button(\'toggle\').change();return false;" type="button" name="${argname}" value="${options}">${options}</button>',
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
        out : '<input style="display:none;" type="file" name="${id}_hidden" id="${id}_hidden" ' + 
              'onchange="$(\'#\\\\${id}\').val(this.value).change();">'+
              '<input type="button" class="btn fileUploadButton" onclick="if(this.disabled)return false;document.getElementById(\'${id}_hidden\').click()" value="Select File" /> ' +
            ' <input id="${id}" data-stlive-argname="${argname}" type="text" style="color:#000;cursor:default;border:none;background-color:transparent;" class="forceDisabled" disabled="disabled"> '
    },
    useLabel: true
  }
];

// now register the set
for(var i = 0, c = null; c = inputs[i]; ++i) {
  registerInput(c.type, c.required, c.html, c.useLabel);
}
