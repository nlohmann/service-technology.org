/*
   this script converts the given form values
   and creates a command line call..
*/


Form2Command = {
    toolName : '',
    args : []
}

Form2Command.listen = function() {
    // event listening
   $('#form_container .toolForm').change(function(){
           Form2Command.args = []; // reset

           // texts, select, file inputs
           $(this).find('input[type=text], button.btn.active, input[type=file], select').each(
               function() {
                    if (!this.value) return;

                    var argname = this.getAttribute('data-stlive-argname');
                    var def = this.getAttribute('data-stlive-default');
                    var val = this.value;

                    if(def == val) {
                        return;
                    }

                    // quote if necessary
                    if(val.indexOf(' ') > -1) {
                        val = '"' + val + '"';
                    }
                    var a = argname + '=' + val;
                    // special case, if no attribute
                    if(argname == '') {
                        a = val;
                    }
                    Form2Command.args.push(a);
               }
            );
           // checkbox inputs
           $(this).find('input[type=checkbox]').each(
               function() {
                   var a = this.getAttribute('data-stlive-argname');
                   var def = this.getAttribute('data-stlive-default');
                   if (!this.checked && def != 'off') {
                       a += '=off';
                       Form2Command.args.push(a);
                   } else if (this.checked && def != 'on') {
                       Form2Command.args.push(a);
                   }
               }
            );
           // ...

           Form2Command.update();


       });
       // move file inputs to our new form
       $('#form_container .toolForm input[type=file]').each(
           function(){
               $('#json-output').closest('form').append(this);
           }
       );

   Form2Command.update();
}
Form2Command.update = function() {
   // create command
   var c = Form2Command.toolName + ' ' + Form2Command.args.join(' ');
   $('#command-output').val(c);

   //create json
   var j = '{\n    "tool": "'+Form2Command.toolName+'",\n    "parameters": [';
   for(var i = 0, c = null; c = Form2Command.args[i]; ++i) {
       if(i > 0) {
           j += ",";
       }
       j += "\n       ";
       j += '"' + c.replace(/\"/g, '\\"') + '"';
   }
   j += '\n    ] \n}';
   $('#json-output').val(j);
}
