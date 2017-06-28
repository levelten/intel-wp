var _l10iq = _l10iq || [];

function L10iStickTracker(_ioq, config) {
    var ioq = _ioq;
    var io = _ioq.io;
    var $;

    this.init = function init() {
        ioq.log('StickTracker::init()');
        $ = jQuery;
        var ths = this;
        ioq.addCallback('timeChange.20', ths.handlePageStick);
        //ioq.addCallback('sessionStick', ths.handleSessionStick);

        //$('a').on('hover', function (event) { ths.doUnload(event) });
    };

    this.handleSessionStick = function () {
        var v;
        //var ps = ioq.get('s.pageviews.0');

        var evtDef = {
            eventCategory: 'Session stick!',
            eventAction: '[[pageTitle]]',
            eventLabel: '[[pageUri]]',
            nonInteraction: false
        };
        if (v = ioq.settings.scorings.stick) {
            evtDef.eventValue = v;
        }
        io('event', evtDef);
    };

    this.handlePageStick = function () {
        var v;
        var evtDef = {
            eventCategory: 'Page stick!',
            eventAction: '[[pageTitle]]',
            eventLabel: '[[pageUri]]',
            eventValue: ioq.get('c.scorings.events.pageStick', 0),
            nonInteraction: false
        };
        io('event', evtDef);

        ioq.set('p.stick', 1);

        ioq.triggerCallbacks('pageStick');
    };

    this.init();
}

_l10iq.push(['providePlugin', 'sticktracker', L10iStickTracker, {}]);