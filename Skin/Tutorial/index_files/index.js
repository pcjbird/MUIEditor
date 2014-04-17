(function(d,i,j){
	var k,h,l,m;
	l={
		paneClass:"pane",sliderClass:"slider",contentClass:"content",iOSNativeScrolling:!1,preventPageScrolling:!1,disableResize:!1,alwaysVisible:!1,flashDelay:1500,sliderMinHeight:20,sliderMaxHeight:null
	};
	k="Microsoft Internet Explorer"===i.navigator.appName&&/msie 7./i.test(i.navigator.appVersion)&&i.ActiveXObject;
	h=null;
	m=function(){
		var a,b;
		a=j.createElement("div");
		b=a.style;
		b.position="absolute";
		b.width="100px";
		b.height="100px";
		b.overflow="scroll";
		b.top="-9999px";
		j.body.appendChild(a);
		b=a.offsetWidth-a.clientWidth;
		j.body.removeChild(a);
		return b
	};
	var c=function(a,b){
		this.el=a;
		this.options=b;
		h||(h=m());
		this.$el=d(this.el);
		this.doc=d(j);
		this.win=d(i);
		this.generate();
		this.createEvents();
		this.addEvents();
		this.reset()
	};
	c.prototype.preventScrolling=function(a,b){
		this.isActive&&("DOMMouseScroll"===a.type?("down"===b&&0<a.originalEvent.detail||"up"===b&&0>a.originalEvent.detail)&&a.preventDefault():"mousewheel"===a.type&&a.originalEvent&&a.originalEvent.wheelDelta&&("down"===b&&0>a.originalEvent.wheelDelta||
		"up"===b&&0<a.originalEvent.wheelDelta)&&a.preventDefault())
	};
	c.prototype.updateScrollValues=function(){
		var a;
		a=this.content;
		this.maxScrollTop=a.scrollHeight-a.clientHeight;
		this.contentScrollTop=a.scrollTop;
		this.maxSliderTop=this.paneHeight-this.sliderHeight;
		this.sliderTop=this.contentScrollTop*this.maxSliderTop/this.maxScrollTop
	};
	c.prototype.createEvents=function(){
		var a=this;
		this.events={
			down:function(b){
				a.isBeingDragged=!0;
				a.offsetY=b.pageY-a.slider.offset().top;
				a.pane.addClass("active");
				a.doc.bind("mousemove",
				a.events.drag).bind("mouseup",a.events.up);
				return!1
			}
			,drag:function(b){
				a.sliderY=b.pageY-a.$el.offset().top-a.offsetY;
				a.scroll();
				a.updateScrollValues();
				a.contentScrollTop>=a.maxScrollTop?a.$el.trigger("scrollend"):0===a.contentScrollTop&&a.$el.trigger("scrolltop");
				return!1
			}
			,up:function(){
				a.isBeingDragged=!1;
				a.pane.removeClass("active");
				a.doc.unbind("mousemove",a.events.drag).unbind("mouseup",a.events.up);
				return!1
			}
			,resize:function(){
				a.reset()
			}
			,panedown:function(b){
				a.sliderY=(b.offsetY||b.originalEvent.layerY)-
				0.5*a.sliderHeight;
				a.scroll();
				a.events.down(b);
				return!1
			}
			,scroll:function(b){
				a.isBeingDragged||(a.updateScrollValues(),a.sliderY=a.sliderTop,a.slider.css({
					top:a.sliderTop
				}),null!=b&&(a.contentScrollTop>=a.maxScrollTop?(a.options.preventPageScrolling&&a.preventScrolling(b,"down"),a.$el.trigger("scrollend")):0===a.contentScrollTop&&(a.options.preventPageScrolling&&a.preventScrolling(b,"up"),a.$el.trigger("scrolltop"))))
			}
			,wheel:function(b){
				if(null!=b)return a.sliderY+=-b.wheelDeltaY||-b.delta,a.scroll(),
				!1
			}
		}
	};
	c.prototype.addEvents=function(){
		var a;
		this.removeEvents();
		a=this.events;
		this.options.disableResize||this.win.bind("resize",a.resize);
		this.slider.bind("mousedown",a.down);
		this.pane.bind("mousedown",a.panedown).bind("mousewheel DOMMouseScroll",a.wheel);
		this.$content.bind("scroll mousewheel DOMMouseScroll touchmove",a.scroll)
	};
	c.prototype.removeEvents=function(){
		var a;
		a=this.events;
		this.win.unbind("resize",a.resize);
		this.slider.unbind();
		this.pane.unbind();
		this.$content.unbind("scroll mousewheel DOMMouseScroll touchmove",
		a.scroll)
	};
	c.prototype.generate=function(){
		var a,b,c,g,e;
		c=this.options;
		g=c.paneClass;
		e=c.sliderClass;
		a=c.contentClass;
		!this.$el.find(""+g).length&&!this.$el.find(""+e).length&&this.$el.append('<div class="'+g+'"><div class="'+e+'" /></div>');
		this.$content=this.$el.children("."+a);
		this.$content.attr("tabindex",0);
		this.content=this.$content[0];
		this.slider=this.$el.find("."+e);
		this.pane=this.$el.find("."+g);
		h&&(b={
			right:-h
		}
		,this.$el.addClass("has-scrollbar"));
		c.iOSNativeScrolling&&(b||(b={}),b.WebkitOverflowScrolling=
		"touch");
		null!=b&&this.$content.css(b);
		return this
	};
	c.prototype.restore=function(){
		this.stopped=!1;
		this.pane.show();
		this.addEvents()
	};
	c.prototype.reset=function(){
		var a,b,c,g,e,d,f;
		this.$el.find("."+this.options.paneClass).length||this.generate().stop();
		this.stopped&&this.restore();
		a=this.content;
		c=a.style;
		g=c.overflowY;
		k&&this.$content.css({
			height:this.$content.height()
		});
		b=a.scrollHeight+h;
		d=this.pane.outerHeight();
		f=parseInt(this.pane.css("top"),10);
		e=parseInt(this.pane.css("bottom"),10);
		e=d+f+
		e;
		f=Math.round(e/b*e);
		f<this.options.sliderMinHeight?f=this.options.sliderMinHeight:null!=this.options.sliderMaxHeight&&f>this.options.sliderMaxHeight&&(f=this.options.sliderMaxHeight);
		"scroll"===g&&"scroll"!==c.overflowX&&(f+=h);
		this.maxSliderTop=e-f;
		this.contentHeight=b;
		this.paneHeight=d;
		this.paneOuterHeight=e;
		this.sliderHeight=f;
		this.slider.height(f);
		this.events.scroll();
		this.pane.show();
		this.isActive=!0;
		a.scrollHeight===a.clientHeight||this.pane.outerHeight(!0)>=a.scrollHeight&&"scroll"!==g?(this.pane.hide(),
		this.isActive=!1):this.el.clientHeight===a.scrollHeight&&"scroll"===g?this.slider.hide():this.slider.show();
		this.options.alwaysVisible?this.pane.css({
			opacity:1,visibility:"visible"
		}):this.pane.css({
			opacity:"",visibility:""
		});
		return this
	};
	c.prototype.scroll=function(){
		if(this.isActive)return this.sliderY=Math.max(0,this.sliderY),this.sliderY=Math.min(this.maxSliderTop,this.sliderY),this.$content.scrollTop(-1*((this.paneHeight-this.contentHeight+h)*this.sliderY/this.maxSliderTop)),this.slider.css({
			top:this.sliderY
		}),
		this
	};
	c.prototype.scrollBottom=function(a){
		if(this.isActive)return this.reset(),this.$content.scrollTop(this.contentHeight-this.$content.height()-a).trigger("mousewheel"),this
	};
	c.prototype.scrollTop=function(a){
		if(this.isActive)return this.reset(),this.$content.scrollTop(+a).trigger("mousewheel"),this
	};
	c.prototype.scrollTo=function(a){
		if(this.isActive)return this.reset(),a=d(a).offset().top,a>this.maxSliderTop&&(a/=this.contentHeight,this.sliderY=a*=this.maxSliderTop,this.scroll()),this
	};
	c.prototype.stop=
	function(){
		this.stopped=!0;
		this.removeEvents();
		this.pane.hide();
		return this
	};
	c.prototype.flash=function(){
		var a=this;
		if(this.isActive)return this.reset(),this.pane.addClass("flashed"),setTimeout(function(){
			a.pane.removeClass("flashed")
		}
		,this.options.flashDelay),this
	};
	d.fn.nanoScroller=function(a){
		return this.each(function(){
			var b;
			if(!(b=this.nanoscroller))b=d.extend({},l,a),this.nanoscroller=b=new c(this,b);
			if(a&&"object"===typeof a){
				d.extend(b.options,a);
				if(a.scrollBottom)return b.scrollBottom(a.scrollBottom);
				if(a.scrollTop)return b.scrollTop(a.scrollTop);
				if(a.scrollTo)return b.scrollTo(a.scrollTo);
				if("bottom"===a.scroll)return b.scrollBottom(0);
				if("top"===a.scroll)return b.scrollTop(0);
				if(a.scroll&&a.scroll instanceof d)return b.scrollTo(a.scroll);
				if(a.stop)return b.stop();
				if(a.flash)return b.flash()
			}
			return b.reset()
		})
	}
})(jQuery,window,document);

