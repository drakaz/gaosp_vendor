
  

  


<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
  <head>
    <meta http-equiv="content-type" content="text/html;charset=UTF-8" />
    <meta http-equiv="X-UA-Compatible" content="chrome=1">
        <title>prebuilt/common/xbin/openvpn-up.sh at froyo from CyanogenMod's android_vendor_cyanogen - GitHub</title>
    <link rel="search" type="application/opensearchdescription+xml" href="/opensearch.xml" title="GitHub" />
    <link rel="fluid-icon" href="http://github.com/fluidicon.png" title="GitHub" />

    <link href="http://assets2.github.com/stylesheets/bundle_common.css?935bbdff6dd3f65d860d46e4de77e4ea817dc684" media="screen" rel="stylesheet" type="text/css" />
<link href="http://assets1.github.com/stylesheets/bundle_github.css?935bbdff6dd3f65d860d46e4de77e4ea817dc684" media="screen" rel="stylesheet" type="text/css" />

    <script type="text/javascript" charset="utf-8">
      var GitHub = {}
      var github_user = null
      
    </script>
    <script src="http://assets3.github.com/javascripts/jquery/jquery-1.4.2.min.js?935bbdff6dd3f65d860d46e4de77e4ea817dc684" type="text/javascript"></script>
    <script src="http://assets0.github.com/javascripts/bundle_common.js?935bbdff6dd3f65d860d46e4de77e4ea817dc684" type="text/javascript"></script>
<script src="http://assets0.github.com/javascripts/bundle_github.js?935bbdff6dd3f65d860d46e4de77e4ea817dc684" type="text/javascript"></script>

        <script type="text/javascript" charset="utf-8">
      GitHub.spy({
        repo: "CyanogenMod/android_vendor_cyanogen"
      })
    </script>

    
  
    
  

  <link href="http://github.com/CyanogenMod/android_vendor_cyanogen/commits/froyo.atom" rel="alternate" title="Recent Commits to android_vendor_cyanogen:froyo" type="application/atom+xml" />

        <meta name="description" content="CyanogenMod Android vendor tree" />
    <script type="text/javascript">
      GitHub.nameWithOwner = GitHub.nameWithOwner || "CyanogenMod/android_vendor_cyanogen";
      GitHub.currentRef = 'froyo';
    </script>
  

            <script type="text/javascript">
      var _gaq = _gaq || [];
      _gaq.push(['_setAccount', 'UA-3769691-2']);
      _gaq.push(['_trackPageview']);
      (function() {
        var ga = document.createElement('script');
        ga.src = ('https:' == document.location.protocol ? 'https://ssl' : 'http://www') + '.google-analytics.com/ga.js';
        ga.setAttribute('async', 'true');
        document.documentElement.firstChild.appendChild(ga);
      })();
    </script>

  </head>

  

  <body class="logged_out ">
    

    
      <script type="text/javascript">
        var _kmq = _kmq || [];
        function _kms(u){
          var s = document.createElement('script'); var f = document.getElementsByTagName('script')[0]; s.type = 'text/javascript'; s.async = true;
          s.src = u; f.parentNode.insertBefore(s, f);
        }
        _kms('//i.kissmetrics.com/i.js');_kms('//doug1izaerwt3.cloudfront.net/406e8bf3a2b8846ead55afb3cfaf6664523e3a54.1.js');
      </script>
    

    

    

    

    <div class="subnavd" id="main">
      <div id="header" class="true">
        
          <a class="logo boring" href="http://github.com">
            <img src="/images/modules/header/logov3.png?changed" class="default" alt="github" />
            <![if !IE]>
            <img src="/images/modules/header/logov3-hover.png" class="hover" alt="github" />
            <![endif]>
          </a>
        
        
        <div class="topsearch">
  
    <ul class="nav logged_out">
      <li><a href="http://github.com">Home</a></li>
      <li class="pricing"><a href="/plans">Pricing and Signup</a></li>
      <li><a href="http://github.com/training">Training</a></li>
      <li><a href="http://gist.github.com">Gist</a></li>
      <li><a href="/blog">Blog</a></li>
      <li><a href="https://github.com/login">Login</a></li>
    </ul>
  
