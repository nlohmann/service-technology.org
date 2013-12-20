// wrapper for hiding scope
var VIZ = (function(){

    vizObjects = [];

    function create(t) {
        // call library Viz.js
        var svg = Viz(t, 'svg');
        console.log(svg);
        vizObjects.push(svg);

        // return the id (array index)
        return vizObjects.length - 1;
    }

    function show(id) {
        var svg = vizObjects[id];
        var viz_container = $('#viz_container');
        viz_container.find('.modal-body').html(svg);
        viz_container.modal();
    }

    return {
        create: create,
        show: show
    };

})();
