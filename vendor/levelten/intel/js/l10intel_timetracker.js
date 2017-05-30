var _l10iq = _l10iq || [];

function L10iTimeTracker(_ioq, config) {
    var ioq = _ioq;
    var io = _ioq.io;
    var $ = jQuery;

    this.init = function init() {
        var ths = this;
        var $window = $(window);
        //$window.on('beforeunload', function (event) { io('abr:doUnload', event); });
        $window.on('beforeunload', function (event) { ths.doUnload(event); }); // do this for efficency

        //$('a').on('hover', function (event) { ths.doUnload(event) });
    };

    this.setStickTimeout = function setStickTimeout() {

    };

    this.checkStick = function checkStick() {

    };

    this.doUnload = function doUnload() {
        ga('set', 'transport', 'beacon');
        // detect if
        var td0, m, s, si, inc;
        var maxTime = 600;
        var td = (window.performance) ? performance.now() / 1000 : (_ioq.getTime() - _ioq.pageviewSent);
        if (td > maxTime) {
            td = maxTime;
        }
        var tdr = Math.round(td);
        var ts = [];
        if (td < 120) {
            inc = 10;
        }
        else if (td < 300) {
            inc = 30;
        }
        else if (td < 600) {
            inc = 60;
        }
        if (inc) {
            td0 = tdr - inc;
            if (td0 < 0) {
                td0 = 0;
            }
            m = Math.floor(td0 / 60);
            s = td0 % 60;
            si = (inc * Math.floor(s / inc));
            ts.push((m < 10) ? '0' + m : m);
            ts.push(':');
            ts.push((si < 10) ? '0' + si : si);
            ts.push(' - ');
            m = Math.floor(tdr / 60);
            s = tdr % 60;
            si = (inc * Math.floor(s / inc));
            ts.push((m < 10) ? '0' + m : m);
            ts.push(':');
            ts.push((si < 10) ? '0' + si : si);
            ts = ts.join('');
        }
        else {
            ts = '10:00+';
        }

        var evtDef = {
            eventCategory: 'Page time',
            eventAction: ts,
            eventLabel: toString(tdr),
            eventValue: tdr,
            nonInteraction: true,
            metric8: tdr,
            metric9: 1
        };
        io('event', evtDef);
        // send timing event
        io('ga.send', 'timing', 'Page visibility', 'visible', Math.round(1000 * td));

    };



    this.init();
}

_l10iq.push(['providePlugin', 'timetracker', L10iTimeTracker, {}]);