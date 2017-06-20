var _ioq = _ioq || [];

function L10iLinkTracker(_ioq, config) {
    var ioq = _ioq;
    var io = _ioq.io;

    this.init = function init() {
        ioq.log('L10iLinkTracker.init()');
        var ths = this;
        var evtDef = {
            selector: 'a',
            selectorNot: '.linktracker-0',
            onEvent: 'click',
            onHandler: ths.eventHandler,
            transport: 'beacon'
        };
        io('event', evtDef);

        return;

        /*
        var $target = $('a.temp').not('.linktracker-0');
        var evtDef = {
            onHandler: function (ed, $t, e) {
                var e2 = {};
                return ths.eventHandler({});
            }
        };
        $('a').not('.linktracker-0').on('click', ths.eventHandler);
        //$('a').on('mouseover', {eventType: 'click'}, ths.eventHandler); // for testing event sends
        if (_ioq.location.params['io-admin']) {
          $('a').not('.linktracker-0').each(function (index, value) {
            var event = {
              type: 'click'
            };
            var $target = $(this);
            var options = {
              test: 1
            };
            var ret = ths.eventHandler(event, $target, options);
            if (!_ioq.isEmpty(ret)) {
              //$target.addClass('io-event-binded io-event-binded-linktracker');
            }
          });
        }
        */
    };

    this.eventHandler = function eventHandler(event) {
        var i, v;

        v = _ioq.getEventArgsFromEvent(event);
        var evtDef = v[0], $obj = v[1], options = v[3];

        var eventType = event.type;
        if (!eventType && evtDef.onEvent) {
            eventType = evtDef.onEvent;
        }

        var href = $obj.attr('href');

        if (!href) {
            return;
        }

        if ($obj.hasClass('prevent-linktracker')) {
          return;
        }

        var hrefTypeEnabled = {
            external: 1,
            internal: 0,
            download: 1,
            mailto: 1,
            tel: 1
        }

        var hrefTypeTitles = {
            external: 'External',
            internal: 'Internal',
            download: 'Download',
            mailto: 'Mailto',
            tel: 'Tel'
        };

        var hrefType = '';

        // check for hrefType specified via class track-link-[type id]
        var classes = $obj.attr('class');
        classes = classes ? classes.split(' ') : [];
        for (i = 0; i < classes.length; i++) {
            v = classes[i];
            if (v.substr(0, 11) == 'track-link-') {
                v = v.substr(11);
                if (v == 'mode-valued') {
                    evtDef.mode = 'valued';
                }
                else {
                    hrefType = v;
                }
            }
        }

        if (!hrefType) {
            var downloadPattern = /\.(zip|exe|dmg|pdf|doc.*|xls.*|ppt.*|mp3|txt|rar|wma|mov|avi|wmv|flv|wav|png|jpg|jpeg|gif)$/i;

            var loc = _ioq.parseUrl(href);

            loc.external = (loc.hostname != _ioq.location.hostname);

            if (href.substr(0, 7) == 'mailto:') {
                hrefType = 'mailto';
            }
            else if (href.substr(0, 4) == 'tel:') {
                hrefType = 'tel';
            }
            else if (loc.external) {
                hrefType = 'external';
            }
            else if (href.match(downloadPattern)) {
                hrefType = 'download';
            }
            else {
                hrefType = 'internal';
            }
        }

        if (!hrefTypeEnabled[hrefType] || !hrefTypeTitles[hrefType]) {
            return;
        }


        if (options.test) {
          evtDef.eventCategory = "[Type] link " + eventType;
        }
        else {
          evtDef.eventCategory = hrefTypeTitles[hrefType] + ' link ' + eventType;
        }

        // force re-construct
        delete(evtDef.const);

        return _ioq.defEventHandler(evtDef, $obj, event, options);
    };

    this.init();
    //_ioq.push(['addCallback', 'domReady', this.init, this]);
}

_ioq.push(['providePlugin', 'linktracker', L10iLinkTracker, {}]);