</div>

      </div>

      
      
        
    <div class="site">
      <div class="pagehead repohead vis-public fork  ">
        <h1>
          <a href="/CyanogenMod">CyanogenMod</a> / <strong><a href="http://github.com/CyanogenMod/android_vendor_cyanogen">android_vendor_cyanogen</a></strong>
          
            <span class="fork-flag">
              
              <span class="text">forked from <a href="/wertarbyte/android_vendor_cyanogen">wertarbyte/android_vendor_cyanogen</a></span>
            </span>
          
          
        </h1>

        
    <ul class="actions">
      

      
        <li class="for-owner" style="display:none"><a href="https://github.com/CyanogenMod/android_vendor_cyanogen/edit" class="minibutton btn-admin "><span><span class="icon"></span>Admin</span></a></li>
        <li>
          <a href="/CyanogenMod/android_vendor_cyanogen/toggle_watch" class="minibutton btn-watch " id="watch_button" style="display:none"><span><span class="icon"></span>Watch</span></a>
          <a href="/CyanogenMod/android_vendor_cyanogen/toggle_watch" class="minibutton btn-watch " id="unwatch_button" style="display:none"><span><span class="icon"></span>Unwatch</span></a>
        </li>
        
          
            <li class="for-notforked" style="display:none"><a href="/CyanogenMod/android_vendor_cyanogen/fork" class="minibutton btn-fork " id="fork_button" onclick="var f = document.createElement('form'); f.style.display = 'none'; this.parentNode.appendChild(f); f.method = 'POST'; f.action = this.href;var s = document.createElement('input'); s.setAttribute('type', 'hidden'); s.setAttribute('name', 'authenticity_token'); s.setAttribute('value', 'bf5ba723a2915bae1fcfaf8c150189846ca050da'); f.appendChild(s);f.submit();return false;"><span><span class="icon"></span>Fork</span></a></li>
            <li class="for-hasfork" style="display:none"><a href="#" class="minibutton btn-fork " id="your_fork_button"><span><span class="icon"></span>Your Fork</span></a></li>
          

          
          <li id="pull_request_item" class='ospr' style="display:none"><a href="/CyanogenMod/android_vendor_cyanogen/pull_request/" class="minibutton btn-pull-request "><span><span class="icon"></span>Pull Request</span></a></li>
          

          <li><a href="#" class="minibutton btn-download " id="download_button"><span><span class="icon"></span>Download Source</span></a></li>
        
      
      
      <li class="repostats">
        <ul class="repo-stats">
          <li class="watchers"><a href="/CyanogenMod/android_vendor_cyanogen/watchers" title="Watchers" class="tooltipped downwards">116</a></li>
          <li class="forks"><a href="/CyanogenMod/android_vendor_cyanogen/network" title="Forks" class="tooltipped downwards">-12</a></li>
        </ul>
      </li>
    </ul>


        
  <ul class="tabs">
    <li><a href="http://github.com/CyanogenMod/android_vendor_cyanogen/tree/froyo" class="selected" highlight="repo_source">Source</a></li>
    <li><a href="http://github.com/CyanogenMod/android_vendor_cyanogen/commits/froyo" highlight="repo_commits">Commits</a></li>

    
    <li><a href="/CyanogenMod/android_vendor_cyanogen/network" highlight="repo_network">Network (-12)</a></li>

    

    

    

            
      <li><a href="http://wiki.github.com/CyanogenMod/android_vendor_cyanogen/">Wiki (2)</a></li>
      
    <li><a href="/CyanogenMod/android_vendor_cyanogen/graphs" highlight="repo_graphs">Graphs</a></li>

    <li class="contextswitch nochoices">
      <span class="toggle leftwards" >
        <em>Branch:</em>
        <code>froyo</code>
      </span>
    </li>
  </ul>

  <div style="display:none" id="pl-description"><p><em class="placeholder">click here to add a description</em></p></div>
  <div style="display:none" id="pl-homepage"><p><em class="placeholder">click here to add a homepage</em></p></div>

  <div class="subnav-bar">
  
  <ul>
    <li>
      <a href="#" class="dropdown">Switch Branches (3)</a>
      <ul>
        
          
          
            <li><a href="/CyanogenMod/android_vendor_cyanogen/blob/donut/prebuilt/common/xbin/openvpn-up.sh" action="show">donut</a></li>
          
        
          
          
            <li><a href="/CyanogenMod/android_vendor_cyanogen/blob/eclair/prebuilt/common/xbin/openvpn-up.sh" action="show">eclair</a></li>
          
        
          
            <li><strong>froyo &#x2713;</strong></li>
            
      </ul>
    </li>
    <li>
      <a href="#" class="dropdown defunct">Switch Tags (0)</a>
      
    </li>
    <li>
    
    <a href="/CyanogenMod/android_vendor_cyanogen/branches" class="manage">Branch List</a>
    
    </li>
  </ul>
