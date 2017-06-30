var _ioq = _ioq || [];

function L10iPageTracker(_ioq, config) {
    var ioq = _ioq;
    var io = _ioq.io;
    var $;
    var $win;

    this.init = function init() {
        ioq.log('PageTracker::init()');
        var ths = this;

        $ = jQuery;
        $win = $(window);

        // add callback for time intervale
        ioq.addCallback('timeInterval.30', ths.handlePageConsumedTime, this);

        // add beforeunload callback to trigger page time and page scroll events
        $win.on('beforeunload', function (event) { ths.handleUnload(event); });
    };

    this.handlePageConsumedTime = function () {
        console.log('PageTracker::handlePageConsumedTime()');
        var
          ths = this,
          scroll = ioq.get('p.scroll.contentBottomMaxPer', 0);
//console.log(scroll);
        // check if visitor has scrolled 90% to bottom of content
        if (scroll > 90) {
            this.sendPageConsumedEvent();
        }
        else {
            ioq.addCallback('scroll', this.handlePageConsumedScroll, this);
        }


    };

    this.handlePageConsumedScroll = function (scroll) {
        console.log('PageTracker::handlePageConsumedScroll()');
        //console.log(scroll.contentBottomMaxPer);
        if (scroll.contentBottomMaxPer > 90) {
            ioq.removeCallback('scroll', this.handlePageConsumedScroll, this);
            this.sendPageConsumedEvent();
        }
    };

    this.sendPageConsumedEvent = function() {
        console.log('PageTracker::sendPageConsumedEvent()');
        var evtDef = {
            eventCategory: 'Page consumed!',
            eventAction: '[[pageTitle]]',
            eventLabel: '[[pageUri]]',
            eventValue: ioq.get('c.scorings.events.pageConsumed', 0),
            nonInteraction: false
        };
        io('event', evtDef);
    };

    this.handleUnload = function handleUnload() {
        ga('set', 'transport', 'beacon');
        // detect if
        var td0, m, s, si, inc, scroll;
        var maxTime = 600;
        var td = ioq.getVisibleTime();
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
            if (td0 == 0) {
                si = td0 + inc;
            }
            else {
                si = (inc * Math.floor(s / inc));
            }

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
            eventLabel: '' + tdr,
            eventValue: tdr,
            nonInteraction: true,
            metric8: tdr,
            metric9: 1
        };
        io('event', evtDef);


        scroll = ioq.get('p.scroll', {});
        m = scroll.contentBottomMaxPer;
        // make sure m is between 0 & 100
        s = Math.round(Math.min(100, Math.max(0, m)));
        ts = '';
        if (s < 10) {
            ts = '  ';
        }
        else if (s < 100) {
            ts = ' ';
        }

        var evtDef = {
            eventCategory: 'Page scroll',
            eventAction: ts + (Math.round(s / 10) * 10) + '%',
            eventLabel: '' + s,
            eventValue: s,
            nonInteraction: true,
            metric10: scroll.pageMax,
            metric11: ioq.round(scroll.bottomMaxPer, 3),
            metric12: ioq.round(m, 3),
        };
        io('event', evtDef);

        // send timing event
        io('ga.send', 'timing', 'Page visibility', 'visible', Math.round(1000 * td));

    };
    _ioq.push(['addCallback', 'domReady', this.init, this]);
    //this.init();
}

_ioq.push(['providePlugin', 'pagetracker', L10iPageTracker, {}]);