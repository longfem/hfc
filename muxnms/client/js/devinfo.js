var _selectcount = 0;//节目计数
var _intChannelCntMax = 0;//设备输入通道数
var _tbleditcount = 0;//edit表stream计数
var _pmtPid = 0;//编辑节目pid
var _editnodekey = ""; //编辑节目节点key
var _channel = 1; //操作中的通道
var _tbl_edit;
var _tbl_pid;
var channel_root = [
	{"title": "输入通道", folder: true, key: "id1.0", expanded: true, "expanded": true, "icon": "img/book.ico"}	  
];

var table_root = [
	{"title": "表", folder: true, key: "id1.0", expanded: true, "expanded": true, "icon": "img/book.ico"}	  
];

var programData = [
	{title: "节目", folder: true, key: "id1.0", expanded: true, "icon": "img/book.ico",
	  children: [
		{title: "通道1 (ASI-1)", folder: true, key: "id1.1", expanded: true, "icon": "img/channel_out.ico"},
		{title: "通道2 (ASI-2)", folder: true, key: "id1.2", expanded: true, "icon": "img/channel_out.ico"},
		{title: "通道3 (ASI-3)", folder: true, key: "id1.3", expanded: true, "icon": "img/channel_out.ico"},
		{title: "通道4 (ASI-4)", folder: true, key: "id1.4", expanded: true, "icon": "img/channel_out.ico"},
		{title: "通道5 (ASI-5)", folder: true, key: "id1.5", expanded: true, "icon": "img/channel_out.ico"},
		{title: "通道6 (ASI-6)", folder: true, key: "id1.6", expanded: true, "icon": "img/channel_out.ico"},
		{title: "通道7 (ASI-7)", folder: true, key: "id1.7", expanded: true, "icon": "img/channel_out.ico"},
		{title: "通道8 (ASI-8)", folder: true, key: "id1.8", expanded: true, "icon": "img/channel_out.ico"},
		{title: "用户自定义", folder: true, key: "id1.9", expanded: true, "icon": "img/book.ico"}
	  ]
	}	
];

var dataSet = [
	['0','2','MPEG2 Video','488','488'],
	['1','2','MPEG2 Audio','460','460'],
	['2','2','MPEG2 Audio','461','461'],
	['3','2','MPEG2 Audio','456','456']	
];

var dataSet1 = [
	['0','2'],
	['1','3'],
	['2','22'],
	['3','24']	
];

function checkselectedprg(){
	var nodes = $("#channel").fancytree("getTree").getNodeByKey("id1.0").children;
	var inCh = 1, flag = 0; //通道号	
	var jsondata = new Array();
	var prgindex = new Array();
	var jsonstr;
	nodes.forEach(function(node) {
		flag = 0;
		prgindex = new Array();
		var chstr = "inCh" + inCh;
		if( node.hasChildren() ) {
			var prgnodes = node.children;	
			prgnodes.forEach(function(prgnode) {
				var tmpstr = '{id:' + prgnode.data.index;
				var offset = 0;
				prgnode.children.forEach(function(streamnode){
					if(streamnode.data.index){
						tmpstr += ',index'+offset+':'+streamnode.data.index;
						offset++;
					};					
				})
				prgindex[flag] = 'id' + flag + ':' + tmpstr + '}';
				flag++;
			});					
		}
		jsonstr = chstr+':{' + prgindex.toString() +'}';
		jsondata[inCh-1] = jsonstr;
		inCh++;
	});	
	$.ajax({
		 type: "GET",
		 async:false,
		 url: "http://"+localip+":4000/do/programs/selectprgs",
		 data: '{' + jsondata.toString() + ',channel:'+1+',prgcnt:'+_selectcount+'}',
		 dataType: "json",
		 success: function(data){
			
		 },    
		 error : function(err) { 
			alert(err);
		 }   
	});
}

function readprgs(){
    $("#devlist").fancytree("getTree").reload();
    $("#channel").fancytree("getTree").reload();
    var node = $("#devlist").fancytree("getTree").getNodeByKey("id1.0");
    var localip = window.location.href.substr(7, window.location.href.indexOf(':', 7) - 7);
    //获取全局初始化信息
    $.ajax({
        type: "GET",
        async:false,
        url: "http://"+localip+":4000/do/programs/getglobalinfo",
        // data: {ip:"192.168.1.134", inch:2},
        dataType: "json",
        success: function(data){
            _intChannelCntMax = data._intChannelCntMax;
        },
        error : function(err) {

        }
    });

    if(_intChannelCntMax != 0 || _intChannelCntMax != ""){
        for(var i=1;i<_intChannelCntMax+1; i++){
            $.ajax({
                type: "GET",
                async:false,
                url: "http://"+localip+":4000/do/programs/getprg?inch="+i,
                // data: {ip:"192.168.1.134", inch:2},
                dataType: "json",
                success: function(data){
                    data = JSON.stringify(data).replace('\\','');
                    //alert("-----"+data);
                    var treeData1 = JSON.parse(data);
                    node.addChildren(treeData1);
                },
                error : function(err) {
                    // view("异常！");
                    var xxx = err;
                    alert("异常！====="+JSON.stringify(err));
                }
            });
        }
    }

    //获取输出通道信息
    $.ajax({
        type: "GET",
        async:false,
        url: "http://"+localip+":4000/do/programs/getoutprg?inch=1",
        // data: {ip:"192.168.1.134", inch:2},
        dataType: "json",
        success: function(data){
            if(data.sts == 1){

            }else{
                $.each(data, function(key, prg) {
                    var nkey = "id1." + prg.ch;
                    node = $("#channel").fancytree("getTree").getNodeByKey(nkey);
                    node.addChildren(prg.children);
                    var prgkey = "id1." + prg.ch +"."+prg.pmtPid;
                    node = $("#devlist").fancytree("getTree").getNodeByKey(prgkey);
                    node.setSelected(true);
                });
            }

        },
        error : function(err) {
            // view("异常！");
            var xxx = err;
            alert("异常！====="+JSON.stringify(err));
        }
    });
}
	