/* v1.4 */
/* https://github.com/farinspace/jquery.imgpreload */
if("undefined"!=typeof jQuery){(function(a){a.imgpreload=function(b,c){c=a.extend({},a.fn.imgpreload.defaults,c instanceof Function?{all:c}:c);if("string"==typeof b){b=new Array(b)}var d=new Array;a.each(b,function(e,f){var g=new Image;var h=f;var i=g;if("string"!=typeof f){h=a(f).attr("src");i=f}a(g).bind("load error",function(e){d.push(i);a.data(i,"loaded","error"==e.type?false:true);if(c.each instanceof Function){c.each.call(i)}if(d.length>=b.length&&c.all instanceof Function){c.all.call(d)}a(this).unbind("load error")});g.src=h})};a.fn.imgpreload=function(b){a.imgpreload(this,b);return this};a.fn.imgpreload.defaults={each:null,all:null}})(jQuery)}

(function(a){function d(a){var b=a.split(/\s/),c={X:b[0],Y:b[1]};return c}var b=a('<div style="background-position: 3px 5px">');a.support.backgroundPosition=b.css("backgroundPosition")==="3px 5px"?true:false;a.support.backgroundPositionXY=b.css("backgroundPositionX")==="3px"?true:false;b=null;var c=["X","Y"];if(!a.support.backgroundPosition&&a.support.backgroundPositionXY){a.cssHooks.backgroundPosition={get:function(b,d,e){return a.map(c,function(c,d){return a.css(b,"backgroundPosition"+c)}).join(" ")},set:function(b,e){a.each(c,function(a,c){var f=d(e);b.style["backgroundPosition"+c]=f[c]})}}}if(a.support.backgroundPosition&&!a.support.backgroundPositionXY){a.each(c,function(b,c){a.cssHooks["backgroundPosition"+c]={get:function(b,e,f){var g=d(a.css(b,"backgroundPosition"));return g[c]},set:function(b,e){var f=d(a.css(b,"backgroundPosition")),g=c==="X";b.style.backgroundPosition=(g?e:f["X"])+" "+(g?f["Y"]:e)}};a.fx.step["backgroundPosition"+c]=function(b){a.cssHooks["backgroundPosition"+c].set(b.elem,b.now+b.unit)}})}})(jQuery)

