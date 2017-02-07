var _l10iq = _l10iq || [];

// load YouTube javascript API
var tag = document.createElement('script');
tag.src = "https://www.youtube.com/iframe_api";
var firstScriptTag = document.getElementsByTagName('script')[0];
firstScriptTag.parentNode.insertBefore(tag, firstScriptTag);

function L10iYouTube(_ioq) {
    var ioq = _ioq;
    var io = _ioq.io;
    this.playerState = {};
    this.players = {};
    this.domReady = false;
    this.apiReady = false;
    this.ready = false;

    this.init = function init() {
        this.domReady = true;
        if (!this.ready && this.apiReady) {
            this.trackYouTube();
            this.ready = true;
        }
    };

    this.apiInit = function () {
        this.apiReady = true;
        if (!this.ready && this.domReady) {
            this.trackYouTube();
            this.ready = true;
        }
    };

    this.trackPlayer = function (player, videoId) {
        player.addEventListener('onReady', function (event) { io('youtube:onPlayerReady', event); });
        player.addEventListener('onStateChange', function (event) { io('youtube:onPlayerStateChange', event); });
        this.players[videoId] = player;
        this.playerState[videoId] = {
            state: -1,
            paused: true
        };

    };

    this.trackYouTube = function () {
        jQuery('iframe').each(function() {
            var video = jQuery(this);
            if(video.attr('src') !== undefined){
                var vidSrc = video.attr('src');
                var regex = /h?t?t?p?s?\:?\/\/www\.youtube\.com\/embed\/([\w-]{11})(?:\?.*)?/;
                var matches = vidSrc.match(regex);
                if(matches && matches.length > 1){
                    // add a anchor link to support report links
                    video.before('<a name="video-' + matches[1] + '"></a>');
                    video.attr('id', matches[1]);
                    var width = video.width();
                    var height = video.height();
                    jQuery('iframe#' + matches[1]).replaceWith('<div id="' + matches[1] + '"></div>');
                    var player = new YT.Player(matches[1], {
                        videoId: matches[1],
                        height: height,
                        width: width
                        /*
                        events: {
                            'onReady': this.onPlayerReady,
                            'onStateChange': this.onPlayerStateChange
                        }
                        */
                    });
                    io('youtube:trackPlayer', player, matches[1]);
                    //ths.trackPlayer(player, matches[1]);
                }
            }
        });
    };

    this.onPlayerReady = function (event) {

    };

    this.onPlayerStateChange = function (event) {
        // check if YouTube API event data struc is correct
        if (event.target == undefined || event.target.getVideoData == undefined) {
            return;
        }
        var videoData = event.target.getVideoData();

        var id = videoData.video_id;
        var title = (videoData.author) ? videoData.author : '(not set)';
        title += ': ' + ((videoData.title) ? videoData.title : '(not set)');

        var player = this.players[id];
        var ga_event = {
            'eventCategory': "Video event",
            'eventAction': "YouTube: " + title,
            'eventLabel': "::youtube:" + id,
            'eventValue': 0,
            'nonInteraction': false
        };
        ga_event.oa = {
            rs: 'youtube',
            rc: 'video',
            rk: id,
            domi: id
        };
        var positionPer = Math.round(100 * player.getCurrentTime() / player.getDuration());
        if (event.data == YT.PlayerState.PLAYING){
            ga_event.eventCategory = 'Video play';
            var value = io('get', 'config.scorings.youtube_video_play', 0);
            if (value > 0) {
                ga_event.eventCategory += '!';
            }
            ga_event.eid = 'videoPlay';
            //_l10iq.push(['_trackIntelEvent', jQuery(this), ga_event, '']);
            io('event', ga_event);
            this.playerState[id].paused = false;
        }
        else if (event.data == YT.PlayerState.ENDED  && !this.playerState[id].paused){
            ga_event.eventCategory = 'Video watched';
            ga_event.eventValue = 100;
            ga_event.eid = 'videoWatched';
            io('event', ga_event);
            //_l10iq.push(['_trackIntelEvent', jQuery(this), ga_event, '']);
        }
        else if (event.data == YT.PlayerState.PAUSED && !this.playerState[id].paused){
            ga_event.eventCategory = 'Video stop';
            ga_event.eid = 'videoStop';
            io('event', ga_event);
            //_l10iq.push(['_trackIntelEvent', jQuery(this), ga_event, '']);

            // copy object for Video watched
            var ga_event2 = jQuery.extend({}, ga_event);
            ga_event2.eventCategory = 'Video watched';
            ga_event2.eventValue = positionPer;
            ga_event2.eid = 'videoWatched';
            io('event', ga_event2);
            //_l10iq.push(['_trackIntelEvent', jQuery(this), ga_event2, '']);
            this.playerState[id].paused = true;
        }
    };
    _l10iq.push(['addCallback', 'domReady', this.init, this]);
}

_l10iq.push(['providePlugin', 'youtube', L10iYouTube, {}]);

// function called by YouTube API when ready
function onYouTubeIframeAPIReady() {
    _l10iq.push(['youtube:apiInit']);
    //this.apiInit();
}