function devinfo_output(devType){
	$('.main-content').empty();
	if(devType == "8合1复用器"){
		$('.main-content').append(
			'<div id="devoutput">'
				+'<ul>'
					+'<li><a href="#tabs-1">输出通道1</a></li>'
					+'<li><a href="#tabs-2">表结构1</a></li>'
					+'<li><a href="#tabs-3">输出通道2</a></li>'
					+'<li><a href="#tabs-4">表结构2</a></li>'
				+'</ul>'
				+'<div id="tabs-1">'
					+'<div class="clearfix">'
						+'<div id="devlist" class="channel_input"></div>'
						+'<div id="channel" class="program"></div>'
					+'</div>'
					+'<div class="tbn_div">'
                        +'<button id="output-search" class="STRING_SEARCH">搜索</button>'
						+'<button id="output-read" class="STRING_READ">读取</button>'
						+'<button id="output-table" class="STRING_WRITE">制表</button>'
						+'<button id="output-write" class="STRING_WRITE">应用</button>'					
					+'</div>'
				+'</div>'
				+'<div id="tabs-2">'
					+'<div class="clearfix">'
						+'<div id="out_tree" class="tbl_struct"></div>'
						+'<div id="out_table">'
							+'<table cellpadding="0" cellspacing="0" border="0" class="cell-border compact hover" id="tbl_outtable"></table>'
						+'</div>'
						
					+'</div>'				
				+'</div>'
				+'<div id="tabs-3">'
					+'<div class="clearfix">'
						+'<div id="devlist2" class="channel_input"></div>'
						+'<div id="channel2" class="program"></div>'
					+'</div>'
					+'<div class="tbn_div">'
						+'<button id="output-read2" class="STRING_READ">读取</button>'
						+'<button id="output-table2" class="STRING_WRITE">制表</button>'
						+'<button id="output-write2" class="STRING_WRITE">应用</button>'					
					+'</div>'
				+'</div>'
				+'<div id="tabs-4">'
					+'<div class="clearfix">'
						+'<div id="out_tree2" class="tbl_struct"></div>'
						+'<div id="out_table">'
							+'<table cellpadding="0" cellspacing="0" border="0" class="cell-border compact hover" id="tbl_outtable2"></table>'
						+'</div>'
						
					+'</div>'				
				+'</div>'
			+'</div>'
		);
	}else{
		$('.main-content').append(
			'<div id="devoutput">'
				+'<ul>'
					+'<li><a href="#tabs-1">输出通道1</a></li>'
					+'<li><a href="#tabs-2">表结构1</a></li>'
				+'</ul>'
				+'<div id="tabs-1">'
					+'<div class="clearfix">'
						+'<div id="devlist" class="channel_input"></div>'
						+'<div id="channel" class="program"></div>'
					+'</div>'
					+'<div class="tbn_div">'
                        +'<button id="output-search" class="STRING_SEARCH">搜索</button>'
						+'<button id="output-read" class="STRING_READ">读取</button>'
						+'<button id="output-table" class="STRING_WRITE">制表</button>'
						+'<button id="output-write" class="STRING_WRITE">应用</button>'					
					+'</div>'
				+'</div>'
				+'<div id="tabs-2">'
					+'<div class="clearfix">'
						+'<div id="out_tree" class="tbl_struct"></div>'
						+'<div id="out_table">'
							+'<table cellpadding="0" cellspacing="0" border="0" class="cell-border compact hover" id="tbl_outtable"></table>'
						+'</div>'
						
					+'</div>'				
				+'</div>'				
			+'</div>'
		);
	}
	$('.main-content').append(
		'<div id="dialog-NIT" title="NIT段编辑">'			
			+'<div class="nit_edit">'
				+'<label>网络ID &nbsp</label>'
				+'<input type="text" class="nit_id" value=""></input>&nbsp&nbsp(十六进制)<br/>'
				+'<label>网络名称</label>'
				+'<input type="text" class="nit_name" value="段1"></input>'
				+'<br/>'
				+'<textarea rows="8" style="width:350px;margin-left:30px">'
				+'</textarea>'
			+'</div>'
		+'</div>'
		+'<!-- Definition of context menu -->'
		+'<ul id="table_menu" class="contextMenu ui-helper-hidden">'
			+'<li class="menu_expandall"><a href="#expandall"><span class="ui-icon ui-icon-folder-open"></span>展开所有子节点</a></li>'
			+'<li class="menu_collasp"><a href="#collasp"><span class="ui-icon ui-icon-folder-collapsed"></span>收起节点</a></li>'
			+'<li>---</li>'
			+'<li class="menu_add"><a href="#add"><span class="ui-icon ui-icon-plusthick"></span>添加段</a></li>'
			+'<li class="menu_delete"><a href="#delete"><span class="ui-icon ui-icon-closethick"></span>删除段</a></li>'
			+'<li>---</li>'
			+'<li class="menu_import"><a href="#import"><span class="ui-icon ui-icon-pencil"></span>NIT表导入</a></li>'			
			+'<li class="menu_export"><a href="#export"><span class="ui-icon ui-icon-closethick"></span>NIT表导出</a></li>'			
			
		+'</ul>'
		+'<ul id="table_menu2" class="contextMenu ui-helper-hidden">'
			+'<li class="menu_expandall"><a href="#expandall"><span class="ui-icon ui-icon-folder-open"></span>展开所有子节点</a></li>'
			+'<li class="menu_collasp"><a href="#collasp"><span class="ui-icon ui-icon-folder-collapsed"></span>收起节点</a></li>'
			+'<li>---</li>'
			+'<li class="menu_add"><a href="#add"><span class="ui-icon ui-icon-plusthick"></span>添加段</a></li>'
			+'<li class="menu_delete"><a href="#delete"><span class="ui-icon ui-icon-closethick"></span>删除段</a></li>'
			+'<li>---</li>'
			+'<li class="menu_import"><a href="#import"><span class="ui-icon ui-icon-pencil"></span>NIT表导入</a></li>'			
			+'<li class="menu_export"><a href="#export"><span class="ui-icon ui-icon-closethick"></span>NIT表导出</a></li>'			
			
		+'</ul>'
		+'<!-- Definition of context menu -->'
		+'<ul id="program_menu" class="contextMenu ui-helper-hidden">'
			+'<li class="menu_expandall"><a href="#expandall"><span class="ui-icon ui-icon-folder-open"></span>展开所有子节点</a></li>'
			+'<li class="menu_collasp"><a href="#collasp"><span class="ui-icon ui-icon-folder-collapsed"></span>收起节点</a></li>'
			+'<li>---</li>'
			+'<li class="menu_delete"><a href="#delete"><span class="ui-icon ui-icon-closethick"></span>删除这个节目</a></li>'
			+'<li class="menu_edit"><a href="#edit"><span class="ui-icon ui-icon-pencil"></span>编辑这个节目</a></li>'			
			+'<li class="menu_deleteall"><a href="#deleteall"><span class="ui-icon ui-icon-closethick"></span>删除下一级所有自增描述符</a></li>'			
			+'<li class="menu_add"><a href="#add"><span class="ui-icon ui-icon-plusthick"></span>添加自增描述符</a></li>'
			
			+'<li class="menu_prgdeleteall" style="display:none"><a href="#prgdeleteall"><span class="ui-icon ui-icon-closethick"></span>删除所有节目</a></li>'
			+'<li class="menu_prgitems" style="display:none"><a href="#itmes"><span class="ui-icon ui-icon-tag"></span>选项</a></li>'			
			+'<li class="menu_re_prg" style="display:none"><a href="#re_prg"><span class="ui-icon ui-icon-refresh"></span>重新分配节目号</a></li>'			
			+'<li class="menu_re_pid" style="display:none"><a href="#re_pid"><span class="ui-icon ui-icon-refresh"></span>重新分配PID</a></li>'
			+'<li class="menu_pidtrans" style="display:none"><a href="#pidtrans"><span class="ui-icon ui-icon-pin-s"></span>PID透传</a></li>'
		+'</ul>'
		+'<ul id="inputprg_menu" class="contextMenu ui-helper-hidden">'
			+'<li class="menu_expandall"><a href="#expandall"><span class="ui-icon ui-icon-folder-open"></span>展开所有子节点</a></li>'
			+'<li class="menu_collasp"><a href="#collasp"><span class="ui-icon ui-icon-folder-collapsed"></span>收起节点</a></li>'
			+'<li>---</li>'
			+'<li class="menu_delete"><a href="#delete"><span class="ui-icon ui-icon-closethick"></span>删除此通道下所有节目</a></li>'
		+'</ul>'
		+'<ul id="program_menu2" class="contextMenu ui-helper-hidden">'
			+'<li class="menu_expandall"><a href="#expandall"><span class="ui-icon ui-icon-folder-open"></span>展开所有子节点</a></li>'
			+'<li class="menu_collasp"><a href="#collasp"><span class="ui-icon ui-icon-folder-collapsed"></span>收起节点</a></li>'
			+'<li>---</li>'
			+'<li class="menu_delete"><a href="#delete"><span class="ui-icon ui-icon-closethick"></span>删除这个节目</a></li>'
			+'<li class="menu_edit"><a href="#edit"><span class="ui-icon ui-icon-pencil"></span>编辑这个节目</a></li>'			
			+'<li class="menu_deleteall"><a href="#deleteall"><span class="ui-icon ui-icon-closethick"></span>删除下一级所有自增描述符</a></li>'			
			+'<li class="menu_add"><a href="#add"><span class="ui-icon ui-icon-plusthick"></span>添加自增描述符</a></li>'
			
			+'<li class="menu_prgdeleteall" style="display:none"><a href="#prgdeleteall"><span class="ui-icon ui-icon-closethick"></span>删除所有节目</a></li>'
			+'<li class="menu_prgitems" style="display:none"><a href="#itmes"><span class="ui-icon ui-icon-tag"></span>选项</a></li>'			
			+'<li class="menu_re_prg" style="display:none"><a href="#re_prg"><span class="ui-icon ui-icon-refresh"></span>重新分配节目号</a></li>'			
			+'<li class="menu_re_pid" style="display:none"><a href="#re_pid"><span class="ui-icon ui-icon-refresh"></span>重新分配PID</a></li>'
			+'<li class="menu_pidtrans" style="display:none"><a href="#pidtrans"><span class="ui-icon ui-icon-pin-s"></span>PID透传</a></li>'
		+'</ul>'
		+'<ul id="inputprg_menu2" class="contextMenu ui-helper-hidden">'
			+'<li class="menu_expandall"><a href="#expandall"><span class="ui-icon ui-icon-folder-open"></span>展开所有子节点</a></li>'
			+'<li class="menu_collasp"><a href="#collasp"><span class="ui-icon ui-icon-folder-collapsed"></span>收起节点</a></li>'
			+'<li>---</li>'
			+'<li class="menu_delete"><a href="#delete"><span class="ui-icon ui-icon-closethick"></span>删除此通道下所有节目</a></li>'
		+'</ul>'
		+'<div id="dialog-form" title="编辑节目">'
			+'<table class="tbl_program">'
				+'<tr>'
					+'<td><label>名称</label></td>'
					+'<td><input type="text" class="prg_name" value=""></input></td>'
					+'<td><label>业务类型</label></td>'
					+'<td>'
						+'<select id="r_servicetype">'
							+'<option value ="1">[1]digital television service</option><option value ="2">[2]digital radio sound service</option>'
							+'<option value ="3">[3]teletext service</option><option value ="4">[4]NVOD refrence service</option>'
							+'<option value ="5">[5]NVOD time-shifted service</option><option value ="6">[6]mosaic service</option>'
							+'<option value ="7">[7]PAL coded signal</option><option value ="8">[8]SECAN coded signal</option>'
							+'<option value ="9">[9]D/D2 MAC</option><option value ="10">[10]FM radio</option>'
							+'<option value ="11">[11]NTSC coded signal</option><option value ="12">[12]data broadcast service</option>'
							+'<option value ="13">[13]reserved for CIU(EN 50221[16])</option><option value ="14">[14]RCS Map(see EN 301 790[34])</option>'
							+'<option value ="15">[15]RCS FLS(see EN 301 790[34])</option><option value ="16">[16]DVB MHP service</option>'
							+'<option value ="17">[17]MPEG-2 HD digital television service</option><option value ="18">[18]Reserved</option>'
							+'<option value ="19">[19]Reserved</option><option value ="20">[20]Reserved</option>'
							+'<option value ="21">[21]Reserved</option><option value ="22">[22]advanced codec SD digital television service</option>'
							+'<option value ="23">[23]advanced codec SD NVOD time-shifted service</option><option value ="24">[24]advanced codec SD NVOD refrence service</option>'
							+'<option value ="25">[25]advanced codec HD digital television service</option><option value ="26">[26]advanced codec HD NVOD time-shifted service</option>'
							+'<option value ="27">[27]advanced codec HD NVOD refrence service</option>'
						+'</select></td>'
				+'</tr>'
				+'<tr>'
					+'<td><label>提供商名称</label></td>'
					+'<td><input type="text" class="prg_merchant" value=""></input></td>'
					+'<td><label>节目号</label></td>'
					+'<td><input type="text" style="width:50px" class="prg_no" value=""></input> <label>PMT_PID(Hex)</label><input style="width:50px" type="text" class="prg_pid" value="1389"></input></td>'
				+'</tr>'
				+'<tr>'
					+'<td><label>PRC通道</label></td>'
					+'<td><input disabled="disabled" type="text" class="prg_prc" value="2"></input></td>'
					+'<td><label>PRC_PID(Hex)</label></td>'
					+'<td><input type="text" style="width:50px" class="prg_prc1" value="488"></input> --><input style="width:50px" type="text" class="prg_prc2" value="488"></input><input style="margin-left:10px" type="checkbox">加扰</input></td>'
				+'</tr>'
			+'</table>'
			+'<div class="tbl_editprg">'
				+'<table cellpadding="0" cellspacing="0" border="0" class="cell-border compact hover" id="tbl_editprg"></table>'
			+'</div>'
		+'</div>'
		+'<div id="dialog-items" title="输出通道设置">'
			+'<table class="tbl_items">'
				+'<tr>'
					+'<td><label>传输流ID</label></td><td><input type="text" class="item_transid" value=""></input></td>'
				+'</tr>'
				+'<tr>'
					+'<td><label>网络ID</label></td><td><input type="text" class="item_netid" value=""></input></td>'
				+'</tr>'
				+'<tr>'
					+'<td><label>原始网络ID</label></td><td><input type="text" class="item_orignetid" value=""></input></td>'
				+'</tr>'
				+'<tr>'
					+'<td><label>输出总码率  (Kpbs)</label></td><td><input type="text" class="item_out" value=""></input></td>'
				+'</tr>'
				+'<tr>'
					+'<td><input class="autoinc_ver" type="checkbox">    自动增长版本号</input></td><td><label class="item_version">2</label></td>'
				+'</tr>'
			+'</table>'
			+'<input type="checkbox" class="pat_auto">    当生成PAT时按业务ID排序</input>'
			+'<fieldset>'
				+'<legend>选择需要的表</legend>'
					+'<input type="checkbox" class="sl_pat">    PAT</input>'
					+'<input type="checkbox" class="sl_pmt">    PMT</input>'
					+'<input type="checkbox" class="sl_sdt">    SDT</input>'
					+'<input type="checkbox" class="sl_cat">    CAT</input>'
					+'<input type="checkbox" class="sl_nit">    NIT</input>'
			+'</fieldset>'
			+'<label id="tag_channel" style="display:none"></label>'
		+'</div>'
		+'<div id="dialog-pid" title="PID表">'
			+'<table cellpadding="0" cellspacing="0" border="0" class="cell-border compact hover" id="tbl_pid"></table>'
		+'</div>'
		+'<div id="dialog-descriptor" title="描述符">'
			+'<div class="clearfix">'
				+'<div class="tbl_descriptor">'
					+'<table cellpadding="0" cellspacing="0" border="0" class="cell-border compact hover" id="tbl_descriptor"></table>'
				+'</div>'				
				+'<div class="desc_content">'
					+'<h3>User define descrptor</h3>'
					+'<label>标签</label> &nbsp&nbsp&nbsp&nbsp <input class="desc_tag" value=""></input>&nbsp&nbsp (Hex  eg:4A)'
					+'<br />'
					+'<label>长度</label> &nbsp&nbsp&nbsp&nbsp <label class="desc_length"></label>'
					+'<br />'
					+'<label style="float:left">字节</label> <textarea class="desc_bytes" row="6"></textarea> <br/><br/>'
					+'<h5>空格分离每个字节，值为十六进制.例：2D 3C 14</h5>'
				+'</div>'
			+'</div>'
		+'</div>'
        +'<div id="progress-dialog" title="搜索">'
            +'<div class="progress-label" data-ch = 1>正在准备搜索...</div>'
            +'<div id="progressbar"></div>'
        +'</div>'
	);
	
	//描述符表
	$('#tbl_descriptor').dataTable( {
		"data": dataSet1,
		"order": [[ 0, "asc" ]],
		"paging":   false,
		"info":     false,
		"searching":   false,
		"scrollCollapse": true,
		"columns": [
			{ "title": "标签", "width":"40%" },
			{ "title": "描述符"}
		]
	});	 	

	var tabs = $("#devoutput").tabs();
    $( "#output-search" ).button({
        icons: {
            primary: "ui-icon-search"
        }
    }).click(function( event ) {
        event.preventDefault();
        if(_intChannelCntMax == 0){
            $.ajax({
                type: "GET",
                async:false,
                url: "http://"+localip+":4000/do/programs/getglobalinfo",
                // data: {ip:"192.168.1.134", inch:2},
                dataType: "json",
                success: function(data){
                    _intChannelCntMax = data._intChannelCntMax;
                },
                error : function(err) {

                }
            });
        }
        if(_intChannelCntMax != 0 || _intChannelCntMax != ""){
            progress_dialog.dialog( "open" );
            progressbar.progressbar( "value", 0 );
            for(var i=1;i<_intChannelCntMax+1; i++){
                $.ajax({
                    type: "GET",
                    async:false,
                    url: "http://"+localip+":4000/do/programs/search?inch="+i,
                    // data: {ip:"192.168.1.134", inch:2},
                    dataType: "json",
                    success: function(data){
                        //更新进度条
                        if(data.sts != 0){
                            alert("搜索出现异常!!");
                            return;
                        }
                        if(_intChannelCntMax == data.process){
                            progressbar.progressbar( "value", 10 * 10 );
                        }else{
                            progressbar.progressbar( "value", Math.round(data.process*100/_intChannelCntMax ));
                            $('.progress-label')[0].dataset.ch = data.process + 1;
                        }

                    },
                    error : function(err) {
                        // view("异常！");
                        var xxx = err;
                        alert("异常！====="+JSON.stringify(err));
                    }
                });
            }
        }
    });

	$( "#output-read" ).button({
      icons: {
        primary: "ui-icon-comment"
      }
    }).click(function( event ) {
        event.preventDefault();
		readprgs();
    });
	
	$( "#output-table" ).button({
      icons: {
        primary: "ui-icon-gear"
      }
    }).click(function( event ) {
        event.preventDefault();
		$("#out_tree").fancytree("getTree").reload();
		var nodes = $("#channel").fancytree("getTree").getNodeByKey("id1.0").children;
		var inCh = 1, flag = 0; //通道号	
		var jsondata = new Array();
		var prgindex = new Array();
		var jsonstr;
		if(_selectcount == 0){
			alert("请选择节目!");
			return;
		}
		nodes.forEach(function(node) {
			flag = 0;
			prgindex = new Array();
			var chstr = "inCh" + inCh;
			if( node.hasChildren() ) {
				var prgnodes = node.children;				
				prgnodes.forEach(function(prgnode) {					
					prgindex[flag] = 'id' + flag + ':' + prgnode.data.index;
					flag++;
				});					
			}
			jsonstr = chstr+':{' + prgindex.toString() +'}';
			jsondata[inCh-1] = jsonstr;
			inCh++;
		});	
		//alert("=========arraystring======>>>"+ jsondata.toString());
		$.ajax({
			 type: "GET",
			 async:false,
			 url: "http://"+localip+":4000/do/programs/maketable",
			 data: '{' + jsondata.toString() + ',channel:'+1+',prgnum:'+_selectcount+'}',
			 dataType: "json",
			 success: function(data){
				var tablenode = $("#out_tree").fancytree("getTree").getNodeByKey("id1.0");
				tablenode.addChildren(data);
			 },    
			 error : function(err) { 
				alert(err);
			 }   
		});
		//获取制表后输出流表
		$.ajax({
			 type: "GET",
			 async:false,
			 url: "http://"+localip+":4000/do/programs/streamtable",
			 data: '{channel:'+1+',prgnum:'+_selectcount+'}',
			 dataType: "json",
			 success: function(data){
				var StreamData = [];	
				$.each(data, function(key, itemv) {
					var item = [itemv.NO,itemv.chn, itemv.newPid.toString(16),itemv.oldPid.toString(16),itemv.type];
					StreamData[StreamData.length] = item;					
				});
				//编辑数据流表
				if ( $.fn.dataTable.isDataTable( '#tbl_outtable' ) ) {									
					$('#tbl_outtable').dataTable().fnClearTable();
					$('#tbl_outtable').dataTable().fnAddData(StreamData);
				}else{
					//表结构右侧table
					$('#tbl_outtable').dataTable( {
						"data": StreamData,
						"order": [[ 0, "asc" ]],
						"paging":   false,
						"info":     false,
						"searching":   false,
						"scrollCollapse": true,
						"columns": [
							{ "title": "NO"},
							{ "title": "CH"},
							{ "title": "IN-PID", "width":"30%"},
							{ "title": "OUT-PID"},
							{ "title": "TYPE" }
						]
					});
				}				
			 },    
			 error : function(err) { 
				alert(err);
			 }   
		});
    });
	
	$( "#output-write" ).button({
      icons: {
        primary: "ui-icon-pencil"
      }
    }).click(function( event ) {
        event.preventDefault();
		$.ajax({
			 type: "GET",
			 async:false,
			 url: "http://"+localip+":4000/do/programs/writetable?channel=" + 1,
			 dataType: "json",
			 success: function(data){
				
			 },    
			 error : function(err) { 
				//alert(err);
			 }   
		});
		//alert('------------------!!!');
    });
	
	$( "#output-read2" ).button({
      icons: {
        primary: "ui-icon-comment"
      }
    }).click(function( event ) {
        event.preventDefault();
		$("#devlist2").fancytree("getTree").reload();
		$("#channel2").fancytree("getTree").reload();
		var node = $("#devlist2").fancytree("getTree").getNodeByKey("id1.0");
		var localip = window.location.href.substr(7, window.location.href.indexOf(':', 7) - 7);
		var _intChannelCntMax = 0;//设备输入通道数
		//获取输出通道信息
		$.ajax({
			 type: "GET",
			 async:false,
			 url: "http://"+localip+":4000/do/programs/getoutprg",
			// data: {ip:"192.168.1.134", inch:2},
			 dataType: "text",
			 success: function(data){
				_intChannelCntMax = Number(data);
			 },    
			 error : function(err) {    
				  // view("异常！");   
				var xxx = err;
				  alert("异常！====="+JSON.stringify(err));    
			 }   
		});
		if(_intChannelCntMax != 0 || _intChannelCntMax != ""){
			for(var i=1;i<_intChannelCntMax+1; i++){
				$.ajax({
					 type: "GET",
					 async:false,
					 url: "http://"+localip+":4000/do/programs/getprg?inch="+i,
					// data: {ip:"192.168.1.134", inch:2},
					 dataType: "json",
					 success: function(data){
						data = JSON.stringify(data).replace('\\','');
						var treeData1 = JSON.parse(data);				
						node.addChildren(treeData1);
					 },    
					 error : function(err) {    
						  // view("异常！");   
						var xxx = err;
						  alert("异常！====="+JSON.stringify(err));    
					 }   
				});
			}
		}	
    });
	
	$( "#output-table2" ).button({
      icons: {
        primary: "ui-icon-gear"
      }
    }).click(function( event ) {
        event.preventDefault();
		var nodes = $("#channel2").fancytree("getTree").getNodeByKey("id1.0").children;
		var inCh = 1, flag = 0; //通道号	
		var jsondata = new Array();
		var prgindex = new Array();
		var jsonstr;
		if(_selectcount == 0){
			alert("请选择节目!");
			return;
		}
		nodes.forEach(function(node) {
			flag = 0;
			prgindex = new Array();
			var chstr = "inCh" + inCh;
			if( node.hasChildren() ) {
				var prgnodes = node.children;				
				prgnodes.forEach(function(prgnode) {					
					prgindex[flag] = 'id' + flag + ':' + prgnode.data.index;
					flag++;
				});					
			}
			jsonstr = chstr+':{' + prgindex.toString() +'}';
			jsondata[inCh-1] = jsonstr;
			inCh++;
		});	
		//alert("=========arraystring======>>>"+ jsondata.toString());
		$.ajax({
			 type: "GET",
			 async:false,
			 url: "http://"+localip+":4000/do/programs/maketable",
			data: '{' + jsondata.toString() + ',channel:'+2+',prgnum:'+_selectcount+'}',
			 dataType: "json",
			 success: function(data){
				var tablenode = $("#out_tree2").fancytree("getTree").getNodeByKey("id1.0");
				tablenode.addChildren(data);
			 },    
			 error : function(err) {    
				  // view("异常！");   
				var xxx = err;
				//  alert("异常！====="+JSON.stringify(err));    
			 }   
		});
		//获取制表后输出流表
		$.ajax({
			 type: "GET",
			 async:false,
			 url: "http://"+localip+":4000/do/programs/streamtable",
			 data: '{channel:'+2+',prgnum:'+_selectcount+'}',
			 dataType: "json",
			 success: function(data){
				var StreamData = [];	
				$.each(data, function(key, itemv) {
					var item = [itemv.NO,itemv.chn, itemv.newPid.toString(16),itemv.oldPid.toString(16),itemv.type];
					StreamData[StreamData.length] = item;						
				});
				//编辑数据流表
				if ( $.fn.dataTable.isDataTable( '#tbl_outtable2' ) ) {									
					$('#tbl_outtable2').dataTable().fnClearTable();
					$('#tbl_outtable2').dataTable().fnAddData(StreamData);
				}else{
					//表结构右侧table
					$('#tbl_outtable2').dataTable( {
						"data": StreamData,
						"order": [[ 0, "asc" ]],
						"paging":   false,
						"info":     false,
						"searching":   false,
						"scrollCollapse": true,
						"columns": [
							{ "title": "NO"},
							{ "title": "CH"},
							{ "title": "IN-PID", "width":"30%"},
							{ "title": "OUT-PID"},
							{ "title": "TYPE" }
						]
					});
				}				
			 },    
			 error : function(err) { 
				alert(err);
			 }   
		});
    });
	
	$( "#output-write2" ).button({
      icons: {
        primary: "ui-icon-pencil"
      }
    }).click(function( event ) {
        event.preventDefault();
		alert('------------------!!!');
    });
	
	//输入通道树
	$("#devlist").fancytree({
		extensions: ["menu"],
		checkbox: true,
		selectMode: 3,
		minExpandLevel:2,
		source: channel_root,
		menu: {
			selector: "#inputprg_menu",
			position: {my: "center"},
			beforeOpen: function(event, data){
			    $.ui.fancytree.debug("Menu beforeOpen ", data.$menu, data.node);
				if(data.node.key == "id1.0"){
					$(".menu_delete").css("display", "none");
				}else if(data.node.key.length == 5){
					$(".menu_delete").css("display", "block");
				}			    
			},
			select: function(event, data){				
				switch(data.menuId){
					case '#expandall' :{
						var nodes = data.node.children;
						data.node.setExpanded(true);
						$.each(nodes, function(index,item){
							item.setExpanded(true);
							//item.render();
						});
						break;
					} case '#collasp': {
						data.node.setExpanded(false);
						break;
					} case '#delete': {
						while( data.node.hasChildren() ) {
							data.node.getFirstChild().remove();
						}
						break;
					}default: {
						alert("Menu select " + data.menuId + ", " + data.node);
						break;
					}			  
				}
			}
		},
		select: function(event, data) {
			var channeltree =  $("#channel").fancytree("getTree");
			//删除节目树节点
			channeltree.reload();
			var prgnode = channeltree.getNodeByKey("id1.0");			

			//添加至节目树	
			_selectcount = 0;
			var selNodes = data.tree.getSelectedNodes();			
			$.map(selNodes, function(node){				
				var tmpnode = channeltree.getNodeByKey(node.getParent().key);	
				var arr = node.key.match(/\./g);
				switch(arr.length){
					case 2:	//节目节点
						if(channeltree.getNodeByKey(node.key) == null){
							tmpnode.addNode(node.toDict());
							_selectcount++;
						}						
						break;
					case 3: //流节点
						if(channeltree.getNodeByKey(node.getParent().key) != null){				//	判断是否已存在节目节点
							channeltree.getNodeByKey(node.getParent().key).addNode(node.toDict());
						}else{
							channeltree.getNodeByKey(node.getParent().getParent().key).addNode(node.getParent().toDict());//添加节目节点
							channeltree.getNodeByKey(node.getParent().key).addNode(node.toDict());
							_selectcount++;
						}						
						break;
					case 4:
						if(channeltree.getNodeByKey(node.getParent().getParent().key) != null){					//	判断是否已存在节目节点
							if(channeltree.getNodeByKey(node.getParent().key) != null){	//判断是否已存在节目子节点
								channeltree.getNodeByKey(node.getParent().key).addNode(node.toDict());
							}else{
								channeltree.getNodeByKey(node.getParent().getParent().key).addNode(node.getParent().toDict()); 	//添加节目子节点
								channeltree.getNodeByKey(node.getParent().key).addNode(node.toDict());	//添加目标子节点
							}							
						}else{
							channeltree.getNodeByKey(node.key.substring(0,5)).addNode(node.getParent().getParent().toDict());//添加节目节点
							_selectcount++;
							channeltree.getNodeByKey(node.getParent().getParent().key).addNode(node.getParent().toDict()); 	//添加节目子节点
							channeltree.getNodeByKey(node.getParent().key).addNode(node.toDict());	//添加目标子节点
						}		
						//count++;
						break;
					default:
						
						break;
				}
				
			});
			prgnode.setTitle("节目: "+ _selectcount);
			prgnode.render();
			checkselectedprg();
		},
		click: function(event, data) {			
			if( $.ui.fancytree.getEventTargetType(event) === "title" ){
			  data.node.toggleSelected();
			}			
		}
	});
	//节目树
	$("#channel").fancytree({
		extensions: ["menu"],
		selectMode: 1,
		minExpandLevel:2,
		source: programData,
		menu: {
			selector: "#program_menu",
			position: {my: "center"},
			beforeOpen: function(event, data){
			    $.ui.fancytree.debug("Menu beforeOpen ", data.$menu, data.node);
				var arr = data.node.key.match(/\./g);
				if(arr.length == 1){
					$(".menu_add").css("display", "none");
					$(".menu_deleteall").css("display", "none");
					$(".menu_edit").css("display", "none");
					$(".menu_delete").css("display", "none");
					$(".menu_prgdeleteall").css("display", "block");
					$(".menu_prgitems").css("display", "block");
					$(".menu_re_prg").css("display", "block");
					$(".menu_re_pid").css("display", "block");
					$(".menu_pidtrans").css("display", "block");
				}else if(arr.length == 2){
					$(".menu_prgdeleteall").css("display", "none");
					$(".menu_prgitems").css("display", "none");
					$(".menu_re_prg").css("display", "none");
					$(".menu_re_pid").css("display", "none");
					$(".menu_pidtrans").css("display", "none");
					$(".menu_add").css("display", "block");
					$(".menu_deleteall").css("display", "block");
					$(".menu_edit").css("display", "block");
					$(".menu_delete").css("display", "block");
				}else{
					$(".menu_prgdeleteall").css("display", "none");
					$(".menu_prgitems").css("display", "none");
					$(".menu_re_prg").css("display", "none");
					$(".menu_re_pid").css("display", "none");
					$(".menu_pidtrans").css("display", "none");
					$(".menu_add").css("display", "block");
					$(".menu_deleteall").css("display", "block");
					$(".menu_edit").css("display", "none");
					$(".menu_delete").css("display", "none");
				}
			    
			},
			select: function(event, data){	
				_channel = 1;
				switch(data.menuId){
					case '#expandall' :{
						var nodes = data.node.children;
						data.node.setExpanded(true);
						$.each(nodes, function(index,item){
							item.setExpanded(true);
							//item.render();
						});
						break;
					} case '#collasp': {
						data.node.setExpanded(false);
						break;
					} case '#delete': {
						var nodekey = data.node.key;
						data.node.remove();
						$("#devlist").fancytree("getTree").getNodeByKey(nodekey).toggleSelected();
						break;
					} case '#edit': {
						//获取编辑节目信息
						$.ajax({
							type: "GET",
							async:false,
							url: "http://"+localip+":4000/do/programs/getprginfo",
							data:'{channel:' + _channel + ',pmtPid:' + data.node.data.pmtPid + '}',
							dataType: "json",
							success: function(data){
								_pmtPid = data.pmtPid;
								$('.prg_name').val(data.prgName);
								$('.prg_no').val(data.prgNum.toString());
								$('.prg_pid').val(data.pmtPid.toString(16));
								$('.prg_prc').val(data.chnId.toString());
								$('.prg_prc1').val(data.oldPcrPid.toString(16));
								$('.prg_prc2').val(data.newPcrPid.toString(16));
								$('#r_servicetype')[0].options[Number(data.serviceType) - 1].selected = true;
								dataSet.length = 0;	
								$.each(data.children, function(key, itemv) {
									var item = [itemv.index,itemv.inChn, itemv.streamtype,itemv.inpid,itemv.outpid];
									dataSet[dataSet.length] = item;
								});
								_tbleditcount = dataSet.length;
								//编辑节目对话框表
								if ( $.fn.dataTable.isDataTable( '#tbl_editprg' ) ) {									
									$('#tbl_editprg').dataTable().fnClearTable();
									$('#tbl_editprg').dataTable().fnAddData(dataSet);
								}else{
									_tbl_edit = $('#tbl_editprg').dataTable( {
										"data": dataSet,
										"order": [[ 0, "asc" ]],
										"paging":   false,
										"info":     false,
										"searching":   false,
										"scrollCollapse": true,										
										"fnRowCallback": function( nRow, aData, iDisplayIndex ) {
											switch(aData[2]){
												case 2:
													$('td:eq(2)', nRow).html( '<select id="r_streamtype'+iDisplayIndex+ '" name="r_streamtype'+iDisplayIndex +'">'
														+'<option value ="1">1-MPEG1 Video</option>'
														+'<option value ="2" selected="selected">2-MPEG2 Video</option>'
														+'<option value ="3">3-MPEG1 Audio</option>'
														+'<option value ="4">4-MPEG2 Audio</option>'												
													+'</select>' );
													break;
												case 4:
													$('td:eq(2)', nRow).html( '<select id="r_streamtype'+iDisplayIndex +'" name="r_streamtype'+iDisplayIndex +'">'
														+'<option value ="1">1-MPEG1 Video</option>'
														+'<option value ="2">2-MPEG2 Video</option>'
														+'<option value ="3">3-MPEG1 Audio</option>'
														+'<option value ="4" selected="selected">4-MPEG2 Audio</option>'												
													+'</select>' );
													break;
												default:
													$('td:eq(2)', nRow).html( '<select id="r_streamtype'+iDisplayIndex +'" name="r_streamtype'+iDisplayIndex +'">'
														+'<option value ="1" class="selected">1-MPEG1 Video</option>'
														+'<option value ="2">2-MPEG2 Video</option>'
														+'<option value ="3">3-MPEG1 Audio</option>'
														+'<option value ="4">4-MPEG2 Audio</option>'												
													+'</select>' );
											}
											$('td:eq(3)', nRow).html( '<input type="text" id="r_inpid'+iDisplayIndex+ '" name="r_inpid'+iDisplayIndex+ '" value="'+ aData[3] + '"></input>' );
											$('td:eq(4)', nRow).html( '<input type="text" id="r_outpid'+iDisplayIndex+ '" name="r_outpid'+iDisplayIndex+ '" value="'+ aData[4] + '"></input>' );
											
										},		
										"columns": [
											{ "title": "序号" },
											{ "title": "输入通道", "width": "70px"},
											{ "title": "流类型"},
											{ "title": "输入PID(Hex)"},
											{ "title": "输出PID(Hex)" }
										]
									});   
									$('#tbl_editprg tbody').on( 'click', 'tr', function () {
										if ( $(this).hasClass('selected') ) {
											$(this).removeClass('selected');
										}else {
											$('#tbl_editprg').DataTable().$('tr.selected').removeClass('selected');
											$(this).addClass('selected');
										}
									} );
								}							
							},    
							error : function(err) {    
								var xxx = err;
								alert("异常！====="+JSON.stringify(err));    
							}   
						});
						dialog_edit.dialog( "open" );
						_editnodekey = data.node.key;
						break;
					} case '#deleteall': {
						
						break;
					} case '#add': {
						dialog_desc.dialog( "open" );
						break;
					} case '#prgdeleteall': {
						var nodekey = data.node.key;
						node.removeChildren();
						$("#devlist").fancytree("getTree").getNodeByKey(nodekey).setSelected(false);
						break;
					} case '#itmes': {
						//获取输出通道设置信息
						$.ajax({
							 type: "GET",
							 async:false,
							 url: "http://"+localip+":4000/do/programs/getchanneloutinfo?channel=1",
							 dataType: "json",
							 success: function(data){
								//JSON.parse(data);
								$('.item_transid').val('0x' + data.streamId.toString(16));
								$('.item_netid').val('0x' + data.networkId.toString(16));
								
								$('.item_orignetid').val('0x' + data.oringal_networkid.toString(16));
								$('.item_out').val(data.outputRate);
								if(data.isAutoRaiseVersion){
									$('.autoinc_ver')[0].checked = true;
								}else{
									$('.autoinc_ver')[0].checked = false;
								}
								$('.item_version')[0].textContent = data.version;
								if(data.isAutoRankPAT){
									$('.pat_auto')[0].checked = true;
								}else{
									$('.pat_auto')[0].checked = false;
								}
								if(data.isNeedSend_cat){
									$('.sl_cat')[0].checked = true;
								}else{
									$('.sl_cat')[0].checked = false;
								}
								if(data.isNeedSend_nit){
									$('.sl_nit')[0].checked = true;
								}else{
									$('.sl_nit')[0].checked = false;
								}
								if(data.isNeedSend_pat){
									$('.sl_pat')[0].checked = true;
								}else{
									$('.sl_pat')[0].checked = false;
								}
								if(data.isNeedSend_pmt){
									$('.sl_pmt')[0].checked = true;
								}else{
									$('.sl_pmt')[0].checked = false;
								}
								if(data.isNeedSend_sdt){
									$('.sl_sdt')[0].checked = true;
								}else{
									$('.sl_sdt')[0].checked = false;
								}
								$('#tag_channel')[0].textContent = 1;
								
								dig_itmes.dialog( "open" );
							 },    
							 error : function(err) {    
								  // view("异常！");   
								var xxx = err;
								alert("异常！====="+JSON.stringify(err));    
							 }   
						});						
						break;
					} case '#re_prg': {
						
						break;
					} case '#re_pid': {
						
						break;
					} case '#pidtrans': {
						//获取pid透传表信息
						$.ajax({
							 type: "GET",
							 async:false,
							 url: "http://"+localip+":4000/do/programs/getpidtransinfo?channel=1",
							 dataType: "json",
							 success: function(data){
								var pidData = [];
								if(data.cnt != 0){
									$.each(data.children, function(key, itemv) {
										var item = [itemv.NO,itemv.ch, itemv.oldPid.toString(16),itemv.newPid.toString(16)];
										pidData[pidData.length] = item;					
									});
								}									
								
								//编辑数据流表
								if ( $.fn.dataTable.isDataTable( '#tbl_pid' ) ) {									
									$('#tbl_pid').dataTable().fnClearTable();
									if(pidData.length != 0){
										$('#tbl_pid').dataTable().fnAddData(pidData);
									}
								}else{
									//PID表
									_tbl_pid = $('#tbl_pid').dataTable( {
										"data": pidData,
										"order": [[ 0, "asc" ]],
										"paging":   false,
										"info":     false,
										"searching":   false,
										"scrollCollapse": true,
										"fnRowCallback": function( nRow, aData, iDisplayIndex ) {
											$('td:eq(1)', nRow).html( '<input type="text" pattern="([0-9]{1}$)" id="p_ch'+iDisplayIndex+ '" name="p_ch'+iDisplayIndex+ '" value="'+ aData[1] + '"></input>' );
											$('td:eq(2)', nRow).html( '<input type="text" pattern="(^0x[a-f0-9]{1,4}$)|(^0X[A-F0-9]{1,4}$)|(^[A-F0-9]{1,4}$)|(^[a-f0-9]{1,4}$)" id="p_oldpid'+iDisplayIndex+ '" name="p_oldpid'+iDisplayIndex+ '" value="'+ aData[2] + '"></input>' );
											$('td:eq(3)', nRow).html( '<input type="text" pattern="(^0x[a-f0-9]{1,4}$)|(^0X[A-F0-9]{1,4}$)|(^[A-F0-9]{1,4}$)|(^[a-f0-9]{1,4}$)" id="p_newpid'+iDisplayIndex+ '" name="p_newpid'+iDisplayIndex+ '" value="'+ aData[3] + '"></input>' );
										},		
										"columns": [
											{ "title": "序号", "width": "60px" },
											{ "title": "通道"},
											{ "title": "输入PID(Hex)"},
											{ "title": "输出PID(Hex)" }
										]
									});
									$('#tbl_pid tbody').on( 'click', 'tr', function () {
										if ( $(this).hasClass('selected') ) {
											$(this).removeClass('selected');
										}else {
											$('#tbl_pid').DataTable().$('tr.selected').removeClass('selected');
											$(this).addClass('selected');
										}
									} );
								}				
							 },    
							 error : function(err) {      
							 }   
						});
						dialog_pid.dialog( "open" );
						break;
					} default: {
						alert("Menu select " + data.menuId + ", " + data.node);
						break;
					}			  
				}
			}
		},
		click: function(event, data) {
			// We should not toggle, if target was "checkbox", because this
			// would result in double-toggle (i.e. no toggle)
			if( $.ui.fancytree.getEventTargetType(event) === "title" ){
			  data.node.toggleSelected();
			}
		}
	});	
	
	//表结构树
	$("#out_tree").fancytree({
		extensions: ["menu"],
		selectMode: 1,
		minExpandLevel:3,
		source: table_root,
		menu: {
			selector: "#table_menu",
			position: {my: "center"},
			select: function(event, data){				
				switch(data.menuId){
					case '#expandall' :{
						var nodes = data.node.children;
						data.node.setExpanded(true);
						$.each(nodes, function(index,item){
							item.setExpanded(true);
							//item.render();
						});
						break;
					} case '#collasp': {
						data.node.setExpanded(false);
						break;
					} case '#add': {
						dialog_NIT.dialog( "open" );
						break;
					} case '#delete': {
						//dialog.dialog( "open" );
						break;
					} case '#import': {
						
						break;
					} case '#export': {
						
						break;
					} default: {
						alert("Menu select " + data.menuId + ", " + data.node);
						break;
					}			  
				}
			}
		},
		click: function(event, data) {
			// We should not toggle, if target was "checkbox", because this
			// would result in double-toggle (i.e. no toggle)
			if( $.ui.fancytree.getEventTargetType(event) === "title" ){
			  data.node.toggleSelected();
			}
		}
	});
	
	//输入通道树
	$("#devlist2").fancytree({
		extensions: ["menu"],
		checkbox: true,
		selectMode: 3,
		minExpandLevel:2,
		source: channel_root,
		menu: {
			selector: "#inputprg_menu2",
			position: {my: "center"},
			beforeOpen: function(event, data){
				if(data.node.key == "id1.0"){
					$(".menu_delete").css("display", "none");
				}else if(data.node.key.length == 5){
					$(".menu_delete").css("display", "block");
				}			    
			},
			select: function(event, data){				
				switch(data.menuId){
					case '#expandall' :{
						var nodes = data.node.children;
						data.node.setExpanded(true);
						$.each(nodes, function(index,item){
							item.setExpanded(true);
							//item.render();
						});
						break;
					} case '#collasp': {
						data.node.setExpanded(false);
						break;
					} case '#delete': {
						while( data.node.hasChildren() ) {
							data.node.getFirstChild().remove();
						}
						break;
					}default: {
						alert("Menu select " + data.menuId + ", " + data.node);
						break;
					}			  
				}
			}
		},
		select: function(event, data) {
			var channeltree =  $("#channel2").fancytree("getTree");
			//删除节目树节点
			channeltree.reload();
			var prgnode = channeltree.getNodeByKey("id1.0");			

			//添加至节目树	
			var count = 0;//节目计数
			var selNodes = data.tree.getSelectedNodes();			
			$.map(selNodes, function(node){				
				var tmpnode = channeltree.getNodeByKey(node.getParent().key);	
				var arr = node.key.match(/\./g);
				switch(arr.length){
					case 2:	//节目节点
						if(channeltree.getNodeByKey(node.key) == null){
							tmpnode.addNode(node.toDict());
							_selectcount++;
						}						
						break;
					case 3: //流节点
						if(channeltree.getNodeByKey(node.getParent().key) != null){				//	判断是否已存在节目节点
							channeltree.getNodeByKey(node.getParent().key).addNode(node.toDict());
						}else{
							channeltree.getNodeByKey(node.getParent().getParent().key).addNode(node.getParent().toDict());//添加节目节点
							channeltree.getNodeByKey(node.getParent().key).addNode(node.toDict());
							_selectcount++;
						}						
						break;
					case 4:
						if(channeltree.getNodeByKey(node.getParent().getParent().key) != null){					//	判断是否已存在节目节点
							if(channeltree.getNodeByKey(node.getParent().key) != null){	//判断是否已存在节目子节点
								channeltree.getNodeByKey(node.getParent().key).addNode(node.toDict());
							}else{
								channeltree.getNodeByKey(node.getParent().getParent().key).addNode(node.getParent().toDict()); 	//添加节目子节点
								channeltree.getNodeByKey(node.getParent().key).addNode(node.toDict());	//添加目标子节点
							}							
						}else{
							channeltree.getNodeByKey(node.key.substring(0,5)).addNode(node.getParent().getParent().toDict());//添加节目节点
							_selectcount++;
							channeltree.getNodeByKey(node.getParent().getParent().key).addNode(node.getParent().toDict()); 	//添加节目子节点
							channeltree.getNodeByKey(node.getParent().key).addNode(node.toDict());	//添加目标子节点
						}		
						//count++;
						break;
					default:
						
						break;
				}
				
			});
			prgnode.setTitle("节目: "+ count);
			prgnode.render();
		},
		click: function(event, data) {			
			if( $.ui.fancytree.getEventTargetType(event) === "title" ){
			  data.node.toggleSelected();
			}			
		}
	});
	//节目树
	$("#channel2").fancytree({
		extensions: ["menu"],
		selectMode: 1,
		minExpandLevel:2,
		source: programData,
		menu: {
			selector: "#program_menu2",
			position: {my: "center"},
			beforeOpen: function(event, data){
			    $.ui.fancytree.debug("Menu beforeOpen ", data.$menu, data.node);
				var arr = data.node.key.match(/\./g);
				if(arr.length == 1){
					$(".menu_add").css("display", "none");
					$(".menu_deleteall").css("display", "none");
					$(".menu_edit").css("display", "none");
					$(".menu_delete").css("display", "none");
					$(".menu_prgdeleteall").css("display", "block");
					$(".menu_prgitems").css("display", "block");
					$(".menu_re_prg").css("display", "block");
					$(".menu_re_pid").css("display", "block");
					$(".menu_pidtrans").css("display", "block");
				}else if(arr.length == 2){
					$(".menu_prgdeleteall").css("display", "none");
					$(".menu_prgitems").css("display", "none");
					$(".menu_re_prg").css("display", "none");
					$(".menu_re_pid").css("display", "none");
					$(".menu_pidtrans").css("display", "none");
					$(".menu_add").css("display", "block");
					$(".menu_deleteall").css("display", "block");
					$(".menu_edit").css("display", "block");
					$(".menu_delete").css("display", "block");
				}else{
					$(".menu_prgdeleteall").css("display", "none");
					$(".menu_prgitems").css("display", "none");
					$(".menu_re_prg").css("display", "none");
					$(".menu_re_pid").css("display", "none");
					$(".menu_pidtrans").css("display", "none");
					$(".menu_add").css("display", "block");
					$(".menu_deleteall").css("display", "block");
					$(".menu_edit").css("display", "none");
					$(".menu_delete").css("display", "none");
				}
			    
			},
			select: function(event, data){				
				switch(data.menuId){
					case '#expandall' :{
						var nodes = data.node.children;
						data.node.setExpanded(true);
						$.each(nodes, function(index,item){
							item.setExpanded(true);
							//item.render();
						});
						break;
					} case '#collasp': {
						data.node.setExpanded(false);
						break;
					} case '#delete': {
						var nodekey = data.node.key;
						data.node.remove();
						$("#devlist2").fancytree("getTree").getNodeByKey(nodekey).toggleSelected();						
						break;
					} case '#edit': {
						//获取编辑节目信息
						$.ajax({
							type: "GET",
							async:false,
							url: "http://"+localip+":4000/do/programs/getprginfo",
							data:'{channel:'+2+ ',pmtPid:'+data.node.data.pmtPid+'}',
							dataType: "json",
							success: function(data){
								_pmtPid = data.pmtPid;
								$('.prg_name').val(data.prgName);
								$('.prg_no').val(data.prgNum.toString());
								$('.prg_pid').val(data.pmtPid.toString(16));
								$('.prg_prc').val(data.chnId.toString());
								$('.prg_prc1').val(data.oldPcrPid.toString(16));
								$('.prg_prc2').val(data.newPcrPid.toString(16));
								dataSet.length = 0;	
								$.each(data.children, function(key, itemv) {
									var item = [itemv.index,itemv.inChn, itemv.streamtype,itemv.inpid,itemv.outpid];
									dataSet[dataSet.length] = item;
								});
								_tbleditcount = dataSet.length;
								//编辑节目对话框表
								if ( $.fn.dataTable.isDataTable( '#tbl_editprg' ) ) {									
									$('#tbl_editprg').dataTable().fnClearTable();
									$('#tbl_editprg').dataTable().fnAddData(dataSet);
								}else{
									_tbl_edit = $('#tbl_editprg').dataTable( {
										"data": dataSet,
										"order": [[ 0, "asc" ]],
										"paging":   false,
										"info":     false,
										"searching":   false,
										"scrollCollapse": true,										
										"fnRowCallback": function( nRow, aData, iDisplayIndex ) {
											switch(aData[2]){
												case 2:
													$('td:eq(2)', nRow).html( '<select id="r_streamtype'+iDisplayIndex+ '" name="r_streamtype'+iDisplayIndex +'">'
														+'<option value ="1">1-MPEG1 Video</option>'
														+'<option value ="2" selected="selected">2-MPEG2 Video</option>'
														+'<option value ="3">3-MPEG1 Audio</option>'
														+'<option value ="4">4-MPEG2 Audio</option>'												
													+'</select>' );
													break;
												case 4:
													$('td:eq(2)', nRow).html( '<select id="r_streamtype'+iDisplayIndex +'" name="r_streamtype'+iDisplayIndex +'">'
														+'<option value ="1">1-MPEG1 Video</option>'
														+'<option value ="2">2-MPEG2 Video</option>'
														+'<option value ="3">3-MPEG1 Audio</option>'
														+'<option value ="4" selected="selected">4-MPEG2 Audio</option>'												
													+'</select>' );
													break;
												default:
													$('td:eq(2)', nRow).html( '<select id="r_streamtype'+iDisplayIndex +'" name="r_streamtype'+iDisplayIndex +'">'
														+'<option value ="1" class="selected">1-MPEG1 Video</option>'
														+'<option value ="2">2-MPEG2 Video</option>'
														+'<option value ="3">3-MPEG1 Audio</option>'
														+'<option value ="4">4-MPEG2 Audio</option>'												
													+'</select>' );
											}
											$('td:eq(3)', nRow).html( '<input type="text" id="r_inpid'+iDisplayIndex+ '" name="r_inpid'+iDisplayIndex+ '" value="'+ aData[3] + '"></input>' );
											$('td:eq(4)', nRow).html( '<input type="text" id="r_outpid'+iDisplayIndex+ '" name="r_outpid'+iDisplayIndex+ '" value="'+ aData[4] + '"></input>' );
											
										},		
										"columns": [
											{ "title": "序号" },
											{ "title": "输入通道", "width": "70px"},
											{ "title": "流类型"},
											{ "title": "输入PID(Hex)"},
											{ "title": "输出PID(Hex)" }
										]
									});   
									$('#tbl_editprg tbody').on( 'click', 'tr', function () {
										if ( $(this).hasClass('selected') ) {
											$(this).removeClass('selected');
										}else {
											$('#tbl_editprg').DataTable().$('tr.selected').removeClass('selected');
											$(this).addClass('selected');
										}
									} );
								}							
							},    
							error : function(err) {    
								var xxx = err;
								alert("异常！====="+JSON.stringify(err));    
							}   
						});
						dialog_edit.dialog( "open" );
						_editnodekey = data.node.key;
						break;
					} case '#deleteall': {
						
						break;
					} case '#add': {
						dialog_desc.dialog( "open" );
						break;
					} case '#prgdeleteall': {
						var nodekey = data.node.key;
						node.removeChildren();
						$("#devlist2").fancytree("getTree").getNodeByKey(nodekey).setSelected(false);
						break;
					} case '#itmes': {
						dig_itmes.dialog( "open" );
						break;
					} case '#re_prg': {
						
						break;
					} case '#re_pid': {
						
						break;
					} case '#pidtrans': {
						dialog_pid.dialog( "open" );
						break;
					} default: {
						alert("Menu select " + data.menuId + ", " + data.node);
						break;
					}			  
				}
			}
		},
		click: function(event, data) {
			// We should not toggle, if target was "checkbox", because this
			// would result in double-toggle (i.e. no toggle)
			if( $.ui.fancytree.getEventTargetType(event) === "title" ){
			  data.node.toggleSelected();
			}
		}
	});	
	
	//表结构树
	$("#out_tree2").fancytree({
		extensions: ["menu"],
		checkbox: true,
		selectMode: 2,
		minExpandLevel:3,
		source: table_root,
		menu: {
			selector: "#table_menu2",
			position: {my: "center"},
			select: function(event, data){				
				switch(data.menuId){
					case '#expandall' :{
						var nodes = data.node.children;
						data.node.setExpanded(true);
						$.each(nodes, function(index,item){
							item.setExpanded(true);
							//item.render();
						});
						break;
					} case '#collasp': {
						data.node.setExpanded(false);
						break;
					} case '#add': {
						dialog_NIT.dialog( "open" );
						break;
					} case '#delete': {
						//dialog.dialog( "open" );
						break;
					} case '#import': {
						
						break;
					} case '#export': {
						
						break;
					} default: {
						alert("Menu select " + data.menuId + ", " + data.node);
						break;
					}			  
				}
			}
		},
		click: function(event, data) {
			// We should not toggle, if target was "checkbox", because this
			// would result in double-toggle (i.e. no toggle)
			if( $.ui.fancytree.getEventTargetType(event) === "title" ){
			  data.node.toggleSelected();
			}
		}
	});

	//描述符右键菜单弹出对话框
	var dialog_desc = $( "#dialog-descriptor" ).dialog({
		autoOpen: false,
		width: 750,
		modal: true,
		buttons: {
			"确定": function() {
			  dialog_desc.dialog( "close" );
			},
			"取消": function() {
			  dialog_desc.dialog( "close" );
			}
		}
	});	
	
	//PID透传右键菜单弹出对话框
	var dialog_pid = $( "#dialog-pid" ).dialog({
		autoOpen: false,
		width: 650,
		modal: true,
		buttons: {
			"添加": function() {
				var index = Number(_tbl_pid[0].rows[_tbl_pid[0].rows.length - 1].firstChild.textContent);
				if( index || (index == 0)){
					index++;
				}else{
					index = 0;
				}
				$('#tbl_pid').DataTable().row.add( [
					index,
					1,
					0,
					0
				] ).draw();
			},
			"删除": function() {
				$('#tbl_pid').DataTable().row('.selected').remove().draw( false );
			},
			"确定": function() {
				var jsonstr;
				var data = $('#tbl_pid').DataTable().$('input').serialize();
				if(data == ""){
					jsonstr = '{"channel":' + _channel + ',"pidcnt":'+$('#tbl_pid').DataTable().$('tr').length + '}';
				}else{
					jsonstr = '{"channel":' + _channel + ',"pidcnt":'+$('#tbl_pid').DataTable().$('tr').length + ',"' + data.replace(/&/g, '","').replace(/=/g, '":"') +  '"}';
				}				
				//下发配置
				$.ajax({
					 type: "GET",
					 async:false,
					 url: "http://"+localip+":4000/do/programs/setpidtransinfo",
					 data: JSON.parse(jsonstr),
					 dataType: "json",
					 success: function(data){
						if(data.sts == 0){
							alert("数据输入错误!!");
							return;
						}
						dialog_pid.dialog( "close" );
					 },    
					 error : function(err) {    
					 }   
				});
				
			},
			"取消": function() {
				dialog_pid.dialog( "close" );
			}
		}
	});
	
	//添加段右键菜单弹出对话框
	var dialog_NIT = $( "#dialog-NIT" ).dialog({
		autoOpen: false,
		height: 400,
		width: 450,
		modal: true,
		buttons: {
			"确定": function() {
			  dialog_NIT.dialog( "close" );
			},
			"取消": function() {
			  dialog_NIT.dialog( "close" );
			}
		}
	});
	
	//编辑节目右键菜单弹出对话框
	var dialog_edit = $( "#dialog-form" ).dialog({
		autoOpen: false,
		height: 500,
		width: 820,
		modal: true,
		buttons: {
			"添加":function() {
				$('#tbl_editprg').DataTable().row.add( [
					Number(_tbl_edit[0].rows[_tbl_edit[0].rows.length - 1].firstChild.textContent) + 1,
					Number($('.prg_prc').val()),
					'MPEG2 Audio',
					200,
					200
				] ).draw();
			} ,
			"删除":function() {
				$('#tbl_editprg').DataTable().row('.selected').remove().draw( false );
			},
			"确定": function() {
				var strindex = "";
				for(var i=1;i<_tbl_edit[0].rows.length;i++){
					strindex += ',"index'+(i-1)+'":' + _tbl_edit[0].rows[i].firstChild.textContent;
				}
				var data = $('#tbl_editprg').DataTable().$('input, select').serialize();
				var jsonstr = '{"channel":' + 1 + ',"prgname":"' + $('.prg_name').val() + ',"prgNum":' + Number($('.prg_no').val()) + ',"orgiralpmtpid":' +  _pmtPid + ',"pmtpid":' +  parseInt($('.prg_pid').val(),16) + ',"oldpcrpid":' + parseInt($('.prg_prc1').val(),16) + ',"newpcrpid":' + parseInt($('.prg_prc2').val(),16)+ ',"streamcnt":'+$('#tbl_editprg').DataTable().$('tr').length+',"' +data.replace(/&/g, ',"').replace(/=/g, '":') + strindex + '}';
				//下发配置
				$.ajax({
					 type: "GET",
					 async:false,
					 url: "http://"+localip+":4000/do/programs/setprginfo",
					 data: JSON.parse(jsonstr),
					 dataType: "json",
					 success: function(data){
						if(data.sts == 1){
							var node = $("#channel").fancytree("getTree").getNodeByKey(_editnodekey);
							node.data.prgNum = Number($('.prg_no').val());
							node.title = "节目"+node.data.prgNum+"(0X"+ node.data.prgNum.toString(16)+"):PID(0X"+$('.prg_pid').val() +") PCR_PID(0X"+$('.prg_prc2').val() +") - "+$('.prg_name').val() ;
							node.renderTitle();
						}
					 },    
					 error : function(err) {    
					 }   
				});
				dialog_edit.dialog( "close" );
			},
			"取消": function() {
			  dialog_edit.dialog( "close" );
			}
		}
	});
	//选项右键菜单弹出对话框
	var dig_itmes = $( "#dialog-items" ).dialog({
		autoOpen: false,
		width: 470,
		modal: true,
		buttons: {							
			"确定": function() {
				var jsonstr = '{"channel":' +$('#tag_channel')[0].textContent + ',"networkId":'+ Number($('.item_netid').val()) + ',"streamId":' + Number($('.item_transid').val()) + ',"oringal_networkid":' + Number($('.item_orignetid').val()) + ',"outputRate":' + Number($('.item_out').val())+ ',"isAutoRaiseVersion":' + ($('.autoinc_ver')[0].checked == true?1:0) + ',"version":' + Number($('.item_version')[0].textContent) + ',"isAutoRankPAT":' + ($('.pat_auto')[0].checked == true?1:0) + ',"isNeedSend_cat":' + ($('.sl_cat')[0].checked == true?1:0) + ',"isNeedSend_nit":' + ($('.sl_nit')[0].checked == true?1:0) + ',"isNeedSend_pat":' + ($('.sl_pat')[0].checked == true?1:0) + ',"isNeedSend_pmt":' + ($('.sl_pmt')[0].checked == true?1:0) + ',"isNeedSend_sdt":' + ($('.sl_sdt')[0].checked == true?1:0) + '}';
				//下发配置
				$.ajax({
					 type: "GET",
					 async:false,
					 url: "http://"+localip+":4000/do/programs/setchanneloutinfo",
					 data: JSON.parse(jsonstr),
					 dataType: "json",
					 success: function(data){
						if(data.sts == 1){
							
						}
					 },    
					 error : function(err) {    
					 }   
				});
				dig_itmes.dialog( "close" );
			},
			"取消": function() {
				dig_itmes.dialog( "close" );
			},
			"应用":function() {
				
				dig_itmes.dialog( "close" );
			}
		}
	});

    var progressbar = $( "#progressbar" ),
        progressLabel = $( ".progress-label" );
    var progress_dialog = $( "#progress-dialog" ).dialog({
        autoOpen: false,
        closeOnEscape: false,
        resizable: false
    });
    progressbar.progressbar({
        value: true,
        change: function() {
            progressLabel.text( "正在搜索第 " + $('.progress-label')[0].dataset.ch + " 通道" );
        },
        complete: function() {
            $('.progress-label')[0].dataset.ch = 1;
            progressLabel.text( "Complete!" );
            progress_dialog.dialog( "close" );
            readprgs();
        }
    });
}


