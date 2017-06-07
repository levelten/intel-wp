var _l10iq = _l10iq || [];

function L10iSocialTracker(_ioq, config) {
    var ioq = _ioq;
    var io = _ioq.io;
    var $ = jQuery;
    var socialDefs = {};
    socialDefs.facebook = {
        title: 'Facebook',
        hostname: ['facebook.com']
    };


    this.init = function init() {
        var ths = this;
        //$('a').not('.linktracker-0').on('click', ths.eventHandler);
        //$('a').on('mouseover', {eventType: 'click'}, ths.eventHandler); // for testing event sends
    };

    this.eventHandlerAlter = function eventHandlerAlter (evtDef, $target, event) {
        var a, href, parsedHref;
        if (!evtDef.socialNetwork) {
            a = $target.attr('data-io-social-network');
            if (a) {
                evtDef.socialNetwork = a;
            }
        }
        if (!evtDef.eventAction && evtDef.socialNetwork) {
            evtDef.eventAction = evtDef.socialNetwork;
        }
    };

    this.eventHandler = function eventHandler(evtDef, $target, event, gaEvt) {
        if (evtDef.socialNetwork && evtDef.socialAction) {
            var socialDef = {
                socialNetwork: evtDef.socialNetwork,
                socialAction: evtDef.socialAction,
                socialTarget: _ioq.location.href,
                hitType: 'social'
            };
            io('ga.send', socialDef);
        }
    };

    this.init();
}

_l10iq.push(['providePlugin', 'socialtracker', L10iSocialTracker, {}]);