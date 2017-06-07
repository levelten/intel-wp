var _ioq = _ioq || [];

function L10iLinkTracker(_ioq, config) {
    var ioq = _ioq;
    var io = _ioq.io;
    var $ = jQuery;


    this.init = function init() {
        var ths = this;
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
              $target.addClass('io-event-binded io-event-binded-linktracker');
            }
          });
        }
    };

    this.eventHandler = function eventHandler(event, $target, options) {
        var i, v;
        var $obj = $target || $(this);
        options = options || {};
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

        if (options.test) {
          return evtDef;
        }
        _ioq.push(['defEventHandler', evtDef, $obj, event]);

    };

    this.init();
}

_ioq.push(['providePlugin', 'linktracker', L10iLinkTracker, {}]);