</div>

  
  
  
  
  
  



        
    <div id="repo_details" class="metabox clearfix">
      <div id="repo_details_loader" class="metabox-loader" style="display:none">Sending Request&hellip;</div>

      

      <div id="repository_description" rel="repository_description_edit">
        
          <p>CyanogenMod Android vendor tree
            <span id="read_more" style="display:none">&mdash; <a href="#readme">Read more</a></span>
          </p>
        
      </div>
      <div id="repository_description_edit" style="display:none;" class="inline-edit">
        <form action="/CyanogenMod/android_vendor_cyanogen/edit/update" method="post"><div style="margin:0;padding:0"><input name="authenticity_token" type="hidden" value="bf5ba723a2915bae1fcfaf8c150189846ca050da" /></div>
          <input type="hidden" name="field" value="repository_description">
          <input type="text" class="textfield" name="value" value="CyanogenMod Android vendor tree">
          <div class="form-actions">
            <button class="minibutton"><span>Save</span></button> &nbsp; <a href="#" class="cancel">Cancel</a>
          </div>
        </form>
      </div>

      
      <div class="repository-homepage" id="repository_homepage" rel="repository_homepage_edit">
        <p><a href="http://cyanogenmod.com" rel="nofollow">http://cyanogenmod.com</a></p>
      </div>
      <div id="repository_homepage_edit" style="display:none;" class="inline-edit">
        <form action="/CyanogenMod/android_vendor_cyanogen/edit/update" method="post"><div style="margin:0;padding:0"><input name="authenticity_token" type="hidden" value="bf5ba723a2915bae1fcfaf8c150189846ca050da" /></div>
          <input type="hidden" name="field" value="repository_homepage">
          <input type="text" class="textfield" name="value" value="http://cyanogenmod.com">
          <div class="form-actions">
            <button class="minibutton"><span>Save</span></button> &nbsp; <a href="#" class="cancel">Cancel</a>
          </div>
        </form>
      </div>

      <div class="rule "></div>

            <div id="url_box" class="url-box">
        <ul class="clone-urls">
          
            
            <li id="http_clone_url"><a href="http://github.com/CyanogenMod/android_vendor_cyanogen.git" data-permissions="Read-Only">HTTP</a></li>
            <li id="public_clone_url"><a href="git://github.com/CyanogenMod/android_vendor_cyanogen.git" data-permissions="Read-Only">Git Read-Only</a></li>
          
        </ul>
        <input type="text" spellcheck="false" id="url_field" class="url-field" />
              <span style="display:none" id="url_box_clippy"></span>
      <span id="clippy_tooltip_url_box_clippy" class="clippy-tooltip tooltipped" title="copy to clipboard">
      <object classid="clsid:d27cdb6e-ae6d-11cf-96b8-444553540000"
              width="14"
              height="14"
              class="clippy"
              id="clippy" >
      <param name="movie" value="http://assets3.github.com/flash/clippy.swf?v5"/>
      <param name="allowScriptAccess" value="always" />
      <param name="quality" value="high" />
      <param name="scale" value="noscale" />
      <param NAME="FlashVars" value="id=url_box_clippy&amp;copied=&amp;copyto=">
      <param name="bgcolor" value="#FFFFFF">
      <param name="wmode" value="opaque">
      <embed src="http://assets3.github.com/flash/clippy.swf?v5"
             width="14"
             height="14"
             name="clippy"
             quality="high"
             allowScriptAccess="always"
             type="application/x-shockwave-flash"
             pluginspage="http://www.macromedia.com/go/getflashplayer"
             FlashVars="id=url_box_clippy&amp;copied=&amp;copyto="
             bgcolor="#FFFFFF"
             wmode="opaque"
      />
      </object>
      </span>

        <p id="url_description">This URL has <strong>Read+Write</strong> access</p>
      </div>
    </div>


        

      </div><!-- /.pagehead -->

      