function dev_srcset() {
	$('.main-content').empty();
	$('.main-content').append(
		'<div class="src_content">'
			+'<fieldset>'
				+'<legend>源设置</legend>'
				+'<table>'
					+'<tr>'
						+'<td><label>网络接口模式</label></td>'
						+'<td><label style="margin-left:30px" label>10M</label></td>'
					+'</tr>'
					+'<tr>'
						+'<td><label>源IP地址</label></td>'
						+'<td><input type="text" value=""></input><label>&nbsp &nbsp(eg:192.168.1.103)</label></td>'
					+'</tr>'
					+'<tr>'
						+'<td><label>源端口号</label></td>'
						+'<td><input type="text" value=""></input><label>&nbsp &nbsp(0~65535)</label></td>'
					+'</tr>'
					+'<tr>'
						+'<td><label>源MAC</label></td>'
						+'<td><input type="text" value=""></input><label>&nbsp &nbsp(值为16进制  eg:00:11:22:33:44:55)</label></td>'
					+'</tr>'
				+'</table>'
				+'<div class="src_btn">'
					+'<button id="src-read" class="STRING_READ">读取</button>'
					+'<button id="src-write" class="STRING_WRITE">应用</button>'
				+'</div>'
			+'</fieldset>'
		+'</div>'
	);
	
	
	$( "#src-read" ).button({
      icons: {
        primary: "ui-icon-comment"
      }
    }).click(function( event ) {
        event.preventDefault();
		alert('------------------!!!');
    });
	
	$( "#src-write" ).button({
      icons: {
        primary: "ui-icon-pencil"
      }
    }).click(function( event ) {
        event.preventDefault();
		alert('------------------!!!');
    });

}

