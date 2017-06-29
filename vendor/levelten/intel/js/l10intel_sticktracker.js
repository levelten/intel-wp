var _l10iq = _l10iq || [];

function L10iStickTracker(_ioq, config) {
    var ioq = _ioq;
    var io = _ioq.io;
    var $;

    this.init = function init() {
        ioq.log('StickTracker::init()');
        $ = jQuery;
        var ths = this;
        ioq.addCallback('timeChange.15', ths.handlePageConsumedTime, this);
        //ioq.addCallback('sessionStick', ths.handleSessionStick);

        //$('a').on('hover', function (event) { ths.doUnload(event) });
    };

    this.handlePageConsumedTime = function () {
        console.log('StickTracker::handlePageConsumedTime()');
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
        console.log('StickTracker::handlePageConsumedScroll()');
        //console.log(scroll.contentBottomMaxPer);
        if (scroll.contentBottomMaxPer > 90) {
            ioq.removeCallback('scroll', this.handlePageConsumedScroll, this);
            this.sendPageConsumedEvent();
        }
    };

    this.sendPageConsumedEvent = function() {
        console.log('StickTracker::sendPageConsumedEvent()');
        var evtDef = {
            eventCategory: 'Page consumed!',
            eventAction: '[[pageTitle]]',
            eventLabel: '[[pageUri]]',
            eventValue: ioq.get('c.scorings.events.pageConsumed', 0),
            nonInteraction: false
        };
        io('event', evtDef);
    };

    this.init();
}

_l10iq.push(['providePlugin', 'sticktracker', L10iStickTracker, {}]);