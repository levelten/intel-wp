var _ioq = _ioq || [];

function L10iAdmin(_ioq, config) {
    var ioq = _ioq;
    var io = _ioq.io;
    var eventTests = [];
    var eventBindReported = [];

    this.init = function init() {
        ioq.log('admin:init()');//
        if (!ioq.settings.admin) {
            return;
        }
        var ths = this;
        io('addCallback', 'bindEvent', ths.bindEventCallback);
        io('addCallback', 'triggerEventAlter', ths.triggerEventAlterCallback);
        io('addCallback', 'triggerEvent', ths.triggerEventCallback);
    };

    this.bindEventCallback = function bindEventCallback(evtDef, $target) {
        var evt = {};
        var options = {
            test: 1,
            admin: {
                bindTarget: []
            }
        };

        $target.each(function(index, value) {
            var evt = {};
            var $value = jQuery(value);


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

            //if (_ioq.isObject(ret) && _ioq.isObject(ret.gaEvent) && ret.gaEvent.eventCategory) {
            //    bindTargets.push(value);
            //}
        });

        var logObj = {
            eventDef: evtDef,
            bindTarget: options.admin.bindTarget
        };
    };

    this.setBindTarget = function ($target) {
        $target.css('outline', '3px solid #33FF33');
        if (ioq.isDebug()) {
            $target.addClass('io-admin-bind-target');
        }
    };

    this.triggerEventAlterCallback = function triggerEventAlterCallback(trigEvt, $target, event, options, evtDef) {
        if (!options.test)  {
            options.test = 2;
        }
    };

    this.triggerEventCallback = function triggerEventCallback(trigEvt, $target, event, options, evtDef, gaEvt) {
        if (!options.test)  {
            return;
        }

        var target;
        var prevent = 0;
        if (ioq.location.params['io-admin-prevent'] && ioq.isFunction(event.preventDefault)) {
            event.preventDefault();
        }

        var logObj = {
            eventDef: evtDef
        };

        // binding stage
        if (options.test == 1) {
            if (ioq.isObject(gaEvt) && gaEvt.eventCategory) {
                if (ioq.is$Object($target)) {
                    options.admin.bindTarget.push($target.get(0));
                }
            }
            //$target.css('outline', '4px solid #33FF33');
            io('admin:setBindTarget', $target);
            return;
        }
        // trigger stage
        if (options.test == 2) {
            ds = '';
            for (i in gaEvt) {
                if (ds) {
                    ds += ',';
                }
                ds += '\n  ' + i + ': ';
                if (ioq.isString(gaEvt[i])) {
                    ds += "'" + gaEvt[i] + "'";
                }
                else {
                    ds += gaEvt[i];
                }
            }
            alert("ga.send.event: {" + ds + '\n}');
            logObj.target$ = $target;
            logObj.event = event;
            logObj.trigEvt = trigEvt;
            logObj.gaEvt = gaEvt;
            logObj.options = options;
        }

        //alert("ga.send.event: ");
    };

    this.init();
}

_ioq.push(['providePlugin', 'admin', L10iAdmin, {}]);