<script type="text/javascript">
  GitHub.currentCommitRef = 'froyo'
  GitHub.currentRepoOwner = 'CyanogenMod'
  GitHub.currentRepo = "android_vendor_cyanogen"
  GitHub.downloadRepo = '/CyanogenMod/android_vendor_cyanogen/archives/froyo'
  GitHub.revType = "ref"

  GitHub.controllerName = "blob"
  GitHub.actionName     = "show"
  GitHub.currentAction  = "blob#show"

  

  
</script>










  <div id="commit">
    <div class="group">
        
  <div class="envelope commit">
    <div class="human">
      
        <div class="message"><pre><a href="/CyanogenMod/android_vendor_cyanogen/commit/e515751e1922e79558e64cf47c4e5a1172c6d36d">espresso: Fix copypasta- use the right overlay.</a> </pre></div>
      

      <div class="actor">
        <div class="gravatar">
          
          <img src="http://www.gravatar.com/avatar/345ef36929e4ecae65ed8fdee4ec92d0?s=140&d=http%3A%2F%2Fgithub.com%2Fimages%2Fgravatars%2Fgravatar-140.png" alt="" width="30" height="30"  />
        </div>
        <div class="name"><a href="/cyanogen">cyanogen</a> <span>(author)</span></div>
        <div class="date">
          <abbr class="relatize" title="2010-08-23 21:28:42">Mon Aug 23 21:28:42 -0700 2010</abbr>
        </div>
      </div>

      

    </div>
    <div class="machine">
      <span>c</span>ommit&nbsp;&nbsp;<a href="/CyanogenMod/android_vendor_cyanogen/commit/e515751e1922e79558e64cf47c4e5a1172c6d36d" hotkey="c">e515751e1922e79558e6</a><br />
      <span>t</span>ree&nbsp;&nbsp;&nbsp;&nbsp;<a href="/CyanogenMod/android_vendor_cyanogen/tree/e515751e1922e79558e64cf47c4e5a1172c6d36d/CHANGELOG.mkdn" hotkey="t">22427c2222c69e6f19c1</a><br />
      
        <span>p</span>arent&nbsp;
        
        <a href="/CyanogenMod/android_vendor_cyanogen/commit/fe136e0d31829be507e79f58fa500b6c50b67be5" hotkey="p">fe136e0d31829be507e7</a>
      

    </div>
  </div>

    </div>
  </div>



  
    <div id="path">
      <b><a href="/CyanogenMod/android_vendor_cyanogen/tree/froyo">android_vendor_cyanogen</a></b> / <a href="/CyanogenMod/android_vendor_cyanogen/tree/froyo/prebuilt">prebuilt</a> / <a href="/CyanogenMod/android_vendor_cyanogen/tree/froyo/prebuilt/common">common</a> / <a href="/CyanogenMod/android_vendor_cyanogen/tree/froyo/prebuilt/common/xbin">xbin</a> / openvpn-up.sh       <span style="display:none" id="clippy_4730">prebuilt/common/xbin/openvpn-up.sh</span>
      
      <object classid="clsid:d27cdb6e-ae6d-11cf-96b8-444553540000"
              width="110"
              height="14"
              class="clippy"
              id="clippy" >
      <param name="movie" value="http://assets3.github.com/flash/clippy.swf?v5"/>
      <param name="allowScriptAccess" value="always" />
      <param name="quality" value="high" />
      <param name="scale" value="noscale" />
      <param NAME="FlashVars" value="id=clippy_4730&amp;copied=copied!&amp;copyto=copy to clipboard">
      <param name="bgcolor" value="#FFFFFF">
      <param name="wmode" value="opaque">
      <embed src="http://assets3.github.com/flash/clippy.swf?v5"
             width="110"
             height="14"
             name="clippy"
             quality="high"
             allowScriptAccess="always"
             type="application/x-shockwave-flash"
             pluginspage="http://www.macromedia.com/go/getflashplayer"
             FlashVars="id=clippy_4730&amp;copied=copied!&amp;copyto=copy to clipboard"
             bgcolor="#FFFFFF"
             wmode="opaque"
      />
      </object>
      

    </div>

    <div id="files">
      <div class="file">
        <div class="meta">
          <div class="info">
            <span class="icon"><img alt="Txt" height="16" src="http://assets0.github.com/images/icons/txt.png?935bbdff6dd3f65d860d46e4de77e4ea817dc684" width="16" /></span>
            <span class="mode" title="File Mode">100644</span>
            
              <span>45 lines (40 sloc)</span>
            
            <span>0.899 kb</span>
          </div>
          <ul class="actions">
            
              <li><a id="file-edit-link" href="#" rel="/CyanogenMod/android_vendor_cyanogen/file-edit/__ref__/prebuilt/common/xbin/openvpn-up.sh">edit</a></li>
            
            <li><a href="/CyanogenMod/android_vendor_cyanogen/raw/froyo/prebuilt/common/xbin/openvpn-up.sh" id="raw-url">raw</a></li>
            
              <li><a href="/CyanogenMod/android_vendor_cyanogen/blame/froyo/prebuilt/common/xbin/openvpn-up.sh">blame</a></li>
            
            <li><a href="/CyanogenMod/android_vendor_cyanogen/commits/froyo/prebuilt/common/xbin/openvpn-up.sh">history</a></li>
          </ul>
        </div>
        
  <div class="data type-bash">
    
      <table cellpadding="0" cellspacing="0">
        <tr>
          <td>
            <pre class="line_numbers"><span id="LID1" rel="#L1">1</span>
