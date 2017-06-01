var _l10iq = _l10iq || [];

function L10iSocialTracker(_ioq, config) {
    var ioq = _ioq;
    var io = _ioq.io;
    var $ = jQuery;


    this.init = function init() {
        var ths = this;
        //$('a').not('.linktracker-0').on('click', ths.eventHandler);
        //$('a').on('mouseover', {eventType: 'click'}, ths.eventHandler); // for testing event sends
    };

    this.eventHandler = function eventHandler(event) {


    };

    this.init();
}

_l10iq.push(['providePlugin', 'socialtracker', L10iSocialTracker, {}]);