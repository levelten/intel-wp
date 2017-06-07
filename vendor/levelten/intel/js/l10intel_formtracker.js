var _ioq = _ioq || [];

function L10iFormTracker(_ioq, config) {
    var ioq = _ioq;
    var io = _ioq.io;
    var $ = jQuery;


    this.init = function init() {
        var ths = this;
        $('form').not('.formtracker-0').on('submit', ths.eventHandler);
        //$('a').on('mouseover', {eventType: 'click'}, ths.eventHandler); // for testing event sends
    };

    this.eventHandler = function eventHandler(event) {
        var i, v;
        var $obj = $(this);
        var evtDef = {
          transport: 'beacon',
        };

        var eventType = 'submit';


        if ($obj.hasClass('prevent-formtracker')) {
          return;
        }

        var formTypeEnabled = {
            search: 1
        }

        var formTypeTitles = {
            search: 'Search'
        };

        var formType = '';

        // check for hrefType specified via class track-link-[type id]
        var classes = $obj.attr('class');
        classes = classes ? classes.split(' ') : [];
        for (i = 0; i < classes.length; i++) {
            v = classes[i];
            if (v.substr(0, 11) == 'track-form-') {
                v = v.substr(11);
                if (v == 'mode-valued') {
                    evtDef.mode = 'valued';
                }
                else {
                    formType = v;
                }
            }
        }

        if () {

        }

        if (!formTypeEnabled[hrefType]) {
            return;
        }

        if (!formTypeTitles[hrefType]) {
            evtDef.eventCategory = 'Form ' + eventType;
        }
        else {
            evtDef.eventCategory = hrefTypeTitles[hrefType] + ' form ' + eventType;
        }

        _ioq.push(['defEventHandler', evtDef, $obj, event]);

    };

    this.init();
}

_ioq.push(['providePlugin', 'formtracker', L10iFormTracker, {}]);