function dev_outset() {
	$('.main-content').empty();
	$('.main-content').append(
		'<div class="src_content">'
			+'<fieldset>'
				+'<legend>输出设置  [CH-1]</legend>'
				+'<table>'
					+'<tr>'
						+'<td><label>输出方式</label></td>'
						+'<td><label style="margin-left:30px" label>UDP</label></td>'
					+'</tr>'
					+'<tr>'
						+'<td><label>目的IP地址</label></td>'
						+'<td><input type="text" value=""></input><label>&nbsp &nbsp(eg:192.168.1.103)</label></td>'
					+'</tr>'
					+'<tr>'
						+'<td><label>目的端口号</label></td>'
						+'<td><input type="text" value=""></input><label>&nbsp &nbsp(0~65535)</label></td>'
					+'</tr>'
					+'<tr>'
						+'<td><label>目的MAC</label></td>'
						+'<td><input type="text" value=""></input><label>&nbsp &nbsp(值为16进制  eg:00:11:22:33:44:55)</label></td>'
					+'</tr>'
				+'</table>'
				+'<div class="src_btn">'
					+'<button id="out-read" class="STRING_READ">读取</button>'
					+'<button id="out-write" class="STRING_WRITE">应用</button>'
				+'</div>'
			+'</fieldset>'
		+'</div>'
	);
	
	
	$( "#out-read" ).button({
      icons: {
        primary: "ui-icon-comment"
      }
    }).click(function( event ) {
        event.preventDefault();
		alert('------------------!!!');
    });
	
	$( "#out-write" ).button({
      icons: {
        primary: "ui-icon-pencil"
      }
    }).click(function( event ) {
        event.preventDefault();
		alert('------------------!!!');
    });
}
