var _ioq = _ioq || [];

function L10iLinkTracker(_ioq, config) {
    var ioq = _ioq;
    var io = _ioq.io;

    this.hrefTypeDefs = {};
    this.hrefTypeDefs.external = {
        title: 'External link'
    };
    this.hrefTypeDefs.download = {
        title: 'Download link'
    };
    this.hrefTypeDefs.mailto = {
        title: 'Mailto link'
    };
    this.hrefTypeDefs.tel = {
        title: 'Tel link'
    };


    this.init = function init() {
        ioq.log(ioq.name + ':linktracker.init()');
        var ths = this;
        var evtDef = {
            key: 'linktracker_link_click',
            selector: 'a',
            selectorNot: '.linktracker-0',
            onEvent: 'click',
            //onEvent: 'contextmenu',
            onHandler: function (event) { ths.handleLinkEvent(event) },
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

    this.addHrefType = function addHrefType(name, obj) {
      this.hrefTypeDefs[name] = obj;
    };

    this.removeHrefType = function removeHrefType(name) {
        if (this.hrefTypeDefs[name]) {
            delete(this.hrefTypeDefs[name]);
        }
    };

    this.handleLinkEvent = function handleLinkEvent(event) {
        var i, v;
        var f = {
            event: event,
            hrefTypeDefs: this.hrefTypeDefs
        };

        v = _ioq.getEventArgsFromEvent(event);
        f.evtDef = v[0], f.$obj = v[1], f.options = v[3];

        f.eventType = event.type;
        if (!f.eventType && f.evtDef.onEvent) {
            f.eventType = f.evtDef.onEvent;
        }

        f.href = f.$obj.attr('href');

        if (!f.href) {
            return;
        }

        if (f.$obj.hasClass('prevent-linktracker')) {
          return;
        }

        f.hrefType = '';

        // check for hrefType specified via class track-link-[type id]
        var classes = f.$obj.attr('class');
        classes = classes ? classes.split(' ') : [];
        for (i = 0; i < classes.length; i++) {
            v = classes[i];
            if (v.substr(0, 11) == 'track-link-') {
                v = v.substr(11);
                if (v == 'mode-valued') {
                    f.evtDef.mode = 'valued';
                }
                else {
                    f.hrefType = v;
                }
            }
        }

        if (!f.hrefType) {
            var downloadPattern = /\.(zip|exe|dmg|pdf|doc.*|xls.*|ppt.*|mp3|txt|rar|wma|mov|avi|wmv|flv|wav|png|jpg|jpeg|gif)$/i;

            f.hrefObj = _ioq.parseUrl(f.href);

            f.hrefObj.external = (f.hrefObj.hostname != ioq.location.hostname);

            if (f.href.substr(0, 7) == 'mailto:') {
                f.hrefType = 'mailto';
            }
            else if (f.href.substr(0, 4) == 'tel:') {
                f.hrefType = 'tel';
            }
            else if (f.hrefObj.external) {
                f.hrefType = 'external';
            }
            else if (f.href.match(downloadPattern)) {
                f.hrefType = 'download';
            }
            else {
                f.hrefType = 'internal';
            }
        }

        // trigger callbacks
        ioq.triggerCallbacks('handleLinkEventAlter', f);
        if (f.hrefTypeDefs[f.hrefType]) {
            f.evtDef.eventCategory = f.hrefTypeDefs[f.hrefType].title;
            if (f.eventType) {
                f.evtDef.eventCategory += ' ' + f.eventType
            }
            // force re-construct
            delete(f.evtDef.const);
        }

        if (f.evtDef.eventCategory) {
            return _ioq.defEventHandler(f.evtDef, f.$obj, f.event, f.options);
        }
    };

    this.init();
    //_ioq.push(['addCallback', 'domReady', this.init, this]);
}

_ioq.push(['providePlugin', 'linktracker', L10iLinkTracker, {}]);