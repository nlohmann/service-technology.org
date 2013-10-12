

// class
HISTORY = function(getStateFunction, setStateFunction, undoBtn, redoBtn){
    // undo stack
    this.history = [];

    //redo stack
    this.future = [];
    this.getState = getStateFunction;
    this.setState = setStateFunction;
    this.changeHandlers = [];

    this.undoBtn = undoBtn;
    this.redoBtn = redoBtn;

    var u = this.undo;
    var r = this.redo;
    var t = this;
    undoBtn.click(function(){if(!$(this).hasClass('disabled')) u.call(t);});
    redoBtn.click(function(){if(!$(this).hasClass('disabled')) r.call(t);});
}


// public interface
HISTORY.prototype = (function() {
    
    //private could go here...
    var changeDisabled = function() {
        if(this.undoPossible()) {
               this.undoBtn.removeClass('disabled');
           } else {
               this.undoBtn.addClass('disabled');
        }
        if(this.redoPossible()) {
               this.redoBtn.removeClass('disabled');
           } else {
               this.redoBtn.addClass('disabled');
        }
    };

    return {
        //public
        addState: function() {
            var state = this.getState();

            // the false state is no state..
            if(!state) return;

            // the same state is no new state
            if(state == this.history[this.history.length-1]) {
                return;
            }
            this.history.push(state);
            // we have done something new, so
            // we dont know the future...
            this.future = [];
            this.onchange();
        },
        undo: function() {
            var stateForRedo = this.history.pop();
            this.future.push(stateForRedo);
            this.setState(this.history[this.history.length-1]);
            this.onchange();
        },
        redo: function() {
            var stateForUndo = this.future.pop();
            this.history.push(stateForUndo);
            this.setState(stateForUndo);
            this.onchange();
        },
        undoPossible: function() {
            // the end of history is now
            return this.history.length > 1;
        },
        redoPossible: function() {
            return this.future.length > 0;
        },
        onchange: function(handler) {
            if(typeof handler == 'function') {
                this.changeHandlers.push(handler);
            } else {
                for(var i = 0, c = null; c = this.changeHandlers[i]; ++i) {
                    c();
                }
            }
            // not an extra handle, because private
            changeDisabled.call(this);
            
        }
    }

//scope wrape
})(); 
