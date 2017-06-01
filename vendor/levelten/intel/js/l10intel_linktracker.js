var _l10iq = _l10iq || [];

function L10iLinkTracker(_ioq, config) {
    var ioq = _ioq;
    var io = _ioq.io;
    var $ = jQuery;


    this.init = function init() {
        var ths = this;
        $('a').not('.linktracker-0').on('click', ths.eventHandler);
        //$('a').on('mouseover', {eventType: 'click'}, ths.eventHandler); // for testing event sends
    };

    this.eventHandler = function eventHandler(event) {
        var i, v;
        var $obj = $(this);
        var evtDef = {
          transport: 'beacon',
        };
        var eventType = event.type;
        if (_ioq.isObject(event.data) && event.data.eventType) {
          event.type = eventType = event.data.eventType;
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
            var downloadPattern = /\.(zip|exe|dmg|pdf|doc.*|xls.*|ppt.*|mp3|txt|rar|wma|mov|avi|wmv|flv|wav)$/i;

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

        evtDef.eventCategory = hrefTypeTitles[hrefType] + ' link ' + eventType;

        _ioq.push(['defEventHandler', evtDef, $obj, event]);

    };

    this.init();
}

_l10iq.push(['providePlugin', 'linktracker', L10iLinkTracker, {}]);