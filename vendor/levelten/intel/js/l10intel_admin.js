var _ioq = _ioq || [];

function L10iAdmin(_ioq, config) {
    var ioq = _ioq;
    var io = _ioq.io;
    var $ = jQuery;


    this.init = function init() {
        var ths = this;
        io('addCallback', 'bindEvent', ths.bindEventCallback);
    };

    this.bindEventCallback = function bindEventCallback(evtDef, $target) {
        //$target.addClass('io-admin io-event-binded');
        $target.each(function(index, value) {
            var evt = {};
            var $value = $(value);
            var options = {
                test: 1
            };
            var ret = 0;
            // check if default eventHandler is overridden
            if (evtDef.onHandler) {
                // spoof event for custom handler
                evt.data = {
                    io: {
                        eventDef: evtDef,
                        options: options
                    }
                };
                evt.target = value;
                ret = evtDef.onHandler(evt);
            }
            else {
                ret = _ioq.defEventHandler(evtDef, $value, evt, options);
            }

            if (_ioq.isObject(ret) && _ioq.isObject(ret.gaEvent) && ret.gaEvent.eventCategory) {
                $value.addClass('io-admin io-event-binded');
            }
        });
    };

    this.init();
}

_ioq.push(['providePlugin', 'admin', L10iAdmin, {}]);