<span id="LID2" rel="#L2">2</span>
<span id="LID3" rel="#L3">3</span>
<span id="LID4" rel="#L4">4</span>
<span id="LID5" rel="#L5">5</span>
<span id="LID6" rel="#L6">6</span>
<span id="LID7" rel="#L7">7</span>
<span id="LID8" rel="#L8">8</span>
<span id="LID9" rel="#L9">9</span>
<span id="LID10" rel="#L10">10</span>
<span id="LID11" rel="#L11">11</span>
<span id="LID12" rel="#L12">12</span>
<span id="LID13" rel="#L13">13</span>
<span id="LID14" rel="#L14">14</span>
<span id="LID15" rel="#L15">15</span>
<span id="LID16" rel="#L16">16</span>
<span id="LID17" rel="#L17">17</span>
<span id="LID18" rel="#L18">18</span>
<span id="LID19" rel="#L19">19</span>
<span id="LID20" rel="#L20">20</span>
<span id="LID21" rel="#L21">21</span>
<span id="LID22" rel="#L22">22</span>
<span id="LID23" rel="#L23">23</span>
<span id="LID24" rel="#L24">24</span>
<span id="LID25" rel="#L25">25</span>
<span id="LID26" rel="#L26">26</span>
<span id="LID27" rel="#L27">27</span>
<span id="LID28" rel="#L28">28</span>
<span id="LID29" rel="#L29">29</span>
<span id="LID30" rel="#L30">30</span>
<span id="LID31" rel="#L31">31</span>
<span id="LID32" rel="#L32">32</span>
<span id="LID33" rel="#L33">33</span>
<span id="LID34" rel="#L34">34</span>
<span id="LID35" rel="#L35">35</span>
<span id="LID36" rel="#L36">36</span>
<span id="LID37" rel="#L37">37</span>
<span id="LID38" rel="#L38">38</span>
<span id="LID39" rel="#L39">39</span>
<span id="LID40" rel="#L40">40</span>
<span id="LID41" rel="#L41">41</span>
<span id="LID42" rel="#L42">42</span>
<span id="LID43" rel="#L43">43</span>
<span id="LID44" rel="#L44">44</span>
<span id="LID45" rel="#L45">45</span>
</pre>
          </td>
          <td width="100%">
            
              <div class="highlight"><pre><div class='line' id='LC1'><span class="c">#!/system/xbin/bash</span></div><div class='line' id='LC2'><span class="c">#</span></div><div class='line' id='LC3'><span class="c"># /system/xbin/openvpn-up.sh v0.2</span></div><div class='line' id='LC4'><span class="c">#</span></div><div class='line' id='LC5'><span class="c"># Philip Freeman &lt;philip.freeman@gmail.com&gt;</span></div><div class='line' id='LC6'><span class="c">#</span></div><div class='line' id='LC7'><span class="c"># TODO: add support for grabbing search domains ?</span></div><div class='line' id='LC8'><span class="c">#</span></div><div class='line' id='LC9'><span class="c"># Changes:</span></div><div class='line' id='LC10'><span class="c">#-- v0.2</span></div><div class='line' id='LC11'><span class="c"># - Added system logging</span></div><div class='line' id='LC12'><span class="c"># - Fixed fome path issues</span></div><div class='line' id='LC13'><span class="c">#</span></div><div class='line' id='LC14'><br/></div><div class='line' id='LC15'><span class="nv">LOG</span><span class="o">=</span><span class="s2">"/system/bin/log -t openvpn-up"</span></div><div class='line' id='LC16'><span class="nv">SETPROP</span><span class="o">=</span>/system/bin/setprop</div><div class='line' id='LC17'><span class="nv">EXPR</span><span class="o">=</span>/system/xbin/expr</div><div class='line' id='LC18'><span class="nv">stop</span><span class="o">=</span>0</div><div class='line' id='LC19'><span class="nv">dns_num</span><span class="o">=</span>1</div><div class='line' id='LC20'><span class="nv">i</span><span class="o">=</span>0</div><div class='line' id='LC21'><br/></div><div class='line' id='LC22'><span class="k">${</span><span class="nv">LOG</span><span class="k">}</span> <span class="s2">"Starting..."</span></div><div class='line' id='LC23'><br/></div><div class='line' id='LC24'><span class="nb">eval </span><span class="nv">opt</span><span class="o">=</span><span class="se">\$</span>foreign_option_<span class="nv">$i</span></div><div class='line' id='LC25'><br/></div><div class='line' id='LC26'><span class="k">while</span> <span class="o">[</span> <span class="k">${</span><span class="nv">stop</span><span class="k">}</span> -eq 0 <span class="o">]</span>; <span class="k">do</span></div><div class='line' id='LC27'><span class="k">  if</span> <span class="o">[</span> <span class="s2">"`${EXPR} substr "</span><span class="nv">$opt</span><span class="s2">" 1 11`"</span> <span class="o">=</span> <span class="s2">"dhcp-option"</span> <span class="o">]</span>; <span class="k">then</span></div><div class='line' id='LC28'><span class="k">    if</span> <span class="o">[</span> <span class="s2">"`${EXPR} substr "</span><span class="nv">$opt</span><span class="s2">" 13 3`"</span> <span class="o">=</span> <span class="s2">"DNS"</span> <span class="o">]</span>; <span class="k">then</span></div><div class='line' id='LC29'><span class="k">      </span><span class="nv">DNS</span><span class="o">=</span><span class="s2">"`${EXPR} substr "</span><span class="nv">$opt</span><span class="s2">" 17 1024`"</span></div><div class='line' id='LC30'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">${</span><span class="nv">LOG</span><span class="k">}</span> <span class="s2">"Got DNS${dns_num}: ${DNS}"</span></div><div class='line' id='LC31'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">if</span> <span class="o">[</span> <span class="k">${</span><span class="nv">dns_num</span><span class="k">}</span> -le 2 <span class="o">]</span>; <span class="k">then</span></div><div class='line' id='LC32'>	<span class="c">#Set it</span></div><div class='line' id='LC33'>	<span class="k">${</span><span class="nv">LOG</span><span class="k">}</span> <span class="k">${</span><span class="nv">SETPROP</span><span class="k">}</span> vpn.dns<span class="k">${</span><span class="nv">dns_num</span><span class="k">}</span> <span class="k">${</span><span class="nv">DNS</span><span class="k">}</span></div><div class='line' id='LC34'>	<span class="k">${</span><span class="nv">SETPROP</span><span class="k">}</span> vpn.dns<span class="k">${</span><span class="nv">dns_num</span><span class="k">}</span> <span class="k">${</span><span class="nv">DNS</span><span class="k">}</span></div><div class='line' id='LC35'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">fi</span></div><div class='line' id='LC36'><span class="k">      </span><span class="nv">dns_num</span><span class="o">=</span><span class="k">$((</span> <span class="k">${</span><span class="nv">dns_num</span><span class="k">}</span><span class="o">+</span><span class="m">1</span> <span class="k">))</span></div><div class='line' id='LC37'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">fi</span></div><div class='line' id='LC38'><span class="k">  fi</span></div><div class='line' id='LC39'><span class="k">  </span><span class="nv">i</span><span class="o">=</span><span class="k">$((</span> <span class="nv">$i</span><span class="o">+</span><span class="m">1</span> <span class="k">))</span></div><div class='line' id='LC40'>&nbsp;&nbsp;<span class="nb">eval </span><span class="nv">opt</span><span class="o">=</span><span class="se">\$</span>foreign_option_<span class="nv">$i</span></div><div class='line' id='LC41'>&nbsp;&nbsp;<span class="k">if</span> <span class="o">[</span> <span class="s2">"$opt"</span> <span class="o">=</span> <span class="s2">""</span> <span class="o">]</span>; <span class="k">then</span></div><div class='line' id='LC42'><span class="k">    </span><span class="nv">stop</span><span class="o">=</span>1</div><div class='line' id='LC43'>&nbsp;&nbsp;<span class="k">fi</span></div><div class='line' id='LC44'><span class="k">done</span></div><div class='line' id='LC45'><br/></div></pre></div>
            
          </td>
        </tr>
      </table>
    
  </div>


      </div>
    </div>

  


    </div>
  
      
    </div>

    <div id="footer" class="clearfix">
      <div class="site">
        <div class="sponsor">
          <a href="http://www.rackspace.com" class="logo">
            <img alt="Dedicated Server" src="http://assets0.github.com/images/modules/footer/rackspace_logo.png?935bbdff6dd3f65d860d46e4de77e4ea817dc684" />
          </a>
          Powered by the <a href="http://www.rackspace.com ">Dedicated
          Servers</a> and<br/> <a href="http://www.rackspacecloud.com">Cloud
          Computing</a> of Rackspace Hosting<span>&reg;</span>
        </div>

        <ul class="links">
          <li class="blog"><a href="http://github.com/blog">Blog</a></li>
          <li><a href="http://support.github.com">Support</a></li>
          <li><a href="http://github.com/training">Training</a></li>
          <li><a href="http://jobs.github.com">Job Board</a></li>
          <li><a href="http://shop.github.com">Shop</a></li>
          <li><a href="http://github.com/contact">Contact</a></li>
          <li><a href="http://develop.github.com">API</a></li>
          <li><a href="http://status.github.com">Status</a></li>
        </ul>
        <ul class="sosueme">
          <li class="main">&copy; 2010 <span id="_rrt" title="0.03940s from fe1.rs.github.com">GitHub</span> Inc. All rights reserved.</li>
          <li><a href="/site/terms">Terms of Service</a></li>
          <li><a href="/site/privacy">Privacy</a></li>
          <li><a href="http://github.com/security">Security</a></li>
        </ul>
      </div>
    </div><!-- /#footer -->

    
      
      
        <!-- current locale:  -->
        <div class="locales">
          <div class="site">

            <ul class="choices clearfix limited-locales">
              <li><span class="current">English</span></li>
              
                
                  <li><a rel="nofollow" href="?locale=de">Deutsch</a></li>
                
              
                
                  <li><a rel="nofollow" href="?locale=fr">Français</a></li>
                
              
                
                  <li><a rel="nofollow" href="?locale=ja">日本語</a></li>
                
              
                
                  <li><a rel="nofollow" href="?locale=pt-BR">Português (BR)</a></li>
                
              
                
                  <li><a rel="nofollow" href="?locale=zh">中文</a></li>
                
              
              <li class="all"><a href="#" class="minibutton btn-forward js-all-locales"><span><span class="icon"></span>See all available languages</span></a></li>
            </ul>

            <div class="all-locales clearfix">
              <h3>Your current locale selection: <strong>English</strong>. Choose another?</h3>
              
              
                <ul class="choices">
                  
                    
                      <li><a rel="nofollow" href="?locale=en">English</a></li>
                    
                  
                    
                      <li><a rel="nofollow" href="?locale=af">Afrikaans</a></li>
                    
                  
                    
                      <li><a rel="nofollow" href="?locale=ca">Català</a></li>
                    
                  
                    
                      <li><a rel="nofollow" href="?locale=cs">Čeština</a></li>
                    
                  
                </ul>
              
                <ul class="choices">
                  
                    
                      <li><a rel="nofollow" href="?locale=de">Deutsch</a></li>
                    
                  
                    
                      <li><a rel="nofollow" href="?locale=es">Español</a></li>
                    
                  
                    
                      <li><a rel="nofollow" href="?locale=fr">Français</a></li>
                    
                  
                    
                      <li><a rel="nofollow" href="?locale=hr">Hrvatski</a></li>
                    
                  
                </ul>
              
                <ul class="choices">
                  
                    
                      <li><a rel="nofollow" href="?locale=id">Indonesia</a></li>
                    
                  
                    
                      <li><a rel="nofollow" href="?locale=it">Italiano</a></li>
                    
                  
                    
                      <li><a rel="nofollow" href="?locale=ja">日本語</a></li>
                    
                  
                    
                      <li><a rel="nofollow" href="?locale=nl">Nederlands</a></li>
                    
                  
                </ul>
              
                <ul class="choices">
                  
                    
                      <li><a rel="nofollow" href="?locale=no">Norsk</a></li>
                    
                  
                    
                      <li><a rel="nofollow" href="?locale=pl">Polski</a></li>
                    
                  
                    
                      <li><a rel="nofollow" href="?locale=pt-BR">Português (BR)</a></li>
                    
                  
                    
                      <li><a rel="nofollow" href="?locale=sr">Српски</a></li>
                    
                  
                </ul>
              
                <ul class="choices">
                  
                    
                      <li><a rel="nofollow" href="?locale=sv">Svenska</a></li>
                    
                  
                    
                      <li><a rel="nofollow" href="?locale=zh">中文</a></li>
                    
                  
                </ul>
              
            </div>

          </div>
          <div class="fade"></div>
        </div>
      
    

    <script>window._auth_token = "bf5ba723a2915bae1fcfaf8c150189846ca050da"</script>
    

    <!--[if IE 8]>
    <script type="text/javascript" charset="utf-8">
      $(document.body).addClass("ie8")
    </script>
    <![endif]-->

    <!--[if IE 7]>
    <script type="text/javascript" charset="utf-8">
      $(document.body).addClass("ie7")
    </script>
    <![endif]-->

    <script type="text/javascript">
      _kmq.push(['trackClick', 'entice_banner_link', 'Entice banner clicked']);
      
    </script>
    
  </body>
</html>

