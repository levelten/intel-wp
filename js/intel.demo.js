var _ioq = _ioq || [];

function L10iDemo(_ioq, config) {
  var ioq = _ioq;
  var io = _ioq.io;

  this.init = function init() {
    ioq.log(ioq.name + ':demo.init()');//
  };

  this.clearVisitor = function clearVisitor() {
    var i, v, a, b;
    var cookies = document.cookie.split(';');
    for (i = 0; i < cookies.length; i++) {
      v = cookies[i].trim();

      if (v.substr(0, 4) == 'l10i') {

        a = v.split('=');
        // deletes standard domain=.[cookieDomain]
        ioq.deleteCookie(a[0]);
        // deletes domain=[cookieDomain]
        b = a[0] + '=;expires=Thu, 01 Jan 1970 00:00:01 GMT;path=/;domain=' + ioq.cookieDomain + ';';
        document.cookie = b;
      }
    }
  };

  this.init();
}

_ioq.push(['providePlugin', 'demo', L10iDemo, {}]);