function removeA(arr){
    var what, a= arguments, L= a.length, ax;
    while(L> 1 && arr.length){
        what= a[--L];
        while((ax= arr.indexOf(what))!= -1){
            arr.splice(ax, 1);
        }
    }
    return arr;
}

function checkMailValidity(value) {
	// contributed by Scott Gonzalez: http://projects.scottsplayground.com/email_address_validation/
	return /^((([a-z]|\d|[!#\$%&'\*\+\-\/=\?\^_`{\|}~]|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])+(\.([a-z]|\d|[!#\$%&'\*\+\-\/=\?\^_`{\|}~]|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])+)*)|((\x22)((((\x20|\x09)*(\x0d\x0a))?(\x20|\x09)+)?(([\x01-\x08\x0b\x0c\x0e-\x1f\x7f]|\x21|[\x23-\x5b]|[\x5d-\x7e]|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])|(\\([\x01-\x09\x0b\x0c\x0d-\x7f]|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF]))))*(((\x20|\x09)*(\x0d\x0a))?(\x20|\x09)+)?(\x22)))@((([a-z]|\d|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])|(([a-z]|\d|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])([a-z]|\d|-|\.|_|~|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])*([a-z]|\d|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])))\.)+(([a-z]|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])|(([a-z]|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])([a-z]|\d|-|\.|_|~|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])*([a-z]|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])))$/i.test(value);
}

$(window).load(function() {
	$([
		'images/bg.jpg',
		'images/bg/1.png',
		'images/bg/2.png',
		'images/bg/3.png',
		'images/bg/4.png',
		'images/bg/5.png',
		'images/bg/6.png',
		'images/bg/7.png'
	]).imgpreload(function() {
		$("#overlay").fadeOut("slow");
	});


	$("#lightbox").css("left", $(window).outerWidth()).removeClass("loading");
	$("#menu, #rightNav").css("padding-top", ($(window).height()-$("#menu").height())/2);

});



$(document).ready(function() {

	$("#menu ul li a").click(function(e) {
		$("#menu .hover").removeClass("hover");
	});

	$("a[href^='http:']:not([href*='" + window.location.host + "'][target='_blank'])").live('click', function(){
		  $(this).attr('target','_blank');
	});

	$(".downloadBox a").click(function(e) {
		$(this).attr('target','_blank');
	});

	$(window).resize(function() {
		$("#menu, #rightNav").css("padding-top", ($(window).height()-$("#menu").height())/2);
		$("#lightbox").css("top", ($(window).height()-970)/2);
	});


	$("#closeIEWarning").click(function(e) {
		e.preventDefault();

		$("#warningIE").fadeOut("fast", function() {
			$(this).remove();
		});
	});


		$.fn.scrollPath("getPath")
			.moveTo(1000, 0, {name: "home"})
			.lineTo(2400, -1400, {name: "project"})
			.arc(2400, 0, 1400, Math.PI*1.5, Math.PI, true)
			.lineTo(1000, 2250, {name: "about"})
			.lineTo(1000, 3250)
			.arc(1550, 3250, 550, -Math.PI, Math.PI/2, true)
			.lineTo(2500, 3800, {name: "layout", rotate: -Math.PI/2})
			.arc(3550, 3800, 900, -Math.PI, Math.PI/2, true)
			.lineTo(4500, 4700, {name: "resource"})
			.arc(4500, 3800, 900, Math.PI/2, 0, true)
			.lineTo(5400, 1350, {name: "widget", rotate: Math.PI/2})
			.arc(5400, -50, 1350, Math.PI/2, Math.PI*1.5, true, {name: "galerie", rotate: 0})
			.lineTo(2800, 1200, {name: "shortcut"})
			.arc(2800, 0, 1200, Math.PI/2,  Math.PI*1.5, true)
			
			;


	// We're done with the path, let's initate the plugin on our wrapper element
	$(".wrapper").scrollPath({drawPath: false, wrapAround: true});

	$("nav > ul > li > a").each(function() {
		var target = $(this).attr("href").replace("#", "");

		$(this).click(function(e) {
			if (target != "home") {
				window.location.hash = '#'+target;
			} else {
				window.location.hash = '#';
			}

			if (!$("#lightbox").hasClass("active")) {
				e.preventDefault();

				$("nav .active").removeClass("active");
				$(this).parent().addClass("active");
				$.fn.scrollPath("scrollTo", target, 2000, function(t, b, c, d) {
	                return -c / 2 * (Math.cos(Math.PI * t / d) - 1) + b;
	        	});

        	} else if (e.hasOwnProperty('originalEvent')) {
				var element = $("#lightbox");
				element.addClass("rotateOut").animate({left: $(window).outerWidth()}, 1400, 'linear', function() {
					element.removeAttr("class");

				});

				$("nav .active").removeClass("active");
				$(this).parent().addClass("active");
				$.fn.scrollPath("scrollTo", target, 2000, function(t, b, c, d) {
	                return -c / 2 * (Math.cos(Math.PI * t / d) - 1) + b;
	        	});

				$(".wrap").fadeIn(1000);
        	}
		});

	});

	$(".lightbox").live("click", function(e) {
		e.preventDefault();

		var lightbox = $("#lightbox");
		var element = $(this);

		$.get(element.attr("href"), function(returnData) {
			window.location.hash = '#!'+element.attr("href");
			$("#lightboxContent").html(returnData);
			$("#lightboxContent .nano").nanoScroller().scrollTop(0);
		});

		$("#lightbox").addClass("rotate active").animate({left: ($(window).outerWidth()-970)/2}, 1400, 'linear', function() {
		});

		$(".wrap").fadeOut(1000);

	});

	$("#lightboxClose").click(function(e) {
		e.preventDefault();

		if ($("#menu .active").attr("id") != "home") {
			window.location.hash = $("#menu .active > a").attr("href");
		} else {
			window.location.hash = null;
		}


		var element = $("#lightbox");
		element.addClass("rotateOut").animate({left: $(window).outerWidth()}, 1400, 'linear', function() {
			element.removeAttr("class");
		});

		$(".wrap").fadeIn(1000);
	});


/*	$("#scrollInfo").click(function(e) {
		e.preventDefault();

		$("nav .active").next().find("> a").click();

	});
*/

	(function($) {
		$("#menu ul > li").hover(function() {
			$(this).addClass("hover");
		}, function() {
			$(this).removeClass("hover");
		});

	})(jQuery);



			$("a#example1").fancybox({
				'padding':0,
				'overlayColor':'#000',
				'overlayOpacity':0.8,
				'hideOnOverlayClick':false,
				'cyclic':true,
				'transitionIn'	: 'elastic',
				'transitionOut'	: 'elastic',
				'titlePosition' 	: 'over'
			});


			$("a[rel=example_group]").fancybox({
				'padding':0,
				'overlayColor':'#000',
				'overlayOpacity':0.8,
				'hideOnOverlayClick':false,
				'cyclic':true,
				'transitionIn'	: 'elastic',
				'transitionOut'	: 'elastic',
				'titlePosition' 	: 'over'
			});

			$("a.various1").fancybox({
				'padding':0,
				'overlayColor':'#000',
				'overlayOpacity':0.8,
				'hideOnOverlayClick':false,
				'cyclic':true,
				'transitionIn'	: 'elastic',
				'transitionOut'	: 'elastic',
				'titlePosition' 	: 'over'
			});

});