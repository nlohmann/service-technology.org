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
// COMMENT: behaviour of sections as tabs is written in InputSetManager.Create()
        out: ' ',
        useLabel: false
    },
  },
  {
    // FLAG (TRUE/FALSE)
    type: 'checkbox',
    required: ['name', 'argname'],
    html: {
       out: '<input type="checkbox" onclick="this.value=this.value?\'\':\'true\'" onchange="this.checked=this.value?\'checked\':\'\';" data-stlive-argname="${argname}" value="" name="${argname}" id="${id}">',
       useLabel: true
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
        out : '<select name="${argname}" id="${id}" data-stlive-argname="${argname}">'
            + '<option value="">${default}</option>${options}</select>',
        useLabel: true
    }
  },
  { // ENUM SMALL
    type: 'radio_buttons',
    required: ['name', 'argname', 'options', 'default'],
    html : {
    // each optons is a button
    // need some js to active the changes
        options : '<button class="btn" onclick="$(${id}).val(this.value).change();return false;" type="button" value="${options}">${options}</button>',
    // creating a button group
        out : '<input type="hidden" value="" id="${id}" onchange="$(\'#${id}_grp button[value=\'+this.value+\']\').button(\'toggle\')">'
            + '<div class="btn-group" id="${id}_grp" data-toggle="buttons-radio"><button class="btn active" onclick="$(${id}).val(\'\').change();return false;">${default}</button>${options}</div>',
    useLabel: true
    }
    //command : {
      //  out : '-${argname}="${value}"'
    //},
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
    }
  }
];

// now register the set
for(var i = 0, c = null; c = inputs[i]; ++i) {
  INPUTS.registerInput(c.type, c.required, c.html);
}

// wrapper for hiding scope
})();
