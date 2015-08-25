var globalObj = {
    _selectcount: 0,    //节目计数
    _selectcount2: 0,
    _intChannelCntMax: 0,//设备输入通道数
    _tbleditcount: 0,   //edit表stream计数
    _index: 0,          //编辑节目index
    _editnodekey: "",   //编辑节目节点key
    _channel: 1,        //操作中的通道
    _tbl_edit: null,    //节目编辑表格
    _tbl_nitc: null,    //NITC编辑表格
    _tbl_pid: null,
    _prgoptflag: 0,     //节目操场判断: 0--编辑, 1--添加
    _isuserprg: 1,     //节目类型判断: 0--userprg, 1--devprg
	_nv: localStorage.getItem("language"),
    localip: window.location.href.substr(7, window.location.href.indexOf(':', 7) - 7),
    timerID: undefined
};

var channel_root = [
	{"title": globalObj._nv == "zh-CN"?"输入通道":"INPUT-CH", folder: true, key: "id1.0", expanded: true, "expanded": true, "icon": "img/book.ico"}	  
];

var table_root = [
	{"title": globalObj._nv == "zh-CN"?"表":"TABLE", folder: true, key: "id1.0", expanded: true, "expanded": true, "icon": "img/book.ico"}	  
];

var programData = [
	{title: globalObj._nv == "zh-CN"?"节目":"Programes", folder: true, key: "id1.0", expanded: true, "icon": "img/book.ico",
	  children: [
		{title: globalObj._nv == "zh-CN"?"通道1 (ASI-1)":"CHANNEL1 (ASI-1)", folder: true, chnid:1, key: "id1.1", expanded: true, "icon": "img/channel_out.ico"},
		{title: globalObj._nv == "zh-CN"?"通道2 (ASI-2)":"CHANNEL2 (ASI-2)", folder: true, chnid:2, key: "id1.2", expanded: true, "icon": "img/channel_out.ico"},
		{title: globalObj._nv == "zh-CN"?"通道3 (ASI-3)":"CHANNEL3 (ASI-3)", folder: true, chnid:3, key: "id1.3", expanded: true, "icon": "img/channel_out.ico"},
		{title: globalObj._nv == "zh-CN"?"通道4 (ASI-4)":"CHANNEL4 (ASI-4)", folder: true, chnid:4, key: "id1.4", expanded: true, "icon": "img/channel_out.ico"},
		{title: globalObj._nv == "zh-CN"?"通道5 (ASI-5)":"CHANNEL5 (ASI-5)", folder: true, chnid:5, key: "id1.5", expanded: true, "icon": "img/channel_out.ico"},
		{title: globalObj._nv == "zh-CN"?"通道6 (ASI-6)":"CHANNEL6 (ASI-6)", folder: true, chnid:6, key: "id1.6", expanded: true, "icon": "img/channel_out.ico"},
		{title: globalObj._nv == "zh-CN"?"通道7 (ASI-7)":"CHANNEL7 (ASI-7)", folder: true, chnid:7, key: "id1.7", expanded: true, "icon": "img/channel_out.ico"},
		{title: globalObj._nv == "zh-CN"?"通道8 (ASI-8)":"CHANNEL8 (ASI-8)", folder: true, chnid:8, key: "id1.8", expanded: true, "icon": "img/channel_out.ico"},
		{title: globalObj._nv == "zh-CN"?"用户自定义":"CUSTOM", folder: true, chnid:9, key: "id1.9", expanded: true, "icon": "img/book.ico"}
	  ]
	}
];

var dataSet = [
];

var dataSet1 = [
];

function createTable(){
    var outtree;
    var snt;
    var channeltree;
    if(globalObj._channel == 1){
        outtree = $("#out_tree").fancytree("getTree");
        snt = globalObj._selectcount;
        channeltree =  $("#channel").fancytree("getTree");
    }else if(globalObj._channel == 2){
        outtree = $("#out_tree2").fancytree("getTree");
        snt = globalObj._selectcount2;
        channeltree =  $("#channel2").fancytree("getTree");
    }
    $.ajax({
        type: "GET",
        async:false,
        url: "http://"+globalObj.localip+":4000/do/programs/maketable",
        data: '{channel:'+ globalObj._channel +'}',
        dataType: "json",
        success: function(data){
            if(data.sts == 5){
                alert(globalObj._nv == "zh-CN"?"该用户权限不足.":"Permission Denied!");
                return;
            }
            if(data.sts == 9){
                window.location = "/login.esp";
                return;
            }
            if(data[0].sts == -1001){
				alert(globalObj._nv == "zh-CN"?"在节目表和透传表有相同PID,请删除其中一个后重试!.":"Reduplicated Pid, Please delete one and retry!");
            }
            outtree.reload();
            var tablenode = outtree.getNodeByKey("id1.0");
            tablenode.addChildren(data);
            //获取制表后输出流表
            $.ajax({
                type: "GET",
                async:false,
                url: "http://"+globalObj.localip+":4000/do/programs/streamtable",
                data: '{channel:'+globalObj._channel+',prgnum:'+snt+'}',
                dataType: "json",
                success: function(data){
                    if(data.sts == 5){
                        if(globalObj._channel == 1){
                            if ( $.fn.dataTable.isDataTable( '#tbl_outtable' ) ) {
                                $('#tbl_outtable').dataTable().fnClearTable();
                            }
                        }else{
                            if ( $.fn.dataTable.isDataTable( '#tbl_outtable2' ) ) {
                                $('#tbl_outtable2').dataTable().fnClearTable();
                            }
                        }
                        return;
                    }
                    var StreamData = [];
                    $.each(data, function(key, itemv) {
                        var item = [itemv.NO,itemv.chn, itemv.newPid.toString(16),itemv.oldPid.toString(16),itemv.type];
                        StreamData[StreamData.length] = item;
                    });
                    //编辑数据流表
                    if(globalObj._channel == 1){
                        if ( $.fn.dataTable.isDataTable( '#tbl_outtable' ) ) {
                            $('#tbl_outtable').dataTable().fnClearTable();
                            $('#tbl_outtable').dataTable().fnAddData(StreamData);
                        }else{
                            //表结构右侧table
                            var tout = $('#tbl_outtable').dataTable( {
                                "data": StreamData,
                                "order": [[ 0, "asc" ]],
                                "paging":   false,
                                "info":     false,
                                "searching":   false,
                                "scrollY": 390,
                                "bAutoWidth": false,
                                "columns": [
                                    { "title": "NO"},
                                    { "title": "CH"},
                                    { "title": "IN-PID"},
                                    { "title": "OUT-PID"},
                                    { "title": "TYPE" }
                                ]
                            });
                            tout.fnDraw();
                        }
                    }else{
                        if ( $.fn.dataTable.isDataTable( '#tbl_outtable2' ) ) {
                            $('#tbl_outtable2').dataTable().fnClearTable();
                            $('#tbl_outtable2').dataTable().fnAddData(StreamData);
                        }else{
                            //表结构右侧table
                            var tout = $('#tbl_outtable2').dataTable( {
                                "data": StreamData,
                                "order": [[ 0, "asc" ]],
                                "paging":   false,
                                "info":     false,
                                "searching":   false,
                                "scrollY": 390,
                                "bAutoWidth": false,
                                "columns": [
                                    { "title": "NO"},
                                    { "title": "CH"},
                                    { "title": "IN-PID"},
                                    { "title": "OUT-PID"},
                                    { "title": "TYPE" }
                                ]
                            });
                            tout.fnDraw();
                        }
                    }
                },
                error : function(err) {
                    alert(err);
                }
            });
            readoutprgs(channeltree,0);
        },
        error : function(err) {
            alert(err);
        }
    });
}

function addstrc(){
    $('.nitc_id').val("0");
    $('.nitc_netid').val("0");
    $('.nitc_hz').val("00000");
    $('.nitc_ksm').val("00000");
    $('#nitc_fecout')[0].options[0].selected = true;
    $('#nitc_fecin')[0].options[0].selected = true;
    $('#nitc_qam')[0].options[0].selected = true;
    if ( $.fn.dataTable.isDataTable( '#tbl_nitc' ) ) {
        $('#tbl_nitc').dataTable().fnClearTable();
    }else {
        dataSet.length = 0;
        globalObj._tbl_nitc = $('#tbl_nitc').dataTable({
            "data": dataSet,
            "order": [[0, "asc"]],
            "paging": false,
            "info": false,
            "searching": false,
            "fnRowCallback": function (nRow, aData, iDisplayIndex) {
                $('td:eq(0)', nRow).html( '<input type="number" pattern="(^[0-9]{1,4}$)" id="nitc_id'+iDisplayIndex+ '" name="nitc_id'+iDisplayIndex+ '" value="'+ aData[0] + '"></input>' );
                $('td:eq(1)', nRow).html(
                    '<select id="nitc_type'+iDisplayIndex+ '" name="nitc_type'+iDisplayIndex +'">'
                    +'<option value ="0" selected="selected">reserve</option>'
                    +'<option value ="1">digital television service</option>'
                    +'<option value ="2">digital radio sound service</option>'
                    +'<option value ="3">Teletext service</option>'
                    +'<option value ="4">NVOD reference service</option>'
                    +'<option value ="5">NVOD time-shifted service</option>'
                    +'<option value ="6">mosaic service</option>'
                    +'<option value ="7">PAL coded signal</option>'
                    +'<option value ="8">SECAM coded siganl</option>'
                    +'<option value ="9">D/D2-MAC</option>'
                    +'<option value ="10">FM Radio</option>'
                    +'<option value ="11">NTSC coded signal</option>'
                    +'<option value ="12">data broadcast service</option>'
                    +'<option value ="13">reserve for Common interface Usage</option>'
                    +'<option value ="14">RCS Map (see EN 301 790[34])</option>'
                    +'<option value ="15">RCS FLS (see EN 301 790[34])</option>'
                    +'<option value ="16">DVB MHP service</option>'
                    +'</select>'
                );
            },
            "columns": [
                {"title": globalObj._nv == "zh-CN"?"业务ID":"ID"},
                {"title": globalObj._nv == "zh-CN"?"类型":"TYPE"}
            ]
        });
        $('#tbl_nitc tbody').on('click', 'tr', function () {
            if ($(this).hasClass('selected')) {
                $(this).removeClass('selected');
            } else {
                $('#tbl_nitc').DataTable().$('tr.selected').removeClass('selected');
                $(this).addClass('selected');
            }
        });
    }
}

function addstrs(){
    $('.nits_id').val("0");
    $('.nits_netid').val("0");
    $('.nits_hz').val("00000");
    $('.nits_ksm').val("00000");
    $('.nits_path').val("0");
    $('#nits_pol')[0].options[0].selected = true;
    $('#nits_fecin')[0].options[0].selected = true;
    $('#nits_qpsk')[0].options[0].selected = true;
    $('#nits_tip')[0].options[0].selected = true;
    if ( $.fn.dataTable.isDataTable( '#tbl_nits' ) ) {
        $('#tbl_nits').dataTable().fnClearTable();
    }else {
        dataSet.length = 0;
        globalObj._tbl_nitc = $('#tbl_nits').dataTable({
            "data": dataSet,
            "order": [[0, "asc"]],
            "paging": false,
            "info": false,
            "searching": false,
            "fnRowCallback": function (nRow, aData, iDisplayIndex) {
                $('td:eq(0)', nRow).html( '<input type="text" pattern="((^[0-9]{1,4}$)" id="nits_id'+iDisplayIndex+ '" name="nits_id'+iDisplayIndex+ '" value="'+ aData[0] + '"></input>' );
                $('td:eq(1)', nRow).html(
                    '<select id="nits_type'+iDisplayIndex+ '" name="nits_type'+iDisplayIndex +'">'
                    +'<option value ="0" selected="selected">reserve</option>'
                    +'<option value ="1">digital television service</option>'
                    +'<option value ="2">digital radio sound service</option>'
                    +'<option value ="3">Teletext service</option>'
                    +'<option value ="4">NVOD reference service</option>'
                    +'<option value ="5">NVOD time-shifted service</option>'
                    +'<option value ="6">mosaic service</option>'
                    +'<option value ="7">PAL coded signal</option>'
                    +'<option value ="8">SECAM coded siganl</option>'
                    +'<option value ="9">D/D2-MAC</option>'
                    +'<option value ="10">FM Radio</option>'
                    +'<option value ="11">NTSC coded signal</option>'
                    +'<option value ="12">data broadcast service</option>'
                    +'<option value ="13">reserve for Common interface Usage</option>'
                    +'<option value ="14">RCS Map (see EN 301 790[34])</option>'
                    +'<option value ="15">RCS FLS (see EN 301 790[34])</option>'
                    +'<option value ="16">DVB MHP service</option>'
                    +'</select>'
                );
            },
            "columns": [
                {"title": globalObj._nv == "zh-CN"?"业务ID":"ID"},
                {"title": globalObj._nv == "zh-CN"?"类型":"TYPE"}
            ]
        });
        $('#tbl_nits tbody').on('click', 'tr', function () {
            if ($(this).hasClass('selected')) {
                $(this).removeClass('selected');
            } else {
                $('#tbl_nits').DataTable().$('tr.selected').removeClass('selected');
                $(this).addClass('selected');
            }
        });
    }
}

function addstrt(){
    $('.nitt_id').val("0");
    $('.nitt_netid').val("0");
    $('.nitt_hz').val("00000");
    $('#nitt_bandwidth')[0].options[0].selected = true;
    $('#nitt_constell')[0].options[0].selected = true;
    $('#nitt_transmissionMode')[0].options[0].selected = true;
    $('#nitt_hierarchy')[0].options[0].selected = true;
    $('#nitt_hp')[0].options[0].selected = true;
    $('#nitt_lp')[0].options[0].selected = true;
    $('#nitt_gi')[0].options[0].selected = true;
    if ( $.fn.dataTable.isDataTable( '#tbl_nitt' ) ) {
        $('#tbl_nitt').dataTable().fnClearTable();
    }else {
        dataSet.length = 0;
        globalObj._tbl_nitc = $('#tbl_nitt').dataTable({
            "data": dataSet,
            "order": [[0, "asc"]],
            "paging": false,
            "info": false,
            "searching": false,
            "fnRowCallback": function (nRow, aData, iDisplayIndex) {
                $('td:eq(0)', nRow).html( '<input type="text" pattern="((^[0-9]{1,4}$)" id="nitt_id'+iDisplayIndex+ '" name="nitt_id'+iDisplayIndex+ '" value="'+ aData[0] + '"></input>' );
                $('td:eq(1)', nRow).html(
                    '<select id="nitt_type'+iDisplayIndex+ '" name="nitt_type'+iDisplayIndex +'">'
                    +'<option value ="0" selected="selected">reserve</option>'
                    +'<option value ="1">digital television service</option>'
                    +'<option value ="2">digital radio sound service</option>'
                    +'<option value ="3">Teletext service</option>'
                    +'<option value ="4">NVOD reference service</option>'
                    +'<option value ="5">NVOD time-shifted service</option>'
                    +'<option value ="6">mosaic service</option>'
                    +'<option value ="7">PAL coded signal</option>'
                    +'<option value ="8">SECAM coded siganl</option>'
                    +'<option value ="9">D/D2-MAC</option>'
                    +'<option value ="10">FM Radio</option>'
                    +'<option value ="11">NTSC coded signal</option>'
                    +'<option value ="12">data broadcast service</option>'
                    +'<option value ="13">reserve for Common interface Usage</option>'
                    +'<option value ="14">RCS Map (see EN 301 790[34])</option>'
                    +'<option value ="15">RCS FLS (see EN 301 790[34])</option>'
                    +'<option value ="16">DVB MHP service</option>'
                    +'</select>'
                );
            },
            "columns": [
                {"title": globalObj._nv == "zh-CN"?"业务ID":"ID"},
                {"title": globalObj._nv == "zh-CN"?"类型":"TYPE"}
            ]
        });
        $('#tbl_nitt tbody').on('click', 'tr', function () {
            if ($(this).hasClass('selected')) {
                $(this).removeClass('selected');
            } else {
                $('#tbl_nitt').DataTable().$('tr.selected').removeClass('selected');
                $(this).addClass('selected');
            }
        });
    }
}

function editnitc(data, res){
    $('.nitc_id').val(res.streamid);
    $('.nitc_netid').val(res.netid);
    $('.nitc_hz').val(res.hz);
    $('.nitc_ksm').val(res.ksm);
    $('#nitc_fecout')[0].options[Number(res.fecout)].selected = true;
    $('#nitc_fecin')[0].options[Number(res.fecin)].selected = true;
    $('#nitc_qam')[0].options[Number(res.qam)].selected = true;
    dataSet.length = 0;
    if(res.children){
        $.each(res.children, function(key, itemv) {
            var item = [itemv.serid,itemv.sertype];
            dataSet[dataSet.length] = item;
        });
    }
    //编辑节目对话框表
    if ( $.fn.dataTable.isDataTable( '#tbl_nitc' ) ) {
        $('#tbl_nitc').dataTable().fnClearTable();
        if(dataSet.length > 0)
            $('#tbl_nitc').dataTable().fnAddData(dataSet);
    }else{
        globalObj._tbl_nitc = $('#tbl_nitc').dataTable({
            "data": dataSet,
            "order": [[0, "asc"]],
            "paging": false,
            "info": false,
            "searching": false,
            "fnRowCallback": function (nRow, aData, iDisplayIndex) {
                $('td:eq(0)', nRow).html( '<input type="number" pattern="(^[0-9]{1,4}$)" id="nitc_id'+iDisplayIndex+ '" name="nitc_id'+iDisplayIndex+ '" value="'+ aData[0] + '"></input>' );
                $('td:eq(1)', nRow).html(
                    '<select id="nitc_type'+iDisplayIndex+ '" name="nitc_type'+iDisplayIndex +'">'
                    +'<option value ="0" selected="selected">reserve</option>'
                    +'<option value ="1">digital television service</option>'
                    +'<option value ="2">digital radio sound service</option>'
                    +'<option value ="3">Teletext service</option>'
                    +'<option value ="4">NVOD reference service</option>'
                    +'<option value ="5">NVOD time-shifted service</option>'
                    +'<option value ="6">mosaic service</option>'
                    +'<option value ="7">PAL coded signal</option>'
                    +'<option value ="8">SECAM coded siganl</option>'
                    +'<option value ="9">D/D2-MAC</option>'
                    +'<option value ="10">FM Radio</option>'
                    +'<option value ="11">NTSC coded signal</option>'
                    +'<option value ="12">data broadcast service</option>'
                    +'<option value ="13">reserve for Common interface Usage</option>'
                    +'<option value ="14">RCS Map (see EN 301 790[34])</option>'
                    +'<option value ="15">RCS FLS (see EN 301 790[34])</option>'
                    +'<option value ="16">DVB MHP service</option>'
                    +'</select>'
                );
            },
            "columns": [
                {"title": globalObj._nv == "zh-CN"?"业务ID":"ID"},
                {"title": globalObj._nv == "zh-CN"?"类型":"TYPE"}
            ]
        });
        $('#tbl_nitc tbody').on('click', 'tr', function () {
            if ($(this).hasClass('selected')) {
                $(this).removeClass('selected');
            } else {
                $('#tbl_nitc').DataTable().$('tr.selected').removeClass('selected');
                $(this).addClass('selected');
            }
        });
    }
    $.each(dataSet, function(key, itemv) {
        $('#nitc_type' + key)[0].options[Number(itemv[1])].selected = true;
    });
}

function editnits(data, res){
    $('.nits_id').val(res.streamid);
    $('.nits_netid').val(res.netid);
    $('.nits_hz').val(res.hz);
    $('.nits_ksm').val(res.ksm);
    $('.nits_path').val(res.position);
    $('#nits_pol')[0].options[Number(res.pol)].selected = true;
    $('#nits_fecin')[0].options[Number(res.fecin)].selected = true;
    $('#nits_qpsk')[0].options[Number(res.qpsk)].selected = true;
    $('#nits_tip')[0].options[Number(res.tip)].selected = true;
    dataSet.length = 0;
    if(res.children){
        $.each(res.children, function(key, itemv) {
            var item = [itemv.serid,itemv.sertype];
            dataSet[dataSet.length] = item;
        });
    }
    //编辑节目对话框表
    if ( $.fn.dataTable.isDataTable( '#tbl_nits' ) ) {
        $('#tbl_nits').dataTable().fnClearTable();
        if(dataSet.length > 0)
            $('#tbl_nits').dataTable().fnAddData(dataSet);
    }else{
        globalObj._tbl_nitc = $('#tbl_nits').dataTable({
            "data": dataSet,
            "order": [[0, "asc"]],
            "paging": false,
            "info": false,
            "searching": false,
            "fnRowCallback": function (nRow, aData, iDisplayIndex) {
                $('td:eq(0)', nRow).html( '<input type="text" pattern="(^[0-9]{1,4}$)" id="nits_id'+iDisplayIndex+ '" name="nits_id'+iDisplayIndex+ '" value="'+ aData[0] + '"></input>' );
                $('td:eq(1)', nRow).html(
                    '<select id="nits_type'+iDisplayIndex+ '" name="nits_type'+iDisplayIndex +'">'
                    +'<option value ="0" selected="selected">reserve</option>'
                    +'<option value ="1">digital television service</option>'
                    +'<option value ="2">digital radio sound service</option>'
                    +'<option value ="3">Teletext service</option>'
                    +'<option value ="4">NVOD reference service</option>'
                    +'<option value ="5">NVOD time-shifted service</option>'
                    +'<option value ="6">mosaic service</option>'
                    +'<option value ="7">PAL coded signal</option>'
                    +'<option value ="8">SECAM coded siganl</option>'
                    +'<option value ="9">D/D2-MAC</option>'
                    +'<option value ="10">FM Radio</option>'
                    +'<option value ="11">NTSC coded signal</option>'
                    +'<option value ="12">data broadcast service</option>'
                    +'<option value ="13">reserve for Common interface Usage</option>'
                    +'<option value ="14">RCS Map (see EN 301 790[34])</option>'
                    +'<option value ="15">RCS FLS (see EN 301 790[34])</option>'
                    +'<option value ="16">DVB MHP service</option>'
                    +'</select>'
                );
            },
            "columns": [
                {"title": globalObj._nv == "zh-CN"?"业务ID":"ID"},
                {"title": globalObj._nv == "zh-CN"?"类型":"TYPE"}
            ]
        });
        $('#tbl_nits tbody').on('click', 'tr', function () {
            if ($(this).hasClass('selected')) {
                $(this).removeClass('selected');
            } else {
                $('#tbl_nits').DataTable().$('tr.selected').removeClass('selected');
                $(this).addClass('selected');
            }
        });
    }
    $.each(dataSet, function(key, itemv) {
        $('#nits_type' + key)[0].options[Number(itemv[1])].selected = true;
    });
}

function editnitt(data, res){
    $('.nitt_id').val(res.streamid);
    $('.nitt_netid').val(res.netid);
    $('.nitt_hz').val(res.hz);
    $('#nitt_bandwidth')[0].options[res.bandwidth].selected = true;
    $('#nitt_constell')[0].options[res.constell].selected = true;
    $('#nitt_transmissionMode')[0].options[res.transmissionMode].selected = true;
    $('#nitt_hierarchy')[0].options[res.hierarchy].selected = true;
    $('#nitt_hp')[0].options[res.hp].selected = true;
    $('#nitt_lp')[0].options[res.lp].selected = true;
    $('#nitt_gi')[0].options[res.gi].selected = true;
    dataSet.length = 0;
    if(res.children){
        $.each(res.children, function(key, itemv) {
            var item = [itemv.serid,itemv.sertype];
            dataSet[dataSet.length] = item;
        });
    }
    //编辑节目对话框表
    if ( $.fn.dataTable.isDataTable( '#tbl_nitt' ) ) {
        $('#tbl_nitt').dataTable().fnClearTable();
        if(dataSet.length > 0)
            $('#tbl_nitt').dataTable().fnAddData(dataSet);
    }else{
        globalObj._tbl_nitc = $('#tbl_nitt').dataTable({
            "data": dataSet,
            "order": [[0, "asc"]],
            "paging": false,
            "info": false,
            "searching": false,
            "fnRowCallback": function (nRow, aData, iDisplayIndex) {
                $('td:eq(0)', nRow).html( '<input type="text" pattern="(^[0-9]{1,4}$)" id="nitt_id'+iDisplayIndex+ '" name="nitt_id'+iDisplayIndex+ '" value="'+ aData[0] + '"></input>' );
                $('td:eq(1)', nRow).html(
                    '<select id="nitt_type'+iDisplayIndex+ '" name="nitt_type'+iDisplayIndex +'">'
                    +'<option value ="0" selected="selected">reserve</option>'
                    +'<option value ="1">digital television service</option>'
                    +'<option value ="2">digital radio sound service</option>'
                    +'<option value ="3">Teletext service</option>'
                    +'<option value ="4">NVOD reference service</option>'
                    +'<option value ="5">NVOD time-shifted service</option>'
                    +'<option value ="6">mosaic service</option>'
                    +'<option value ="7">PAL coded signal</option>'
                    +'<option value ="8">SECAM coded siganl</option>'
                    +'<option value ="9">D/D2-MAC</option>'
                    +'<option value ="10">FM Radio</option>'
                    +'<option value ="11">NTSC coded signal</option>'
                    +'<option value ="12">data broadcast service</option>'
                    +'<option value ="13">reserve for Common interface Usage</option>'
                    +'<option value ="14">RCS Map (see EN 301 790[34])</option>'
                    +'<option value ="15">RCS FLS (see EN 301 790[34])</option>'
                    +'<option value ="16">DVB MHP service</option>'
                    +'</select>'
                );
            },
            "columns": [
                {"title": globalObj._nv == "zh-CN"?"业务ID":"ID"},
                {"title": globalObj._nv == "zh-CN"?"类型":"TYPE"}
            ]
        });
        $('#tbl_nitt tbody').on('click', 'tr', function () {
            if ($(this).hasClass('selected')) {
                $(this).removeClass('selected');
            } else {
                $('#tbl_nitt').DataTable().$('tr.selected').removeClass('selected');
                $(this).addClass('selected');
            }
        });
    }
    $.each(dataSet, function(key, itemv) {
        $('#nitt_type' + key)[0].options[Number(itemv[1])].selected = true;
    });
}

function checkselectedprg(nodekey, selected, snode){
    var chantree;
    var snt;
    if(globalObj._channel == 1){
        chantree = $("#channel").fancytree("getTree");
        snt = globalObj._selectcount;
    }else if(globalObj._channel == 2){
        chantree = $("#channel2").fancytree("getTree");
        snt = globalObj._selectcount2;
    }
    var prgnode;
    var jsondata = new Array();
    var arr = nodekey.match(/\./g);
    if(selected){
        switch(arr.length){
            case 1:	//通道节点
                if(nodekey == "id1.0"){
                    var chstr = "flag:1,"+"selected:1";
                    jsondata[0] = chstr;
                }else{
                    var chstr = "flag:2,"+"selected:1,ch:"+ nodekey.substr(nodekey.indexOf('.')+1);
                    jsondata[0] = chstr;
                }
                break;
            case 2:	//节目节点
                prgnode = chantree.getNodeByKey(nodekey);
                if(nodekey.indexOf("cat")>0){
                    var chstr = "flag:8,"+"selected:1,ch:"+ snode.data.chnid;
                }else{
                    var chstr = "flag:3,"+"selected:1,ch:"+ snode.data.chnid + ",index:"+ snode.data.index;
                }
                jsondata[0] = chstr;
                break;
            case 3: //流节点
            case 4:
                prgnode = chantree.getNodeByKey(snode.getParent().key);
                if(nodekey.indexOf("cat")>0){
                    var chstr = "flag:9,"+"selected:1,ch:"+ prgnode.data.chnid + ",index:"+ snode.data.index;
                }else{
                    var chstr = "flag:4,"+"selected:1,ch:"+ prgnode.data.chnid + ",index:"+ prgnode.data.index
                        + ",streamindex:"+ snode.data.index;
                }
                jsondata[0] = chstr;
                break;
            default:

                break;
        }
    }else{
        switch(arr.length){
            case 1:	//通道节点
                if(nodekey == "id1.0"){
                    var chstr = "flag:1,"+"selected:0";
                    jsondata[0] = chstr;
                }else{
                    var chstr = "flag:2,"+"selected:0,ch:"+ nodekey.substr(nodekey.indexOf('.')+1);
                    jsondata[0] = chstr;
                }
                break;
            case 2:	//节目节点
                if(nodekey.indexOf("cat")>0){
                    var chstr = "flag:8,"+"selected:0,ch:"+ snode.data.chnid;
                }else {
                    var chstr = "flag:3," + "selected:0,ch:" + snode.data.chnid + ",index:" + snode.data.index;
                }
                jsondata[0] = chstr;
                break;
            case 3:
            case 4:
                prgnode = chantree.getNodeByKey(snode.getParent().key);
                if(!prgnode.hasChildren()){
                    prgnode.remove();
                }
                if(nodekey.indexOf("cat")>0){
                    var chstr = "flag:9,"+"selected:0,ch:"+ prgnode.data.chnid + ",index:"+ snode.data.index;
                }else {
                    var chstr = "flag:4," + "selected:0,ch:" + prgnode.data.chnid + ",index:" + prgnode.data.index
                        + ",streamindex:" + snode.data.index;
                }
                jsondata[0] = chstr;
                break;
            //break;
            default:
                break;
        }

    }
    $.ajax({
        type: "GET",
        async:false,
        url: "http://"+globalObj.localip+":4000/do/programs/selectprgs",
        data: '{' + jsondata.toString() + ',channel:'+globalObj._channel+',prgcnt:'+snt+'}',
        dataType: "json",
        success: function(data){

        },
        error : function(err) {
            alert(err);
        }
    });
}

function readprgs(){
    var node;
    var channeltree;
    var devlisttree;
    if(globalObj._channel == 1){
        channeltree =  $("#channel").fancytree("getTree");
        devlisttree = $("#devlist").fancytree("getTree");
        globalObj._selectcount = 0;
    }else if(globalObj._channel == 2){
        channeltree =  $("#channel2").fancytree("getTree");
        devlisttree = $("#devlist2").fancytree("getTree");
        globalObj._selectcount2 = 0;
    }
    channeltree.reload();
    devlisttree.reload();
    node = devlisttree.getNodeByKey("id1.0");
    globalObj.localip = window.location.href.substr(7, window.location.href.indexOf(':', 7) - 7);
    //获取全局初始化信息
    $.ajax({
        type: "GET",
        async:false,
        url: "http://"+globalObj.localip+":4000/do/programs/getglobalinfo?channel="+globalObj._channel,
        dataType: "json",
        success: function(data){
            if(data.sts == 9){
                window.location = "/login.esp";
                return;
            }
            globalObj._intChannelCntMax = data._intChannelCntMax;
        },
        error : function(err) {

        }
    });

    if(globalObj._intChannelCntMax != 0 || globalObj._intChannelCntMax != ""){
        for(var i=1;i<globalObj._intChannelCntMax+1; i++){
            $.ajax({
                type: "GET",
                async:false,
                url: "http://"+globalObj.localip+":4000/do/programs/getprg?inch="+i,
                dataType: "json",
                success: function(data){
                    data = JSON.stringify(data).replace('\\','');
                    var treeData1 = JSON.parse(data);
                    node.addChildren(treeData1);
                },
                error : function(err) {
                    alert("异常！====="+JSON.stringify(err));
                }
            });
        }
    }
    //获取输出通道信息
    $.ajax({
        type: "GET",
        async:false,
        url: "http://"+globalObj.localip+":4000/do/programs/getoutprg?inch="+ globalObj._channel+"&isEnableOutChannel=1",
        dataType: "json",
        success: function(data){
            if(data.sts == 1){

            }else{
                $.each(data, function(key, prg) {
                    var nkey = "id1." + prg.ch;
                    node = channeltree.getNodeByKey(nkey);
                    node.addChildren(prg.children);
                    var prgkey = prg.children.key;
                    node = devlisttree.getNodeByKey(prgkey);
                    if(globalObj._channel == 1 && prgkey.indexOf("cat")<0){
                        globalObj._selectcount++;
                    }else if(globalObj._channel == 2 && prgkey.indexOf("cat")<0){
                        globalObj._selectcount2++;
                    }
                    if(node != null){
                        node.setSelected(true);
                    }
                });

                var prgnode = channeltree.getNodeByKey("id1.0");
                if(globalObj._channel == 1){
                    prgnode.setTitle(globalObj._nv == "zh-CN"?"节目: ":"Program: "+ globalObj._selectcount);
                }else if(globalObj._channel == 2){
                    prgnode.setTitle(globalObj._nv == "zh-CN"?"节目: ":"Program: "+ globalObj._selectcount2);
                }
                prgnode.render();
                createTable();
            }

        },
        error : function(err) {
            alert("异常！====="+JSON.stringify(err));
        }
    });
}

function readoutprgs(channeltree, snt){
    channeltree.reload();
    var prgnode = channeltree.getNodeByKey("id1.0");
    //获取输出通道信息
    $.ajax({
        type: "GET",
        async:false,
        url: "http://"+globalObj.localip+":4000/do/programs/getoutprg?inch="+ globalObj._channel+"&isEnableOutChannel=0",
        dataType: "json",
        success: function(data){
            if(data.sts == 1){

            }else{
                snt = 0;
                $.each(data, function(key, prg) {
                    var nkey = "id1." + prg.ch;
                    node = channeltree.getNodeByKey(nkey);
                    node.addChildren(prg.children);
                });
                snt = data.length;
                if(globalObj._channel == 1){
                    globalObj._selectcount = snt;
                }else{
                    globalObj._selectcount2 = snt;
                }
                prgnode.setTitle(globalObj._nv == "zh-CN"?"节目: ":"Program: "+ snt);
                prgnode.render();
            }
        },
        error : function(err) {
            alert("异常！====="+JSON.stringify(err));
        }
    });
}
	
function devinfo_output(devType){
    if(globalObj.timerID != undefined){
        clearInterval(globalObj.timerID);
    }
	$('.main-content').empty();
	if(devType == "MUX8in1"){
		$('.main-content').append(
			'<div id="devoutput">'
				+'<ul>'
					+'<li><a href="#tabs-1" class="i18n_OUTPUT1">OUTPUT1</a></li>'
					+'<li><a href="#tabs-2" class="i18n_TABSTRUCT1">LIST STRUCTURE1</a></li>'
					+'<li><a href="#tabs-3" class="i18n_OUTPUT2">OUTPUT2</a></li>'
					+'<li><a href="#tabs-4" class="i18n_TABSTRUCT2">LIST STRUCTURE2</a></li>'
				+'</ul>'
				+'<div id="tabs-1">'
					+'<div class="clearfix">'
						+'<div id="devlist" class="channel_input"></div>'
						+'<div id="channel" class="program"></div>'
					+'</div>'
					+'<div class="tbn_div">'
                        +'<button id="output-search" class="i18n_BTNSEARCH">Search</button>'
						+'<button id="output-read" class="i18n_BTNREAD">Read</button>'
						+'<button id="output-table" class="i18n_BTNMAKETABLE">MAKE TABLE</button>'
						+'<button id="output-write" class="i18n_BTNAPPLY">Apply</button>'					
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
						+'<button id="output-read2" class="i18n_BTNREAD">Read</button>'
						+'<button id="output-table2" class="i18n_BTNMAKETABLE">MAKE TABLE</button>'
						+'<button id="output-write2" class="i18n_BTNAPPLY">Apply</button>'					
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
	}
	$('.main-content').append(
		'<div id="dialog-NIT" title="NIT">'
			+'<div class="nit_edit">'
				+'<label class="i18n_NETID">NETID &nbsp</label>'
				+'<input type="text" pattern="(^0x[a-f0-9]{1,4}$)|(^0X[A-F0-9]{1,4}$)|(^[A-F0-9]{1,4}$)|(^[a-f0-9]{1,4}$)" class="nit_id" value="" />&nbsp&nbsp(HEX)<br/>'
				+'<label class="i18n_NETNAME">NET NAME</label>'
				+'<input type="text" class="nit_name" value=""/>'
				+'<br/>'
				+'<textarea rows="8" style="width:350px;margin-left:30px">'
				+'</textarea>'
			+'</div>'
		+'</div>'
		+'<!-- Definition of context menu -->'
		+'<ul id="table_menu" class="contextMenu ui-helper-hidden">'
			+'<li class="menu_expandall"><a href="#expandall" class="i18n_EXPANDALL"><span class="ui-icon ui-icon-folder-open"></span>EXPAND ALL</a></li>'
			+'<li class="menu_collasp"><a href="#collasp" class="i18n_COLLASP"><span class="ui-icon ui-icon-folder-collapsed"></span>COLLASP</a></li>'
            +'<li class="menu_editstr"><a href="#editstr" class="i18n_EDITSTR"><span class="ui-icon ui-icon-pencil"></span>STREAM EDIT</a></li>'
            +'<li class="menu_deletestr"><a href="#deletestr" class="i18n_DELETESTR"><span class="ui-icon ui-icon-closethick"></span>STREAM DELETE</a></li>'
            +'<li class="menu_hr">---</li>'
            +'<li class="menu_edit"><a href="#edit" class="i18n_EDIT"><span class="ui-icon ui-icon-pencil"></span>EDIT SECTION</a></li>'
			+'<li class="menu_add"><a href="#add" class="i18n_ADD"><span class="ui-icon ui-icon-plusthick"></span>ADD SECTION</a></li>'
			+'<li class="menu_delete"><a href="#delete" class="i18n_DELETE"><span class="ui-icon ui-icon-closethick"></span>DELETE SECTION</a></li>'
            +'<li class="menu_deleteall"><a href="#deleteall" class="i18n_DELETEALL"><span class="ui-icon ui-icon-closethick"></span>DEL ALL SECTIONS</a></li>'
            +'<li class="menu_deleteallstr"><a href="#deleteallstr" class="i18n_DELETEALLSTR"><span class="ui-icon ui-icon-closethick"></span>DEL ALL STREAMS</a></li>'
			+'<li class="menu_hr">---</li>'
            +'<li class="menu_addstrc"><a href="#addstrc" class="i18n_ADDSTRC"><span class="ui-icon ui-icon-plusthick"></span>ADD STREAM[DVB-C]</a></li>'
            +'<li class="menu_addstrs"><a href="#addstrs" class="i18n_ADDSTRS"><span class="ui-icon ui-icon-plusthick"></span>ADD STREAM[DVB-S]</a></li>'
            +'<li class="menu_addstrt"><a href="#addstrt" class="i18n_ADDSTRT"><span class="ui-icon ui-icon-plusthick"></span>ADD STREAM[DVB-T]</a></li>'
			+'<li class="menu_import"><a href="#import" class="i18n_NITIMPORT"><span class="ui-icon ui-icon-pencil"></span>NIT IMPORT</a></li>'			
			+'<li class="menu_export"><a href="#export" class="i18n_NITEXPORT"><span class="ui-icon ui-icon-closethick"></span>NIT EXPORT</a></li>'			
			
		+'</ul>'
		+'<ul id="table_menu2" class="contextMenu ui-helper-hidden">'
            +'<li class="menu_expandall"><a href="#expandall" class="i18n_EXPANDALL"><span class="ui-icon ui-icon-folder-open"></span>EXPAND ALL</a></li>'
            +'<li class="menu_collasp"><a href="#collasp" class="i18n_COLLASP"><span class="ui-icon ui-icon-folder-collapsed"></span>COLLASP</a></li>'
            +'<li class="menu_editstr"><a href="#editstr" class="i18n_EDITSTR"><span class="ui-icon ui-icon-pencil"></span>STREAM EDIT</a></li>'
            +'<li class="menu_deletestr"><a href="#deletestr" class="i18n_DELETESTR"><span class="ui-icon ui-icon-closethick"></span>STREAM DELETE</a></li>'
            +'<li class="menu_hr">---</li>'
            +'<li class="menu_edit"><a href="#edit" class="i18n_EDIT"><span class="ui-icon ui-icon-pencil"></span>EDIT SECTION</a></li>'
            +'<li class="menu_add"><a href="#add" class="i18n_ADD"><span class="ui-icon ui-icon-plusthick"></span>ADD SECTION</a></li>'
            +'<li class="menu_delete"><a href="#delete" class="i18n_DELETE"><span class="ui-icon ui-icon-closethick"></span>DELETE SECTION</a></li>'
            +'<li class="menu_deleteall"><a href="#deleteall" class="i18n_DELETEALL"><span class="ui-icon ui-icon-closethick"></span>DEL ALL SECTIONS</a></li>'
            +'<li class="menu_deleteallstr"><a href="#deleteallstr" class="i18n_DELETEALLSTR"><span class="ui-icon ui-icon-closethick"></span>DEL ALL STREAMS</a></li>'
            +'<li class="menu_hr">---</li>'
            +'<li class="menu_addstrc"><a href="#addstrc" class="i18n_ADDSTRC"><span class="ui-icon ui-icon-plusthick"></span>ADD STREAM[DVB-C]</a></li>'
            +'<li class="menu_addstrs"><a href="#addstrs" class="i18n_ADDSTRS"><span class="ui-icon ui-icon-plusthick"></span>ADD STREAM[DVB-S]</a></li>'
            +'<li class="menu_addstrt"><a href="#addstrt" class="i18n_ADDSTRT"><span class="ui-icon ui-icon-plusthick"></span>ADD STREAM[DVB-T]</a></li>'
            +'<li class="menu_import"><a href="#import" class="i18n_NITIMPORT"><span class="ui-icon ui-icon-pencil"></span>NIT IMPORT</a></li>'
            +'<li class="menu_export"><a href="#export" class="i18n_NITEXPORT"><span class="ui-icon ui-icon-closethick"></span>NIT EXPORT</a></li>'

        +'</ul>'
		+'<!-- Definition of context menu -->'
		+'<ul id="program_menu" class="contextMenu ui-helper-hidden">'
			+'<li class="menu_expandall"><a href="#expandall" class="i18n_EXPANDALL"><span class="ui-icon ui-icon-folder-open"></span>EXPAND ALL</a></li>'
			+'<li class="menu_collasp"><a href="#collasp" class="i18n_COLLASP"><span class="ui-icon ui-icon-folder-collapsed"></span>COLLASP</a></li>'
			+'<li>---</li>'
			+'<li class="menu_delete"><a href="#delete" class="i18n_DELPRG"><span class="ui-icon ui-icon-closethick"></span>Program DELETE</a></li>'
			+'<li class="menu_edit"><a href="#edit" class="i18n_EDITPRG"><span class="ui-icon ui-icon-pencil"></span>Program EDIT</a></li>'			
			+'<li class="menu_deleteall"><a href="#deleteall" class="i18n_DELALLDESC"><span class="ui-icon ui-icon-closethick"></span>Delete the next level descriptors</a></li>'			
			+'<li class="menu_add"><a href="#add" class="i18n_ADDDESC"><span class="ui-icon ui-icon-plusthick"></span>Add descriptors</a></li>'
			
			+'<li class="menu_prgdeleteall" style="display:none"><a href="#prgdeleteall" class="i18n_DELALLPRG"><span class="ui-icon ui-icon-closethick"></span>DEL ADD Programs</a></li>'
            +'<li class="menu_chndelete" style="display:none"><a href="#chndelete" class="i18n_DELCHALLPRG"><span class="ui-icon ui-icon-closethick"></span>Delete all programs on this channel</a></li>'
            +'<li class="menu_prgdeletecus" style="display:none"><a href="#prgdeletecus" class="i18n_DELAUTOALLPRG"><span class="ui-icon ui-icon-closethick"></span>Delete all self increasing programs</a></li>'
            +'<li>---</li>'
			+'<li class="menu_prgitems" style="display:none"><a href="#itmes" class="i18n_ITEM"><span class="ui-icon ui-icon-tag"></span>ITEM</a></li>'			
			+'<li class="menu_re_prg" style="display:none"><a href="#re_prg" class="i18n_REPRGID"><span class="ui-icon ui-icon-refresh"></span>Re distribution program number</a></li>'			
			+'<li class="menu_re_pid" style="display:none"><a href="#re_pid" class="i18n_REPRGPID"><span class="ui-icon ui-icon-refresh"></span>Re distribution PID</a></li>'
            +'<li class="menu_cus_add" style="display:none"><a href="#cus_add" class="i18n_ADDAUTOPRG"><span class="ui-icon ui-icon-add"></span>Add self increasing program</a></li>'
			+'<li class="menu_pidtrans" style="display:none"><a href="#pidtrans" class="i18n_PIDTRANS"><span class="ui-icon ui-icon-pin-s"></span>PID Transmission</a></li>'

		+'</ul>'
		+'<ul id="inputprg_menu" class="contextMenu ui-helper-hidden">'
			+'<li class="menu_expandall"><a href="#expandall" class="i18n_EXPANDALL"><span class="ui-icon ui-icon-folder-open"></span>EXPAND ALL</a></li>'
			+'<li class="menu_collasp"><a href="#collasp" class="i18n_COLLASP"><span class="ui-icon ui-icon-folder-collapsed"></span>COLLASP</a></li>'
			+'<li>---</li>'
            +'<li class="menu_search"><a href="#search" class="i18n_CHSEARCH"><span class="ui-icon ui-icon-search"></span>SEARCH</a></li>'
			+'<li class="menu_delete"><a href="#delete" class="i18n_CHDELALL"><span class="ui-icon ui-icon-closethick"></span>Delete all programs on this channel</a></li>'
		+'</ul>'
		+'<ul id="program_menu2" class="contextMenu ui-helper-hidden">'
        +'<li class="menu_expandall"><a href="#expandall" class="i18n_EXPANDALL"><span class="ui-icon ui-icon-folder-open"></span>EXPAND ALL</a></li>'
        +'<li class="menu_collasp"><a href="#collasp" class="i18n_COLLASP"><span class="ui-icon ui-icon-folder-collapsed"></span>COLLASP</a></li>'
        +'<li>---</li>'
        +'<li class="menu_delete"><a href="#delete" class="i18n_DELPRG"><span class="ui-icon ui-icon-closethick"></span>Program DELETE</a></li>'
        +'<li class="menu_edit"><a href="#edit"><span class="ui-icon ui-icon-pencil"></span>Program EDIT</a></li>'
        +'<li class="menu_deleteall"><a href="#deleteall" class="i18n_DELALLDESC"><span class="ui-icon ui-icon-closethick"></span>Delete the next level descriptors</a></li>'
        +'<li class="menu_add"><a href="#add" class="i18n_ADDDESC"><span class="ui-icon ui-icon-plusthick"></span>Add descriptors</a></li>'

        +'<li class="menu_prgdeleteall" style="display:none"><a href="#prgdeleteall" class="i18n_DELALLPRG"><span class="ui-icon ui-icon-closethick"></span>DEL ALL Programs</a></li>'
        +'<li class="menu_chndelete" style="display:none"><a href="#chndelete" class="i18n_CHDELALL"><span class="ui-icon ui-icon-closethick"></span>Delete all programs on this channel</a></li>'
        +'<li class="menu_prgdeletecus" style="display:none"><a href="#prgdeletecus" class="i18n_DELAUTOALLPRG"><span class="ui-icon ui-icon-closethick"></span>Delete all self increasing programs</a></li>'
        +'<li>---</li>'
        +'<li class="menu_prgitems" style="display:none"><a href="#itmes" class="i18n_ITEM"><span class="ui-icon ui-icon-tag"></span>ITEM</a></li>'
        +'<li class="menu_re_prg" style="display:none"><a href="#re_prg" class="i18n_REPRGID"><span class="ui-icon ui-icon-refresh"></span>Re distribution program number</a></li>'
        +'<li class="menu_re_pid" style="display:none"><a href="#re_pid" class="i18n_REPRGPID"><span class="ui-icon ui-icon-refresh"></span>Re distribution PID</a></li>'
        +'<li class="menu_cus_add" style="display:none"><a href="#cus_add" class="i18n_ADDAUTOPRG"><span class="ui-icon ui-icon-add"></span>Add self increasing program</a></li>'
        +'<li class="menu_pidtrans" style="display:none"><a href="#pidtrans" class="i18n_PIDTRANS"><span class="ui-icon ui-icon-pin-s"></span>PID Transmission</a></li>'

        +'</ul>'
		+'<ul id="inputprg_menu2" class="contextMenu ui-helper-hidden">'
			+'<li class="menu_expandall"><a href="#expandall" class="i18n_EXPANDALL"><span class="ui-icon ui-icon-folder-open"></span>EXPAND ALL</a></li>'
			+'<li class="menu_collasp"><a href="#collasp" class="i18n_COLLASP"><span class="ui-icon ui-icon-folder-collapsed"></span>COLLASP</a></li>'
			+'<li>---</li>'
        +'<li class="menu_search"><a href="#search" class="i18n_CHSEARCH"><span class="ui-icon ui-icon-search"></span>SEARCH</a></li>'
        +'<li class="menu_delete"><a href="#delete" class="i18n_CHDELALL"><span class="ui-icon ui-icon-closethick"></span>Delete all programs on this channel</a></li>'
		+'</ul>'
		+'<div id="dialog-items" class="i18n_OUTCHSET" title="Output channel setting">'
			+'<table class="tbl_items">'
				+'<tr>'
					+'<td><label class="i18n_STREAMID">STREAM ID</label></td><td><input type="text" class="item_transid" value="" /></td>'
				+'</tr>'
				+'<tr>'
					+'<td><label class="i18n_NETWORKID">NET ID</label></td><td><input type="text" class="item_netid" value="" /></td>'
				+'</tr>'
				+'<tr>'
					+'<td><label class="i18n_ORINETWORKID">Original network ID</label></td><td><input type="text" class="item_orignetid" value="" /></td>'
				+'</tr>'
				+'<tr>'
					+'<td><label class="i18n_OUTBITS">Total output rate  (Kpbs)</label></td><td><input type="text" class="item_out" value="" /></td>'
				+'</tr>'
				+'<tr>'
					+'<td><input class="autoinc_ver i18n_AUTOVERSION" type="checkbox">    AUTO increasing Version</input></td><td><label class="item_version">2</label></td>'
				+'</tr>'
			+'</table>'
			+'<input type="checkbox" class="pat_auto i18n_SORTID">    When PAT is generated by the business ID sort</input>'
			+'<fieldset>'
				+'<legend class="i18n_CHOOSETABLE">Select the required table</legend>'
					+'<input type="checkbox" class="sl_pat">    PAT</input>'
					+'<input type="checkbox" class="sl_pmt">    PMT</input>'
					+'<input type="checkbox" class="sl_sdt">    SDT</input>'
					+'<input type="checkbox" class="sl_cat">    CAT</input>'
					+'<input type="checkbox" class="sl_nit">    NIT</input>'
			+'</fieldset>'
			+'<label id="tag_channel" style="display:none"></label>'
		+'</div>'
		+'<div id="dialog-descriptor" class="i18n_DESC" title="Descriptor">'
			+'<div class="clearfix">'
				+'<div class="tbl_descriptor">'
					+'<table cellpadding="0" cellspacing="0" border="0" class="cell-border compact hover" id="tbl_descriptor"></table>'
				+'</div>'				
				+'<div class="desc_content">'
					+'<h3>User define descrptor</h3>'
					+'<label class="i18n_TAG">TAG</label> &nbsp&nbsp&nbsp&nbsp <input class="desc_tag" value="" />&nbsp&nbsp (Hex  eg:4A)'
					+'<br />'
					+'<label class="i18n_LENGTH">LENGTH</label> &nbsp&nbsp&nbsp&nbsp <label class="desc_length"></label>'
					+'<br />'
					+'<label style="float:left" class="i18n_BYTE">BYTE</label> <textarea class="desc_bytes" row="6"></textarea> <br/><br/>'
					+'<h5 class="i18n_TIPS1">Space to separate each byte，Hex value. E.G.：2D 3C 14</h5>'
				+'</div>'
			+'</div>'
		+'</div>'
        +'<div id="progress-dialog" title="SEARCH">'
            +'<div class="progress-label" data-ch = 1><label class="i18n_SEARCHING">Preparing to search...</label></div>'
            +'<div id="progressbar"></div>'
        +'</div>'
        +'<div id="progress-notification" title="NOTE">'
            +'<p class="i18n_APPLYING">Applying, Please waiting...</p>'
        +'</div>'
	);
	$.i18n.properties({
        name : globalObj._nv, //资源文件名称
        path : '/i18n/', //资源文件路径
        mode : 'map', //用Map的方式使用资源文件中的值
        language : globalObj._nv,
        callback : function() {//加载成功后设置显示内容
			$('.i18n_OUTPUT1')[0].textContent = $.i18n.prop('OUTPUT1');
			$('.i18n_OUTPUT2')[0].textContent = $.i18n.prop('OUTPUT2');
			$('.i18n_TABSTRUCT1')[0].textContent = $.i18n.prop('TABSTRUCT1');
			$('.i18n_TABSTRUCT2')[0].textContent = $.i18n.prop('TABSTRUCT2');
			$('.i18n_BTNSEARCH')[0].textContent = $.i18n.prop('BTNSEARCH');
			$('.i18n_BTNREAD')[0].textContent = $.i18n.prop('BTNREAD');
			$('.i18n_BTNREAD')[1].textContent = $.i18n.prop('BTNREAD');
			$('.i18n_BTNMAKETABLE')[0].textContent = $.i18n.prop('BTNMAKETABLE');
			$('.i18n_BTNMAKETABLE')[1].textContent = $.i18n.prop('BTNMAKETABLE'); 
			$('.i18n_BTNAPPLY')[0].textContent = $.i18n.prop('BTNAPPLY');
			$('.i18n_BTNAPPLY')[1].textContent = $.i18n.prop('BTNAPPLY');
			$('.i18n_NETID').html($.i18n.prop('NETID'));
			$('.i18n_NETNAME').html($.i18n.prop('NETNAME'));
			$('.i18n_EXPANDALL')[0].textContent = $.i18n.prop('EXPANDALL');
			$('.i18n_EXPANDALL')[1].textContent = $.i18n.prop('EXPANDALL');
			$('.i18n_EXPANDALL')[2].textContent = $.i18n.prop('EXPANDALL');
			$('.i18n_EXPANDALL')[3].textContent = $.i18n.prop('EXPANDALL');
			$('.i18n_EXPANDALL')[4].textContent = $.i18n.prop('EXPANDALL');
            $('.i18n_COLLASP')[0].textContent = $.i18n.prop('COLLASP');
			$('.i18n_COLLASP')[1].textContent = $.i18n.prop('COLLASP');
			$('.i18n_COLLASP')[2].textContent = $.i18n.prop('COLLASP');
			$('.i18n_COLLASP')[3].textContent = $.i18n.prop('COLLASP');
			$('.i18n_COLLASP')[4].textContent = $.i18n.prop('COLLASP');            
			$('.i18n_EDITSTR')[0].textContent = $.i18n.prop('EDITSTR');
			$('.i18n_EDITSTR')[1].textContent = $.i18n.prop('EDITSTR');			
			$('.i18n_DELETESTR')[0].textContent = $.i18n.prop('DELETESTR');
			$('.i18n_DELETESTR')[1].textContent = $.i18n.prop('DELETESTR');			
			$('.i18n_EDIT')[0].textContent = $.i18n.prop('EDIT');
			$('.i18n_EDIT')[1].textContent = $.i18n.prop('EDIT');			
			$('.i18n_ADD')[0].textContent = $.i18n.prop('ADD');
			$('.i18n_ADD')[1].textContent = $.i18n.prop('ADD');
			$('.i18n_DELETE')[0].textContent = $.i18n.prop('DELETE');
			$('.i18n_DELETE')[1].textContent = $.i18n.prop('DELETE');
			$('.i18n_DELETEALL')[0].textContent = $.i18n.prop('DELETEALL');
			$('.i18n_DELETEALL')[1].textContent = $.i18n.prop('DELETEALL');			
			$('.i18n_DELETEALLSTR')[0].textContent = $.i18n.prop('DELETEALLSTR');
			$('.i18n_DELETEALLSTR')[1].textContent = $.i18n.prop('DELETEALLSTR');
			$('.i18n_ADDSTRC')[0].textContent = $.i18n.prop('ADDSTRC');
			$('.i18n_ADDSTRC')[1].textContent = $.i18n.prop('ADDSTRC');
			$('.i18n_ADDSTRS')[0].textContent = $.i18n.prop('ADDSTRS');
			$('.i18n_ADDSTRS')[1].textContent = $.i18n.prop('ADDSTRS');
			$('.i18n_ADDSTRT')[0].textContent = $.i18n.prop('ADDSTRT');
			$('.i18n_ADDSTRT')[1].textContent = $.i18n.prop('ADDSTRT');
			$('.i18n_NITIMPORT')[0].textContent = $.i18n.prop('NITIMPORT');
			$('.i18n_NITIMPORT')[1].textContent = $.i18n.prop('NITIMPORT');
			$('.i18n_NITEXPORT')[0].textContent = $.i18n.prop('NITEXPORT');
			$('.i18n_NITEXPORT')[1].textContent = $.i18n.prop('NITEXPORT');
			$('.i18n_DELPRG')[0].textContent = $.i18n.prop('DELPRG');
			$('.i18n_DELPRG')[1].textContent = $.i18n.prop('DELPRG');
			$('.i18n_EDITPRG')[0].textContent = $.i18n.prop('EDITPRG');
			$('.i18n_DELALLDESC')[0].textContent = $.i18n.prop('DELALLDESC');
			$('.i18n_DELALLDESC')[1].textContent = $.i18n.prop('DELALLDESC');			
			$('.i18n_ADDDESC')[0].textContent = $.i18n.prop('ADDDESC');
			$('.i18n_ADDDESC')[1].textContent = $.i18n.prop('ADDDESC');
			$('.i18n_DELALLPRG')[0].textContent = $.i18n.prop('DELALLPRG');
			$('.i18n_DELALLPRG')[1].textContent = $.i18n.prop('DELALLPRG');
			$('.i18n_DELCHALLPRG')[0].textContent = $.i18n.prop('DELCHALLPRG');
			$('.i18n_DELAUTOALLPRG')[0].textContent = $.i18n.prop('DELAUTOALLPRG');
			$('.i18n_DELAUTOALLPRG')[1].textContent = $.i18n.prop('DELAUTOALLPRG');			
			$('.i18n_ITEM')[0].textContent = $.i18n.prop('ITEM');
			$('.i18n_ITEM')[1].textContent = $.i18n.prop('ITEM');
			$('.i18n_REPRGID')[0].textContent = $.i18n.prop('REPRGID');
			$('.i18n_REPRGID')[1].textContent = $.i18n.prop('REPRGID');
			$('.i18n_REPRGPID')[0].textContent = $.i18n.prop('REPRGPID');
			$('.i18n_REPRGPID')[1].textContent = $.i18n.prop('REPRGPID');
			$('.i18n_ADDAUTOPRG')[0].textContent = $.i18n.prop('ADDAUTOPRG');
			$('.i18n_ADDAUTOPRG')[1].textContent = $.i18n.prop('ADDAUTOPRG');
			$('.i18n_PIDTRANS')[0].textContent = $.i18n.prop('PIDTRANS');
			$('.i18n_PIDTRANS')[1].textContent = $.i18n.prop('PIDTRANS');
			$('.i18n_CHSEARCH')[0].textContent = $.i18n.prop('CHSEARCH');
			$('.i18n_CHSEARCH')[1].textContent = $.i18n.prop('CHSEARCH');
			$('.i18n_CHDELALL')[0].textContent = $.i18n.prop('CHDELALL');
			$('.i18n_CHDELALL')[1].textContent = $.i18n.prop('CHDELALL');
			$('.i18n_OUTCHSET')[0].title = $.i18n.prop('OUTCHSET');			
			$('.i18n_STREAMID').html($.i18n.prop('STREAMID'));
			$('.i18n_NETWORKID').html($.i18n.prop('NETWORKID'));
			$('.i18n_ORINETWORKID').html($.i18n.prop('ORINETWORKID'));
			$('.i18n_OUTBITS').html($.i18n.prop('OUTBITS'));
			$('.i18n_AUTOVERSION')[0].nextSibling.data = $.i18n.prop('AUTOVERSION');
			$('.i18n_SORTID')[0].nextSibling.data = $.i18n.prop('SORTID');
			$('.i18n_CHOOSETABLE').html($.i18n.prop('CHOOSETABLE'));
			$('.i18n_DESC')[0].title = $.i18n.prop('DESC');
			$('.i18n_TAG').html($.i18n.prop('TAG'));
			$('.i18n_LENGTH').html($.i18n.prop('LENGTH'));
			$('.i18n_BYTE').html($.i18n.prop('BYTE'));
			$('.i18n_TIPS1').html($.i18n.prop('TIPS1'));
			$('.i18n_SEARCHING').html($.i18n.prop('SEARCHING'));
			$('.i18n_APPLYING').html($.i18n.prop('APPLYING'));
        }
    });

	//描述符表
	$('#tbl_descriptor').dataTable( {
		"data": dataSet1,
		"order": [[ 0, "asc" ]],
		"paging":   false,
		"info":     false,
		"searching":   false,
		"scrollCollapse": true,
		"columns": [
			{ "title": globalObj._nv == "zh-CN"?"标签":"TAG", "width":"40%" },
			{ "title": globalObj._nv == "zh-CN"?"描述符":"DESC"}
		]
	});	 	

	var tabs = $("#devoutput").tabs();
    $( "#output-search" ).button({
        icons: {
            primary: "ui-icon-search"
        }
    }).click(function( event ) {
        event.preventDefault();
        globalObj.localip = window.location.href.substr(7, window.location.href.indexOf(':', 7) - 7);
        if(globalObj._intChannelCntMax == 0){
            $.ajax({
                type: "GET",
                async:false,
                url: "http://"+globalObj.localip+":4000/do/programs/getglobalinfo?channel="+globalObj._channel,
                dataType: "json",
                success: function(data){
                    globalObj._intChannelCntMax = data._intChannelCntMax;
                },
                error : function(err) {

                }
            });
        }
        if(globalObj._intChannelCntMax != 0 || globalObj._intChannelCntMax != ""){
            $('.progress-label')[0].dataset.ch = 1;
            progress_dialog.dialog( "open" );
            progressbar.progressbar( "value", 0 );
            var flag = false;
            var offset = 2;
            if(os == "Chrome"){
                var flag = true;
            }else{
                var flag = false;
            }
            for(var i=1;i<globalObj._intChannelCntMax+1; i++){
                $.ajax({
                    type: "GET",
                    async: flag,
                    url: "http://"+globalObj.localip+":4000/do/programs/search?inch="+i,
                    dataType: "json",
                    success: function(data){
                        //更新进度条
                        if(globalObj._intChannelCntMax == i){
                            progressbar.progressbar( "value", 10 * 10 );
                        }else{
                            progressbar.progressbar( "value", Math.round((offset-1)*100/globalObj._intChannelCntMax ));
                            $('.progress-label')[0].dataset.ch = offset;
                            offset++;
                        }

                    },
                    error : function(err) {
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
        globalObj._channel = 1;
		readprgs();
    });
	
	$( "#output-table" ).button({
      icons: {
        primary: "ui-icon-gear"
      }
    }).click(function( event ) {
        event.preventDefault();
        globalObj._channel = 1;
        if(globalObj._selectcount > 29){
            alert(globalObj._nv == "zh-CN"?"制表节目数已溢出!":"Program number overflow!");
            return;
        }
		$("#out_tree").fancytree("getTree").reload();
        globalObj.localip = window.location.href.substr(7, window.location.href.indexOf(':', 7) - 7);
        createTable();
    });
	
	$( "#output-write" ).button({
      icons: {
        primary: "ui-icon-pencil"
      }
    }).click(function( event ) {
        if(globalObj._selectcount > 29){
            alert(globalObj._nv == "zh-CN"?"制表节目数已溢出!":"Program number overflow!");
            return;
        }
        dig_notification.dialog( "open" );
		$.ajax({
			 type: "GET",
			 async:true,
			 url: "http://"+globalObj.localip+":4000/do/programs/writetable?channel=" + 1,
			 dataType: "json",
			 success: function(data){
                 if(data.sts == 5){
                     alert(globalObj._nv == "zh-CN"?"该用户权限不足.":"Permission Denied!");
                 }else if(data.sts == 9){
                     window.location = "/login.esp";
                 }
                 dig_notification.dialog( "close" );
			 },
			 error : function(err) {
                 dig_notification.dialog( "close" );
			 }
		});
    });
	
	$( "#output-read2" ).button({
      icons: {
        primary: "ui-icon-comment"
      }
    }).click(function( event ) {
        event.preventDefault();
        globalObj._channel = 2;
        readprgs();
    });
	
	$( "#output-table2" ).button({
      icons: {
        primary: "ui-icon-gear"
      }
    }).click(function( event ) {
        event.preventDefault();
        globalObj._channel = 2;
        if(globalObj._selectcount2 > 29){
            alert(globalObj._nv == "zh-CN"?"制表节目数已溢出!":"Program number overflow!");
            return;
        }
        globalObj.localip = window.location.href.substr(7, window.location.href.indexOf(':', 7) - 7);
        createTable();
    });
	
	$( "#output-write2" ).button({
      icons: {
        primary: "ui-icon-pencil"
      }
    }).click(function( event ) {
        //event.preventDefault();
        if(globalObj._selectcount2 > 29){
            alert(globalObj._nv == "zh-CN"?"制表节目数已溢出!":"Program number overflow!");
            return;
        }
        dig_notification.dialog( "open" );
        $.ajax({
            type: "GET",
            async:true,
            url: "http://"+globalObj.localip+":4000/do/programs/writetable?channel=" + 2,
            dataType: "json",
            success: function(data){
                if(data.sts == 5){
                    alert(globalObj._nv == "zh-CN"?"该用户权限不足.":"Permission Denied!");
                }else if(data.sts == 9){
                    window.location = "/login.esp";
                }
                dig_notification.dialog( "close" );
            },
            error : function(err) {
                //alert(err);
                dig_notification.dialog( "close" );
            }
        });
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
                    $(".menu_search").css("display", "none");
				}else if(data.node.key.length == 5){
					$(".menu_delete").css("display", "block");
                    $(".menu_search").css("display", "block");
				}else{
                    $(".menu_delete").css("display", "none");
                    $(".menu_search").css("display", "none");
                }
			},
			select: function(event, data){
                globalObj._channel = 1;
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
                            data.node.getFirstChild().setSelected(false);
							data.node.getFirstChild().remove();
						}
						break;
					} case '#search': {
                        dig_notification.dialog( "open" );
                        $.ajax({
                            type: "GET",
                            async: false,
                            url: "http://"+globalObj.localip+":4000/do/programs/search?inch="+data.node.key.substring(data.node.key.indexOf('.')+1),
                            dataType: "json",
                            success: function(res){
                                $.ajax({
                                    type: "GET",
                                    async:false,
                                    url: "http://"+globalObj.localip+":4000/do/programs/getprg?inch="+data.node.key.substring(data.node.key.indexOf('.')+1),
                                    dataType: "json",
                                    success: function(res){
                                        $.ajax({
                                            type: "GET",
                                            async:false,
                                            url: "http://"+globalObj.localip+":4000/do/programs/getprg?inch="+data.node.key.substring(data.node.key.indexOf('.')+1),
                                            dataType: "json",
                                            success: function(resp){
                                                data.node.removeChildren();
                                                resp = JSON.stringify(resp).replace('\\','');
                                                var treeData1 = JSON.parse(resp);
                                                if(treeData1[0].children != undefined){
                                                    $.each(treeData1[0].children, function(index,item){
                                                        data.node.addChildren(item);
                                                    });
                                                }
                                                readoutprgs($("#channel").fancytree("getTree"),0);
                                                createTable();
                                                dig_notification.dialog( "close" );
                                            },
                                            error : function(err) {
                                                alert("异常！====="+JSON.stringify(err));
                                            }
                                        });
                                    },
                                    error : function(err) {
                                        alert("异常！====="+JSON.stringify(err));
                                    }
                                });
                            },
                            error : function(err) {
                                alert("异常！====="+JSON.stringify(err));
                            }
                        });
                        break;
                    }default: {
						break;
					}			  
				}
			}
		},
		select: function(event, data) {

		},
		click: function(event, data) {
			if( $.ui.fancytree.getEventTargetType(event) === "title" ){
			    data.node.toggleSelected();
			}
            if(data.targetType == "checkbox"){
                data.node.toggleSelected();
                globalObj._channel = 1;
                var tmpnode;
                var channeltree =  $("#channel").fancytree("getTree");
                //删除节目树节点
                var prgnode = channeltree.getNodeByKey("id1.0");

                //添加至节目树
                var selnode = data.tree.getNodeByKey(data.node.key);
                var arr = data.node.key.match(/\./g);
                if(data.node.selected){
                    switch(arr.length){
                        case 1:	//通道节点
                            if(data.node.key == "id1.0"){
                                $.each(data.node.children, function(index,chnode){
                                    if(chnode.children != null){
                                        $.each(chnode.children, function(index,prgnode){
                                            if(channeltree.getNodeByKey(prgnode.key) != null){
                                                channeltree.getNodeByKey(prgnode.key).remove();
                                                tmpnode = channeltree.getNodeByKey(prgnode.parent.key);
                                                tmpnode.addNode(prgnode.toDict(true));
                                            }else{
                                                tmpnode = channeltree.getNodeByKey(prgnode.parent.key);
                                                tmpnode.addNode(prgnode.toDict(true));
                                                globalObj._selectcount++;
                                            }
                                        });
                                    }
                                });

                            }else{
                                $.each(data.node.children, function(index,item){
                                    if(channeltree.getNodeByKey(item.key) != null){
                                        channeltree.getNodeByKey(item.key).remove();
                                        tmpnode = channeltree.getNodeByKey(item.parent.key);
                                        tmpnode.addNode(item.toDict(true));
                                    }else{
                                        tmpnode = channeltree.getNodeByKey(item.parent.key);
                                        tmpnode.addNode(item.toDict(true));
                                        globalObj._selectcount++;
                                    }
                                });
                            }
                            break;
                        case 2:	//节目节点
                            if(channeltree.getNodeByKey(data.node.key) != null){
                                channeltree.getNodeByKey(data.node.key).remove();
                                tmpnode = channeltree.getNodeByKey(data.node.parent.key);
                                tmpnode.addNode(selnode.toDict(true));
                            }else{
                                tmpnode = channeltree.getNodeByKey(data.node.parent.key);
                                tmpnode.addNode(selnode.toDict(true));
                                if(data.node.key.indexOf("cat")<0){
                                    globalObj._selectcount++;
                                }
                            }
                            break;
                        case 3: //流节点
                            if(channeltree.getNodeByKey(data.node.parent.key) != null){				//	判断是否已存在节目节点
                                channeltree.getNodeByKey(data.node.parent.key).addNode(selnode.toDict(true));
                            }else{
                                channeltree.getNodeByKey(data.node.parent.parent.key).addNode(selnode.getParent().toDict());//添加节目节点
                                channeltree.getNodeByKey(data.node.parent.key).addNode(selnode.toDict(true));
                                if(data.node.key.indexOf("cat")<0){
                                    globalObj._selectcount++;
                                }
                            }
                            break;
                        case 4:
                            //if(channeltree.getNodeByKey(data.node.parent.parent.key) != null){					//	判断是否已存在节目节点
                            //    if(channeltree.getNodeByKey(data.node.parent.key) != null){	//判断是否已存在节目子节点
                            //        channeltree.getNodeByKey(data.node.parent.key).addNode(selnode.toDict(true));
                            //    }else{
                            //        channeltree.getNodeByKey(data.node.parent.parent.key).addNode(selnode.getParent().toDict()); 	//添加节目子节点
                            //        channeltree.getNodeByKey(data.node.parent.key).addNode(selnode.toDict(true));	//添加目标子节点
                            //    }
                            //}else{
                            //    channeltree.getNodeByKey(data.node.key.substring(0,5)).addNode(selnode.getParent().getParent().toDict());//添加节目节点
                            //    _selectcount++;
                            //    channeltree.getNodeByKey(selnode.getParent().getParent().key).addNode(selnode.getParent().toDict()); 	//添加节目子节点
                            //    channeltree.getNodeByKey(selnode.getParent().key).addNode(selnode.toDict(true));	//添加目标子节点
                            //}
                            //count++;
                            break;
                        default:

                            break;
                    }
                }else{
                    switch(arr.length){
                        case 1:	//通道节点
                            tmpnode = channeltree.getNodeByKey(data.node.key);
                            if(tmpnode.key == "id1.0"){
                                $.each(tmpnode.children, function(index,chnode){
                                    while( chnode.hasChildren() ) {
                                        chnode.getFirstChild().remove();
                                        globalObj._selectcount--;
                                    }
                                });

                            }else{
                                while( tmpnode.hasChildren() ) {
                                    tmpnode.getFirstChild().remove();
                                    globalObj._selectcount--;
                                }
                            }
                            break;
                        case 2:	//节目节点
                            tmpnode = channeltree.getNodeByKey(data.node.key);
                            tmpnode.remove();
                            if(data.node.key.indexOf("cat")<0){
                                globalObj._selectcount--;
                            }
                            break;
                        case 3:
                            tmpnode = channeltree.getNodeByKey(data.node.key);
                            var pnode = tmpnode.getParent();
                            tmpnode.remove();
                            if(!pnode.hasChildren() && data.node.key.indexOf("cat")<0){
                                globalObj._selectcount--;
                            }
                            break;
                        //case 4:
                        default:

                            break;
                    }

                }
                prgnode.setTitle(globalObj._nv == "zh-CN"?"节目: ":"Program: "+ globalObj._selectcount);
                prgnode.render();
                checkselectedprg(data.node.key, data.node.selected, selnode);
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
                    $(".menu_pidtrans").css("display", "block");
                    $(".menu_add").css("display", "none");
                    $(".menu_delete").css("display", "none");
                    $(".menu_edit").css("display", "none");
                    if(data.node.key == "id1.0"){
                        $(".menu_re_prg").css("display", "block");
                        $(".menu_re_pid").css("display", "block");
                        $(".menu_deleteall").css("display", "block");
                        $(".menu_prgdeleteall").css("display", "block");
                        $(".menu_prgitems").css("display", "block");
                        $(".menu_chndelete").css("display", "none");
                        $(".menu_prgdeletecus").css("display", "none");
                        $(".menu_cus_add").css("display", "none");
                    }else{
                        $(".menu_deleteall").css("display", "none");
                        $(".menu_prgdeleteall").css("display", "none");
                        $(".menu_re_prg").css("display", "none");
                        $(".menu_re_pid").css("display", "none");
                        $(".menu_chndelete").css("display", "block");
                        $(".menu_prgdeletecus").css("display", "none");
                        $(".menu_cus_add").css("display", "block");
                    }

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
                    $(".menu_chndelete").css("display", "none");
                    $(".menu_prgdeletecus").css("display", "none");
                    $(".menu_cus_add").css("display", "none");
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
                    $(".menu_chndelete").css("display", "none");
                    $(".menu_prgdeletecus").css("display", "none");
                    $(".menu_cus_add").css("display", "none");
				}
			},
			select: function(event, data){
                globalObj._channel = 1;
                var channeltree =  $("#channel").fancytree("getTree");
                if(data.node.data.chnid == 9){
                    globalObj._isuserprg = 0;   //user custom prg
                }else{
                    globalObj._isuserprg = 1;
                }
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
					} case '#delete': { //删除这个节目
						var nodekey = data.node.key;
                        var snode = data.node;
						data.node.remove();
                        globalObj._selectcount--;
                        var prgnode = channeltree.getNodeByKey("id1.0");
                        prgnode.setTitle(globalObj._nv == "zh-CN"?"节目: ":"Program: "+ globalObj._selectcount);
                        prgnode.render();
                        var devnode = $("#devlist").fancytree("getTree").getNodeByKey(nodekey);
                        if(devnode != null){
                            devnode.setSelected(false);
                        }
                        checkselectedprg(nodekey, false, snode);
						break;
					} case '#edit': {
						//获取编辑节目信息
						$.ajax({
							type: "GET",
							async:false,
							url: "http://"+globalObj.localip+":4000/do/programs/getprginfo",
							data:'{channel:' + globalObj._channel + ',chnid:' + data.node.data.chnid + ',index:' + data.node.data.index + '}',
							dataType: "json",
							success: function(data){
                                globalObj._index = data.index;
								$('.prg_name').val(data.prgName);
                                $('.prg_merchant').val(data.providerName);
								$('.prg_no').val(data.prgNum.toString());
								$('.prg_pid').val(data.pmtPid.toString(16));
								$('.prg_prc').val(data.chnId.toString());
								$('.prg_prc1').val(data.oldPcrPid.toString(16));
								$('.prg_prc2').val(data.newPcrPid.toString(16));
								$('#r_servicetype')[0].options[Number(data.serviceType) - 1].selected = true;
								dataSet.length = 0;
								$.each(data.children, function(key, itemv) {
									var item = [itemv.index,itemv.inChn, itemv.streamtype,itemv.inpid.toString(16),itemv.outpid.toString(16)];
									dataSet[dataSet.length] = item;
								});
                                globalObj._tbleditcount = dataSet.length;
								//编辑节目对话框表
								if ( $.fn.dataTable.isDataTable( '#tbl_editprg' ) ) {
                                    $('#tbl_editprg').dataTable().fnClearTable();
                                    $('#tbl_editprg').dataTable().fnAddData(dataSet);
                                }else{
                                    globalObj._tbl_edit = $('#tbl_editprg').dataTable( {
										"data": dataSet,
										"order": [[ 0, "asc" ]],
										"paging":   false,
										"info":     false,
										"searching":   false,
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
                                            $('td:eq(3)', nRow).html( '<input type="text" pattern="(^0x[a-f0-9]{1,4}$)|(^0X[A-F0-9]{1,4}$)|(^[A-F0-9]{1,4}$)|(^[a-f0-9]{1,4}$)" id="r_inpid'+iDisplayIndex+ '" name="r_inpid'+iDisplayIndex+ '" value="'+ aData[3] + '"></input>' );
                                            $('td:eq(4)', nRow).html( '<input type="text" pattern="(^0x[a-f0-9]{1,4}$)|(^0X[A-F0-9]{1,4}$)|(^[A-F0-9]{1,4}$)|(^[a-f0-9]{1,4}$)" id="r_outpid'+iDisplayIndex+ '" name="r_outpid'+iDisplayIndex+ '" value="'+ aData[4] + '"></input>' );
										},		
										"columns": [
											{ "title": globalObj._nv == "zh-CN"?"序号":"NO." },
											{ "title": globalObj._nv == "zh-CN"?"输入通道":"INPUT", "width": "70px"},
											{ "title": globalObj._nv == "zh-CN"?"流类型":"StreamType"},
											{ "title": globalObj._nv == "zh-CN"?"输入PID(Hex)":"IN-PID(Hex)"},
											{ "title": globalObj._nv == "zh-CN"?"输出PID(Hex)":"OUT-PID(Hex)" }
										]
									});
                                    globalObj._tbl_edit.fnDraw();
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
								alert("异常！====="+JSON.stringify(err));    
							}   
						});
                        globalObj._prgoptflag = 0;
						dialog_edit.dialog( "open" );
                        globalObj._editnodekey = data.node.key;
						break;
					} case '#deleteall': {//删除下一级所有自增描述符

						break;
					} case '#chndelete': {//删除此通道下所有节目
                        var nodekey;
                        var snode = data.node;
                        var tmpnode;
                        while( data.node.hasChildren() ) {
                            nodekey = data.node.getFirstChild().key;
                            tmpnode = $("#devlist").fancytree("getTree").getNodeByKey(nodekey);
                            if(tmpnode != null){
                                tmpnode.setSelected(false);
                            }
                            data.node.getFirstChild().remove();
                            globalObj._selectcount--;
                        }
                        var prgnode = $("#channel").fancytree("getTree").getNodeByKey("id1.0");
                        prgnode.setTitle(globalObj._nv == "zh-CN"?"节目: ":"Program: "+ globalObj._selectcount);
                        prgnode.render();
                        checkselectedprg(data.node.key, false, snode);
                        break;
                    } case '#add': {
						dialog_desc.dialog( "open" );
						break;
					} case '#cus_add': {//添加自增节目
                        $('.prg_name').val("");
                        $('.prg_merchant').val("");
                        $('.prg_no').val("1");
                        $('.prg_pid').val("1ff");
                        $('.prg_prc').val("");
                        $('.prg_prc1').val("1ffe");
                        $('.prg_prc2').val("1ffe");
                        $('.prg_prc').attr("disabled", false);
                        //编辑节目对话框表
                        if ( $.fn.dataTable.isDataTable( '#tbl_editprg' ) ) {
                            $('#tbl_editprg').dataTable().fnClearTable();
                        }else{
                            globalObj._tbl_edit = $('#tbl_editprg').dataTable( {
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
                                    $('td:eq(3)', nRow).html( '<input type="text" pattern="(^0x[a-f0-9]{1,4}$)|(^0X[A-F0-9]{1,4}$)|(^[A-F0-9]{1,4}$)|(^[a-f0-9]{1,4}$)" id="r_inpid'+iDisplayIndex+ '" name="r_inpid'+iDisplayIndex+ '" value="'+ aData[3] + '"></input>' );
                                    $('td:eq(4)', nRow).html( '<input type="text" pattern="(^0x[a-f0-9]{1,4}$)|(^0X[A-F0-9]{1,4}$)|(^[A-F0-9]{1,4}$)|(^[a-f0-9]{1,4}$)" id="r_outpid'+iDisplayIndex+ '" name="r_outpid'+iDisplayIndex+ '" value="'+ aData[4] + '"></input>' );
                                },
                                "columns": [
                                    { "title": globalObj._nv == "zh-CN"?"序号":"NO." },
									{ "title": globalObj._nv == "zh-CN"?"输入通道":"INPUT", "width": "70px"},
									{ "title": globalObj._nv == "zh-CN"?"流类型":"StreamType"},
									{ "title": globalObj._nv == "zh-CN"?"输入PID(Hex)":"IN-PID(Hex)"},
									{ "title": globalObj._nv == "zh-CN"?"输出PID(Hex)":"OUT-PID(Hex)" }
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
                        globalObj._prgoptflag = 1;
                        dialog_edit.dialog( "open" );
                        break;
                    } case '#prgdeletecus': {//删除所有自增节目
                        var snode = data.node;
                        var tmpnode;
                        $.each(data.node.children, function(index,chnode){
                            if(chnode.children != null){
                                var tmpnodes = chnode.children;
                                $.each(tmpnodes.children, function(ig,prgnode){
                                    if(prgnode.extraClasses == "userprg" ){
                                        chnode.children[ig].remove();
                                        globalObj._selectcount--;
                                    }
                                });
                            }
                        });
                        var prgnode = $("#channel").fancytree("getTree").getNodeByKey("id1.0");
                        prgnode.setTitle(globalObj._nv == "zh-CN"?"节目: ":"Program: "+ globalObj._selectcount);
                        prgnode.render();
                        checkselectedprg(data.node.key, false, snode);
                        break;
                    } case '#prgdeleteall': {//删除所有节目
                        var snode = data.node;
                        var tmpnode;
                        $.each(data.node.children, function(index,chnode){
                            if(chnode.children != null){
                                while(chnode.hasChildren()){
                                    tmpnode = $("#devlist").fancytree("getTree").getNodeByKey(chnode.getFirstChild().key);
                                    if(tmpnode != null){
                                        tmpnode.setSelected(false);
                                    }
                                    chnode.getFirstChild().remove();
                                    globalObj._selectcount--;
                                }
                            }
                        });
                        var prgnode = $("#channel").fancytree("getTree").getNodeByKey("id1.0");
                        prgnode.setTitle(globalObj._nv == "zh-CN"?"节目: ":"Program: "+ globalObj._selectcount);
                        prgnode.render();
                        checkselectedprg(data.node.key, false, snode);
						break;
					} case '#itmes': {
						//获取输出通道设置信息
						$.ajax({
							 type: "GET",
							 async:false,
							 url: "http://"+globalObj.localip+":4000/do/programs/getchanneloutinfo?channel="+globalObj._channel,
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
								alert("异常！====="+JSON.stringify(err));    
							 }   
						});						
						break;
					} case '#re_prg': {
                        $.ajax({
                            type: "GET",
                            async:false,
                            url: "http://"+globalObj.localip+":4000/do/programs/reprgnum?inch="+globalObj._channel,
                            dataType: "json",
                            success: function(data){
                                if(data.sts == 1){
                                    //获取输出通道信息
                                    readoutprgs(channeltree, globalObj._selectcount);
                                }else if(data.sts == 5){
                                    alert(globalObj._nv == "zh-CN"?"该用户权限不足.":"Permission Denied!");
                                    return;
                                }
                            },
                            error : function(err) {
                                alert("异常！====="+JSON.stringify(err));
                            }
                        });
                    break;
					} case '#re_pid': {
                    $.ajax({
                        type: "GET",
                        async:false,
                        url: "http://"+globalObj.localip+":4000/do/programs/reprgpid?inch="+globalObj._channel,
                        dataType: "json",
                        success: function(data){
                            if(data.sts == 1){
                                //获取输出通道信息
                                readoutprgs(channeltree, globalObj._selectcount);
                            }else if(data.sts == 5){
                                alert(globalObj._nv == "zh-CN"?"该用户权限不足.":"Permission Denied!");
                                return;
                            }
                        },
                        error : function(err) {
                            alert("异常！====="+JSON.stringify(err));
                        }
                    });
						break;
					} case '#pidtrans': {
						//获取pid透传表信息
						$.ajax({
							 type: "GET",
							 async:false,
							 url: "http://"+globalObj.localip+":4000/do/programs/getpidtransinfo?channel="+globalObj._channel,
							 dataType: "json",
							 success: function(data){
								var pidData = [];
								if(data.cnt != 0){
									$.each(data.children, function(key, itemv) {
										var item = [itemv.NO,itemv.ch, itemv.oldPid.toString(16),itemv.newPid.toString(16)];
										pidData[pidData.length] = item;					
									});
								}
								//PID表
								globalObj._tbl_pid = $('#tbl_pid').dataTable( {
									"data": pidData,
									"order": [[ 0, "asc" ]],
									"bDestroy" : true,
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
										{ "title": globalObj._nv == "zh-CN"?"序号":"NO.", "width": "60px" },
										{ "title": globalObj._nv == "zh-CN"?"通道":"CH"},
										{ "title": globalObj._nv == "zh-CN"?"输入PID(Hex)":"IN-PID(Hex)"},
										{ "title": globalObj._nv == "zh-CN"?"输出PID(Hex)":"OUT-PID(Hex)" }
									]
								});
								$('#tbl_pid').dataTable().fnDraw();
								$('#tbl_pid tbody').on( 'click', 'tr', function () {
									if ( $(this).hasClass('selected') ) {
										$(this).removeClass('selected');
									}else {
										$('#tbl_pid').DataTable().$('tr.selected').removeClass('selected');
										$(this).addClass('selected');
									}
								} );
								
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
			if( $.ui.fancytree.getEventTargetType(event) === "title" ){
			  data.node.toggleSelected();
			}
		}
	});	
	
	//表结构树
	$("#out_tree").fancytree({
		extensions: ["menu"],
		selectMode: 1,
		minExpandLevel:2,
		source: table_root,
		menu: {
			selector: "#table_menu",
			position: {my: "center"},
            beforeOpen: function(event, data){
                var arr = data.node.key.match(/\./g);
                if(arr.length == 1 && data.node.key == "id1.5") {
                    $(".menu_collasp").css("display", "block");
                    $(".menu_expandall").css("display", "block");
                    $(".menu_add").css("display", "block"); //添加段
                    $(".menu_deleteall").css("display", "block"); //刪除所有段
                    $(".menu_delete").css("display", "none"); //刪除段
                    $(".menu_edit").css("display", "none"); //编辑段
                    $(".menu_deleteallstr").css("display", "none"); //删除所有传输流
                    $(".menu_addstrc").css("display", "none"); //添加传输流[DVB-C Style]
                    $(".menu_addstrs").css("display", "none"); //添加传输流[DVB-S Style]
                    $(".menu_addstrt").css("display", "none"); //添加传输流[DVB-T Style]
                    $(".menu_import").css("display", "block"); //导入NIT
                    $(".menu_export").css("display", "block"); //导出NIT
                    $(".menu_editstr").css("display", "none"); //编辑传输流
                    $(".menu_deletestr").css("display", "none"); //删除传输流
                    $(".menu_hr").css("display", "block");
                }else if(arr.length == 2 && data.node.key == "id1.5.1"){
                    $(".menu_collasp").css("display", "block");
                    $(".menu_expandall").css("display", "block");
                    $(".menu_add").css("display", "none"); //添加段
                    $(".menu_deleteall").css("display", "none"); //刪除所有段
                    $(".menu_delete").css("display", "block"); //刪除段
                    $(".menu_edit").css("display", "block"); //编辑段
                    $(".menu_deleteallstr").css("display", "block"); //删除所有传输流
                    $(".menu_addstrc").css("display", "block"); //添加传输流[DVB-C Style]
                    $(".menu_addstrs").css("display", "block"); //添加传输流[DVB-S Style]
                    $(".menu_addstrt").css("display", "block"); //添加传输流[DVB-T Style]
                    $(".menu_import").css("display", "none"); //导入NIT
                    $(".menu_export").css("display", "none"); //导出NIT
                    $(".menu_editstr").css("display", "none"); //编辑传输流
                    $(".menu_deletestr").css("display", "none"); //删除传输流
                    $(".menu_hr").css("display", "block");
                }else if(arr.length == 3 && data.node.key.substr(0,5) == "id1.5"){
                    $(".menu_collasp").css("display", "none");
                    $(".menu_expandall").css("display", "none");
                    $(".menu_add").css("display", "none"); //添加段
                    $(".menu_deleteall").css("display", "none"); //刪除所有段
                    $(".menu_delete").css("display", "none"); //刪除段
                    $(".menu_edit").css("display", "none"); //编辑段
                    $(".menu_deleteallstr").css("display", "none"); //删除所有传输流
                    $(".menu_addstrc").css("display", "none"); //添加传输流[DVB-C Style]
                    $(".menu_addstrs").css("display", "none"); //添加传输流[DVB-S Style]
                    $(".menu_addstrt").css("display", "none"); //添加传输流[DVB-T Style]
                    $(".menu_import").css("display", "none"); //导入NIT
                    $(".menu_export").css("display", "none"); //导出NIT
                    $(".menu_editstr").css("display", "block"); //编辑传输流
                    $(".menu_deletestr").css("display", "block"); //删除传输流
                    $(".menu_hr").css("display", "none");
                }else{
                    $("li[class^=menu_]").css("display", "none");
                }
            },
			select: function(event, data){
                globalObj._channel = 1;
				switch(data.menuId){
					case '#expandall' :{
						var nodes = data.node.children;
						data.node.setExpanded(true);
						$.each(nodes, function(index,item){
							item.setExpanded(true);
						});
						break;
					} case '#collasp': {
						data.node.setExpanded(false);
						break;
					} case '#add': {
                        $('.nit_name').val("");
                        $('.nit_id').val("");
                        globalObj._prgoptflag = 1;
						dialog_NIT.dialog( "open" );
						break;
					} case '#edit': {
                        $.ajax({
                            type: "GET",
                            async:false,
                            url: "http://"+globalObj.localip+":4000/do/nitController/getsection?channel=1",
                            //data: ,
                            dataType: "json",
                            success: function(data){
                                if(data.sts == 5){
                                   alert("权限不足，请与管理员联系");
                                }else if(data.sts == 9){
                                    window.location = "/login.esp";
                                    return;
                                }
                                $('.nit_name').val(data.name);
                                $('.nit_id').val(data.streamid.toString(16));
                                globalObj._prgoptflag = 0;
                                dialog_NIT.dialog( "open" );
                            },
                            error : function(err) {
                            }
                        });
                        break;
                    } case '#delete': {
                        $.ajax({
                            type: "GET",
                            async:false,
                            url: "http://"+globalObj.localip+":4000/do/nitController/delnit?channel=1",
                            //data: ,
                            dataType: "json",
                            success: function(data){
                                if(data.sts == 0){
                                    dialog_NIT.dialog( "close" );
                                    return;
                                }else if(data.sts == 5){
                                    alert(globalObj._nv == "zh-CN"?"该用户权限不足.":"Permission Denied!");
                                }else if(data.sts == 9){
                                    window.location = "/login.esp";
                                    return;
                                }
                            },
                            error : function(err) {
                            }
                        });
                        data.node.remove();
						break;
					} case '#deleteallstr':{
                        $.ajax({
                            type: "GET",
                            async:false,
                            url: "http://"+globalObj.localip+":4000/do/nitController/delallstr?channel=1",
                            //data: ,
                            dataType: "json",
                            success: function(res){
                                if(res.sts == 0){
                                    dialog_NIT.dialog( "close" );
                                    return;
                                }else if(res.sts == 5){
                                    alert(globalObj._nv == "zh-CN"?"该用户权限不足.":"Permission Denied!");
                                }else if(res.sts == 9){
                                    window.location = "/login.esp";
                                    return;
                                }
                                while(data.node.hasChildren()){
                                    data.node.getFirstChild().remove();
                                }
                            },
                            error : function(err) {
                            }
                        });
                        break;
                    } case '#addstrc':{
                        addstrc();
                        globalObj._prgoptflag = 1;
                        globalObj._editnodekey = data.node.key;
                        dialog_nitc.dialog( "open" );
                        break;
                    } case '#addstrs':{
                        addstrs();
                        globalObj._prgoptflag = 1;
                        globalObj._editnodekey = data.node.key;
                        dialog_nits.dialog( "open" );
                        break;
                    }  case '#addstrt':{
                        addstrt();
                        globalObj._prgoptflag = 1;
                        globalObj._editnodekey = data.node.key;
                        dialog_nitt.dialog( "open" );
                        break;
                    } case '#editstr': {
                        $.ajax({
                            type: "GET",
                            async:false,
                            url: "http://"+globalObj.localip+":4000/do/nitController/getstream?channel=1&streamid="+ data.node.data.streamid,
                            //data: ,
                            dataType: "json",
                            success: function(res){
                                if(res.sts == 0){
                                    alert(globalObj._nv == "zh-CN"?"读取流错误!":"Read Stream Error!");
                                    dialog_NIT.dialog( "close" );
                                    return;
                                }else if(res.sts == 5){
                                    alert(globalObj._nv == "zh-CN"?"该用户权限不足.":"Permission Denied!");
                                }else if(res.sts == 9){
                                    window.location = "/login.esp";
                                    return;
                                }
                                switch(data.node.data.type){
                                    case 'DVB-C':
                                        editnitc(data, res);
                                        globalObj._prgoptflag = 0;
                                        globalObj._editnodekey = data.node.key;
                                        dialog_nitc.dialog( "open" );
                                        break;
                                    case 'DVB-S':
                                        editnits(data, res);
                                        globalObj._prgoptflag = 0;
                                        globalObj._editnodekey = data.node.key;
                                        dialog_nits.dialog( "open" );
                                        break;
                                    case 'DVB-T':
                                        editnitt(data, res);
                                        globalObj._prgoptflag = 0;
                                        globalObj._editnodekey = data.node.key;
                                        dialog_nitt.dialog( "open" );
                                        break;
                                }
                            },
                            error : function(err) {
                            }
                        });
                        break;
                    } case '#deletestr': {
                        var oristreamid = data.node.data.streamid;
                        $.ajax({
                            type: "GET",
                            async:false,
                            url: "http://"+globalObj.localip+":4000/do/nitController/delstr?channel=1&streamid="+oristreamid,
                            //data: ,
                            dataType: "json",
                            success: function(data){
                                if(data.sts == 0){
                                    dialog_NIT.dialog( "close" );
                                    return;
                                }else if(data.sts == 5){
                                    alert(globalObj._nv == "zh-CN"?"该用户权限不足.":"Permission Denied!");
                                }else if(data.sts == 9){
                                    window.location = "/login.esp";
                                    return;
                                }
                            },
                            error : function(err) {
                            }
                        });
                        data.node.remove();
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
                    $(".menu_search").css("display", "none");
				}else if(data.node.key.length == 5){
					$(".menu_delete").css("display", "block");
                    $(".menu_search").css("display", "block");
				}			    
			},
			select: function(event, data){
                globalObj._channel = 2;
				switch(data.menuId){
					case '#expandall' :{
						var nodes = data.node.children;
						data.node.setExpanded(true);
						$.each(nodes, function(index,item){
							item.setExpanded(true);
						});
						break;
					} case '#collasp': {
						data.node.setExpanded(false);
						break;
					} case '#delete': { //删除此通道下所有节目
						while( data.node.hasChildren() ) {
                            if(data.node.getFirstChild().isSelected()){
                                data.node.getFirstChild().setSelected(false);
                            }
							data.node.getFirstChild().remove();
						}
						break;
					} case '#search': {
                        dig_notification.dialog( "open" );
                        $.ajax({
                            type: "GET",
                            async: false,
                            url: "http://"+globalObj.localip+":4000/do/programs/search?inch="+data.node.key.substring(data.node.key.indexOf('.')+1),
                            dataType: "json",
                            success: function(res){
                                $.ajax({
                                    type: "GET",
                                    async:false,
                                    url: "http://"+globalObj.localip+":4000/do/programs/getprg?inch="+data.node.key.substring(data.node.key.indexOf('.')+1),
                                    // data: {ip:"192.168.1.134", inch:2},
                                    dataType: "json",
                                    success: function(resp){
                                        data.node.removeChildren();
                                        resp = JSON.stringify(resp).replace('\\','');
                                        var treeData1 = JSON.parse(resp);
                                        $.each(treeData1[0].children, function(index,item){
                                            data.node.addChildren(item);
                                        });
                                        readoutprgs($("#channel2").fancytree("getTree"),0);
                                        createTable();
                                        dig_notification.dialog( "close" );
                                    },
                                    error : function(err) {
                                        alert("异常！====="+JSON.stringify(err));
                                    }
                                });
                            },
                            error : function(err) {
                                alert("异常！====="+JSON.stringify(err));
                            }
                        });
                        break;
                    }default: {
						alert("Menu select " + data.menuId + ", " + data.node);
						break;
					}			  
				}
			}
		},
		select: function(event, data) {

		},
		click: function(event, data) {			
			if( $.ui.fancytree.getEventTargetType(event) === "title" ){
			  data.node.toggleSelected();
			}
            if(data.targetType == "checkbox"){
                data.node.toggleSelected();
                globalObj._channel = 2;
                var tmpnode;
                var channeltree =  $("#channel2").fancytree("getTree");
                //删除节目树节点
                var prgnode = channeltree.getNodeByKey("id1.0");

                //添加至节目树
                var selnode = data.tree.getNodeByKey(data.node.key);
                var arr = data.node.key.match(/\./g);
                if(data.node.selected){
                    switch(arr.length){
                        case 1:	//通道节点
                            if(data.node.key == "id1.0"){
                                $.each(data.node.children, function(index,chnode){
                                    if(chnode.children != null){
                                        $.each(chnode.children, function(index,prgnode){
                                            if(channeltree.getNodeByKey(prgnode.key) != null){
                                                channeltree.getNodeByKey(prgnode.key).remove();
                                                tmpnode = channeltree.getNodeByKey(prgnode.parent.key);
                                                tmpnode.addNode(prgnode.toDict(true));
                                            }else{
                                                tmpnode = channeltree.getNodeByKey(prgnode.parent.key);
                                                tmpnode.addNode(prgnode.toDict(true));
                                                globalObj._selectcount2++;
                                            }
                                        });
                                    }
                                });
                            }else{
                                $.each(data.node.children, function(index,item){
                                    if(channeltree.getNodeByKey(item.key) != null){
                                        channeltree.getNodeByKey(item.key).remove();
                                        tmpnode = channeltree.getNodeByKey(item.parent.key);
                                        tmpnode.addNode(item.toDict(true));
                                    }else{
                                        tmpnode = channeltree.getNodeByKey(item.parent.key);
                                        tmpnode.addNode(item.toDict(true));
                                        globalObj._selectcount2++;
                                    }
                                });
                            }
                            break;
                        case 2:	//节目节点
                            if(channeltree.getNodeByKey(data.node.key) != null){
                                channeltree.getNodeByKey(data.node.key).remove();
                                tmpnode = channeltree.getNodeByKey(data.node.parent.key);
                                tmpnode.addNode(selnode.toDict(true));
                            }else{
                                tmpnode = channeltree.getNodeByKey(data.node.parent.key);
                                tmpnode.addNode(selnode.toDict(true));
                                globalObj._selectcount2++;
                            }
                            break;
                        case 3: //流节点
                            if(channeltree.getNodeByKey(data.node.parent.key) != null){				//	判断是否已存在节目节点
                                channeltree.getNodeByKey(data.node.parent.key).addNode(selnode.toDict(true));
                            }else{
                                channeltree.getNodeByKey(data.node.parent.parent.key).addNode(selnode.getParent().toDict());//添加节目节点
                                channeltree.getNodeByKey(data.node.parent.key).addNode(selnode.toDict(true));
                                globalObj._selectcount2++;
                            }
                            break;
                        case 4:
                            //if(channeltree.getNodeByKey(data.node.parent.parent.key) != null){					//	判断是否已存在节目节点
                            //    if(channeltree.getNodeByKey(data.node.parent.key) != null){	//判断是否已存在节目子节点
                            //        channeltree.getNodeByKey(data.node.parent.key).addNode(selnode.toDict(true));
                            //    }else{
                            //        channeltree.getNodeByKey(data.node.parent.parent.key).addNode(selnode.getParent().toDict()); 	//添加节目子节点
                            //        channeltree.getNodeByKey(data.node.parent.key).addNode(selnode.toDict(true));	//添加目标子节点
                            //    }
                            //}else{
                            //    channeltree.getNodeByKey(data.node.key.substring(0,5)).addNode(selnode.getParent().getParent().toDict());//添加节目节点
                            //    _selectcount2++;
                            //    channeltree.getNodeByKey(selnode.getParent().getParent().key).addNode(selnode.getParent().toDict()); 	//添加节目子节点
                            //    channeltree.getNodeByKey(selnode.getParent().key).addNode(selnode.toDict(true));	//添加目标子节点
                            //}
                            //count++;
                            break;
                        default:

                            break;
                    }
                }else{
                    switch(arr.length){
                        case 1:	//通道节点
                            tmpnode = channeltree.getNodeByKey(data.node.key);
                            if(tmpnode.key == "id1.0"){
                                $.each(tmpnode.children, function(index,chnode){
                                    while( chnode.hasChildren() ) {
                                        chnode.getFirstChild().remove();
                                        globalObj._selectcount2--;
                                    }
                                });

                            }else{
                                while( tmpnode.hasChildren() ) {
                                    tmpnode.getFirstChild().remove();
                                    globalObj._selectcount2--;
                                }
                            }
                            break;
                        case 2:	//节目节点
                            tmpnode = channeltree.getNodeByKey(data.node.key);
                            tmpnode.remove();
                            globalObj._selectcount2--;
                            break;
                        case 3:
                            tmpnode = channeltree.getNodeByKey(data.node.key);
                            tmpnode.remove();
                            break;
                        //case 4:

                        default:

                            break;
                    }

                }
                prgnode.setTitle(globalObj._nv == "zh-CN"?"节目: ":"Program: "+ globalObj._selectcount2);
                prgnode.render();
                checkselectedprg(data.node.key, data.node.selected, data.node);
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
                    $(".menu_pidtrans").css("display", "block");
                    $(".menu_add").css("display", "none");
                    $(".menu_delete").css("display", "none");
                    $(".menu_edit").css("display", "none");
                    if(data.node.key == "id1.0"){
                        $(".menu_re_prg").css("display", "block");
                        $(".menu_re_pid").css("display", "block");
                        $(".menu_deleteall").css("display", "block");
                        $(".menu_prgitems").css("display", "block");
                        $(".menu_prgdeleteall").css("display", "block");
                        $(".menu_chndelete").css("display", "none");
                        $(".menu_prgdeletecus").css("display", "none");
                        $(".menu_cus_add").css("display", "none");
                    }else{
                        $(".menu_deleteall").css("display", "none");
                        $(".menu_prgdeleteall").css("display", "none");
                        $(".menu_re_prg").css("display", "none");
                        $(".menu_re_pid").css("display", "none");
                        $(".menu_chndelete").css("display", "block");
                        $(".menu_prgdeletecus").css("display", "none");
                        $(".menu_cus_add").css("display", "block");
                    }
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
                    $(".menu_chndelete").css("display", "none");
                    $(".menu_prgdeletecus").css("display", "none");
                    $(".menu_cus_add").css("display", "none");
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
                    $(".menu_chndelete").css("display", "none");
                    $(".menu_prgdeletecus").css("display", "none");
                    $(".menu_cus_add").css("display", "none");
                }
			    
			},
			select: function(event, data){
                globalObj._channel = 2;
                if(data.node.data.chnid == 9){
                    globalObj._isuserprg = 0;   //user custom prg
                }else{
                    globalObj._isuserprg = 1;
                }
                var channeltree =  $("#channel2").fancytree("getTree");
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
                        var snode = data.node;
                        data.node.remove();
                        globalObj._selectcount2--;
                    var prgnode = $("#channel2").fancytree("getTree").getNodeByKey("id1.0");
                        prgnode.setTitle(globalObj._nv == "zh-CN"?"节目: ":"Program: "+ globalObj._selectcount2);
                        prgnode.render();
                        checkselectedprg(nodekey, false, snode);
						break;
					} case '#edit': {
						//获取编辑节目信息
						$.ajax({
							type: "GET",
							async:false,
							url: "http://"+globalObj.localip+":4000/do/programs/getprginfo",
                            data:'{channel:' + globalObj._channel + ',chnid:' + data.node.data.chnid + ',index:' + data.node.data.index + '}',
							dataType: "json",
							success: function(data){
                                globalObj._index = data.index;
								$('.prg_name').val(data.prgName);
                                $('.prg_merchant').val(data.providerName);
								$('.prg_no').val(data.prgNum.toString());
								$('.prg_pid').val(data.pmtPid.toString(16));
								$('.prg_prc').val(data.chnId.toString());
								$('.prg_prc1').val(data.oldPcrPid.toString(16));
								$('.prg_prc2').val(data.newPcrPid.toString(16));
								dataSet.length = 0;	
								$.each(data.children, function(key, itemv) {
									var item = [itemv.index,itemv.inChn, itemv.streamtype,itemv.inpid.toString(16),itemv.outpid.toString(16)];
									dataSet[dataSet.length] = item;
								});
                                globalObj._tbleditcount = dataSet.length;
								//编辑节目对话框表
								if ( $.fn.dataTable.isDataTable( '#tbl_editprg' ) ) {									
									$('#tbl_editprg').dataTable().fnClearTable();
									$('#tbl_editprg').dataTable().fnAddData(dataSet);
								}else{
                                    globalObj._tbl_edit = $('#tbl_editprg').dataTable( {
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
											$('td:eq(3)', nRow).html( '<input type="text" pattern="(^0x[a-f0-9]{1,4}$)|(^0X[A-F0-9]{1,4}$)|(^[A-F0-9]{1,4}$)|(^[a-f0-9]{1,4}$)" id="r_inpid'+iDisplayIndex+ '" name="r_inpid'+iDisplayIndex+ '" value="'+ aData[3] + '"></input>' );
											$('td:eq(4)', nRow).html( '<input type="text" pattern="(^0x[a-f0-9]{1,4}$)|(^0X[A-F0-9]{1,4}$)|(^[A-F0-9]{1,4}$)|(^[a-f0-9]{1,4}$)" id="r_outpid'+iDisplayIndex+ '" name="r_outpid'+iDisplayIndex+ '" value="'+ aData[4] + '"></input>' );
											
										},		
										"columns": [
											{ "title": globalObj._nv == "zh-CN"?"序号":"NO." },
											{ "title": globalObj._nv == "zh-CN"?"输入通道":"INPUT", "width": "70px"},
											{ "title": globalObj._nv == "zh-CN"?"流类型":"StreamType"},
											{ "title": globalObj._nv == "zh-CN"?"输入PID(Hex)":"IN-PID(Hex)"},
											{ "title": globalObj._nv == "zh-CN"?"输出PID(Hex)":"OUT-PID(Hex)" }
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
                        globalObj._prgoptflag = 0;
                        globalObj._editnodekey = data.node.key;
						dialog_edit.dialog( "open" );
						break;
					} case '#deleteall': {
						
						break;
					} case '#chndelete': {
                        var nodekey;
                        var snode = data.node;
                        var tmpnode;
                        while( data.node.hasChildren() ) {
                            nodekey = data.node.getFirstChild().key;
                            tmpnode = $("#devlist").fancytree("getTree").getNodeByKey(nodekey);
                            if(tmpnode != null){
                                tmpnode.setSelected(false);
                            }
                            data.node.getFirstChild().remove();
                            globalObj._selectcount2--;
                        }
                        var prgnode = $("#channel2").fancytree("getTree").getNodeByKey("id1.0");
                        prgnode.setTitle(globalObj._nv == "zh-CN"?"节目: ":"Program: "+ globalObj._selectcount2);
                        prgnode.render();
                        checkselectedprg(data.node.key, false, snode);
                        break;
                    } case '#add': {
						dialog_desc.dialog( "open" );
						break;
					} case '#cus_add': {//添加自增节目
                        $('.prg_name').val("");
                        $('.prg_merchant').val("");
                        $('.prg_no').val("1");
                        $('.prg_pid').val("1ff");
                        $('.prg_prc').val("");
                        $('.prg_prc1').val("1ffe");
                        $('.prg_prc2').val("1ffe");
                        $('.prg_prc').attr("disabled", false);
                        //编辑节目对话框表
                        if ( $.fn.dataTable.isDataTable( '#tbl_editprg' ) ) {
                            $('#tbl_editprg').dataTable().fnClearTable();
                        }else{
                            globalObj._tbl_edit = $('#tbl_editprg').dataTable( {
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
                                    $('td:eq(3)', nRow).html( '<input type="text" pattern="(^0x[a-f0-9]{1,4}$)|(^0X[A-F0-9]{1,4}$)|(^[A-F0-9]{1,4}$)|(^[a-f0-9]{1,4}$)" id="r_inpid'+iDisplayIndex+ '" name="r_inpid'+iDisplayIndex+ '" value="'+ aData[3] + '"></input>' );
                                    $('td:eq(4)', nRow).html( '<input type="text" pattern="(^0x[a-f0-9]{1,4}$)|(^0X[A-F0-9]{1,4}$)|(^[A-F0-9]{1,4}$)|(^[a-f0-9]{1,4}$)" id="r_outpid'+iDisplayIndex+ '" name="r_outpid'+iDisplayIndex+ '" value="'+ aData[4] + '"></input>' );
                                },
                                "columns": [
                                    { "title": globalObj._nv == "zh-CN"?"序号":"NO." },
									{ "title": globalObj._nv == "zh-CN"?"输入通道":"INPUT", "width": "70px"},
									{ "title": globalObj._nv == "zh-CN"?"流类型":"StreamType"},
									{ "title": globalObj._nv == "zh-CN"?"输入PID(Hex)":"IN-PID(Hex)"},
									{ "title": globalObj._nv == "zh-CN"?"输出PID(Hex)":"OUT-PID(Hex)" }
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
                        globalObj._prgoptflag = 1;
                        dialog_edit.dialog( "open" );
                        break;
                    } case '#prgdeletecus': {//删除自增节目
                        var snode = data.node;
                        var tmpnode;
                        $.each(data.node.children, function(index,chnode){
                            if(chnode.children != null){
                                var tmpnodes = chnode.children;
                                $.each(tmpnodes.children, function(ig,prgnode){
                                    if(prgnode.extraClasses == "userprg" ){
                                        chnode.children[ig].remove();
                                        globalObj._selectcount2--;
                                    }
                                });
                            }
                        });
                        var prgnode = $("#channel2").fancytree("getTree").getNodeByKey("id1.0");
                        prgnode.setTitle(globalObj._nv == "zh-CN"?"节目: ":"Program: "+ globalObj._selectcount2);
                        prgnode.render();
                        checkselectedprg(data.node.key, false, snode);
                        break;
                    }  case '#prgdeleteall': {
                        var snode = data.node;
                        var tmpnode;
                        $.each(data.node.children, function(index,chnode){
                            if(chnode.children != null){
                                while(chnode.hasChildren()){
                                    tmpnode = $("#devlist").fancytree("getTree").getNodeByKey(chnode.getFirstChild().key);
                                    if(tmpnode != null){
                                        tmpnode.setSelected(false);
                                    }
                                    chnode.getFirstChild().remove();
                                    globalObj._selectcount2--;
                                }
                            }
                        });
                        var prgnode = $("#channel2").fancytree("getTree").getNodeByKey("id1.0");
                        prgnode.setTitle(globalObj._nv == "zh-CN"?"节目: ":"Program: "+ globalObj._selectcount2);
                        prgnode.render();
                        checkselectedprg(data.node.key, false, snode);
						break;
					} case '#itmes': {
                        //获取输出通道设置信息
                        $.ajax({
                            type: "GET",
                            async:false,
                            url: "http://"+globalObj.localip+":4000/do/programs/getchanneloutinfo?channel="+globalObj._channel,
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
                                $('#tag_channel')[0].textContent = 2;

                                dig_itmes.dialog( "open" );
                            }
                        });
						break;
					} case '#re_prg': {
                    $.ajax({
                        type: "GET",
                        async:false,
                        url: "http://"+globalObj.localip+":4000/do/programs/reprgnum?inch="+globalObj._channel,
                        dataType: "json",
                        success: function(data){
                            if(data.sts == 1){
                                //获取输出通道信息
                                readoutprgs(channeltree, globalObj._selectcount2);
                            }else if(data.sts == 5){
                                alert(globalObj._nv == "zh-CN"?"该用户权限不足.":"Permission Denied!");
                                return;
                            }
                        },
                        error : function(err) {
                            alert("异常！====="+JSON.stringify(err));
                        }
                    });
                    break;
                } case '#re_pid': {
                    $.ajax({
                        type: "GET",
                        async:false,
                        url: "http://"+globalObj.localip+":4000/do/programs/reprgpid?inch="+globalObj._channel,
                        dataType: "json",
                        success: function(data){
                            if(data.sts == 1){
                                //获取输出通道信息
                                readoutprgs(channeltree, globalObj._selectcount2);
                            }else if(data.sts == 5){
                                alert(globalObj._nv == "zh-CN"?"该用户权限不足.":"Permission Denied!");
                                return;
                            }
                        },
                        error : function(err) {
                            alert("异常！====="+JSON.stringify(err));
                        }
                    });
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
            beforeOpen: function(event, data){
                var arr = data.node.key.match(/\./g);
                if(arr.length == 1 && data.node.key == "id1.5") {
                    $(".menu_collasp").css("display", "block");
                    $(".menu_expandall").css("display", "block");
                    $(".menu_add").css("display", "block"); //添加段
                    $(".menu_deleteall").css("display", "block"); //刪除所有段
                    $(".menu_delete").css("display", "none"); //刪除段
                    $(".menu_edit").css("display", "none"); //编辑段
                    $(".menu_deleteallstr").css("display", "none"); //删除所有传输流
                    $(".menu_addstrc").css("display", "none"); //添加传输流[DVB-C Style]
                    $(".menu_addstrs").css("display", "none"); //添加传输流[DVB-S Style]
                    $(".menu_addstrt").css("display", "none"); //添加传输流[DVB-T Style]
                    $(".menu_import").css("display", "block"); //导入NIT
                    $(".menu_export").css("display", "block"); //导出NIT
                    $(".menu_editstr").css("display", "none"); //编辑传输流
                    $(".menu_deletestr").css("display", "none"); //删除传输流
                    $(".menu_hr").css("display", "block");
                }else if(arr.length == 2 && data.node.key == "id1.5.1"){
                    $(".menu_collasp").css("display", "block");
                    $(".menu_expandall").css("display", "block");
                    $(".menu_add").css("display", "none"); //添加段
                    $(".menu_deleteall").css("display", "none"); //刪除所有段
                    $(".menu_delete").css("display", "block"); //刪除段
                    $(".menu_edit").css("display", "block"); //编辑段
                    $(".menu_deleteallstr").css("display", "block"); //删除所有传输流
                    $(".menu_addstrc").css("display", "block"); //添加传输流[DVB-C Style]
                    $(".menu_addstrs").css("display", "block"); //添加传输流[DVB-S Style]
                    $(".menu_addstrt").css("display", "block"); //添加传输流[DVB-T Style]
                    $(".menu_import").css("display", "none"); //导入NIT
                    $(".menu_export").css("display", "none"); //导出NIT
                    $(".menu_editstr").css("display", "none"); //编辑传输流
                    $(".menu_deletestr").css("display", "none"); //删除传输流
                    $(".menu_hr").css("display", "block");
                }else if(arr.length == 3 && data.node.key.substr(0,5) == "id1.5"){
                    $(".menu_collasp").css("display", "none");
                    $(".menu_expandall").css("display", "none");
                    $(".menu_add").css("display", "none"); //添加段
                    $(".menu_deleteall").css("display", "none"); //刪除所有段
                    $(".menu_delete").css("display", "none"); //刪除段
                    $(".menu_edit").css("display", "none"); //编辑段
                    $(".menu_deleteallstr").css("display", "none"); //删除所有传输流
                    $(".menu_addstrc").css("display", "none"); //添加传输流[DVB-C Style]
                    $(".menu_addstrs").css("display", "none"); //添加传输流[DVB-S Style]
                    $(".menu_addstrt").css("display", "none"); //添加传输流[DVB-T Style]
                    $(".menu_import").css("display", "none"); //导入NIT
                    $(".menu_export").css("display", "none"); //导出NIT
                    $(".menu_editstr").css("display", "block"); //编辑传输流
                    $(".menu_deletestr").css("display", "block"); //删除传输流
                    $(".menu_hr").css("display", "none");
                }else{
                    $("li[class^=menu_]").css("display", "none");
                }
            },
            select: function(event, data){
                globalObj._channel = 2;
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
                    $('.nit_name').val("");
                    $('.nit_id').val("");
                    globalObj._prgoptflag = 1;
                    dialog_NIT.dialog( "open" );
                    break;
                } case '#edit': {
                    $.ajax({
                        type: "GET",
                        async:false,
                        url: "http://"+globalObj.localip+":4000/do/nitController/getsection?channel=2",
                        //data: ,
                        dataType: "json",
                        success: function(data){
                            if(data.sts == 5){
                                alert(globalObj._nv == "zh-CN"?"该用户权限不足.":"Permission Denied!");
                            }else if(data.sts == 9){
                                window.location = "/login.esp";
                                return;
                            }
                            $('.nit_name').val(data.name);
                            $('.nit_id').val(data.streamid.toString(16));
                            globalObj._prgoptflag = 0;
                            dialog_NIT.dialog( "open" );
                        },
                        error : function(err) {
                        }
                    });
                    break;
                } case '#delete': {
                    $.ajax({
                        type: "GET",
                        async:false,
                        url: "http://"+globalObj.localip+":4000/do/nitController/delnit?channel=2",
                        //data: ,
                        dataType: "json",
                        success: function(data){
                            if(data.sts == 0){
                                dialog_NIT.dialog( "close" );
                                return;
                            }else if(data.sts == 5){
                                alert(globalObj._nv == "zh-CN"?"该用户权限不足.":"Permission Denied!");
                            }else if(data.sts == 9){
                                window.location = "/login.esp";
                                return;
                            }
                        },
                        error : function(err) {
                        }
                    });
                    data.node.remove();
                    break;
                } case '#deleteallstr':{
                    $.ajax({
                        type: "GET",
                        async:false,
                        url: "http://"+globalObj.localip+":4000/do/nitController/delallstr?channel=2",
                        //data: ,
                        dataType: "json",
                        success: function(res){
                            if(res.sts == 0){
                                dialog_NIT.dialog( "close" );
                                return;
                            }else if(res.sts == 5){
                                alert(globalObj._nv == "zh-CN"?"该用户权限不足.":"Permission Denied!");
                            }else if(res.sts == 9){
                                window.location = "/login.esp";
                                return;
                            }
                            while(data.node.hasChildren()){
                                data.node.getFirstChild().remove();
                            }
                        },
                        error : function(err) {
                        }
                    });
                    break;
                } case '#addstrc':{
                    addstrc();
                    globalObj._prgoptflag = 1;
                    globalObj._editnodekey = data.node.key;
                    dialog_nitc.dialog( "open" );
                    break;
                } case '#addstrs':{
                    addstrs();
                    globalObj._prgoptflag = 1;
                    globalObj._editnodekey = data.node.key;
                    dialog_nits.dialog( "open" );
                    break;
                }  case '#addstrt':{
                    addstrt();
                    globalObj._prgoptflag = 1;
                    globalObj._editnodekey = data.node.key;
                    dialog_nitt.dialog( "open" );
                    break;
                } case '#editstr': {
                    $.ajax({
                        type: "GET",
                        async:false,
                        url: "http://"+globalObj.localip+":4000/do/nitController/getstream?channel=2&streamid="+ data.node.data.streamid,
                        //data: ,
                        dataType: "json",
                        success: function(res){
                            if(res.sts == 0){
                                alert(globalObj._nv == "zh-CN"?"读取流错误!":"Read Stream Error!");
                                dialog_NIT.dialog( "close" );
                                return;
                            }else if(res.sts == 5){
                                alert(globalObj._nv == "zh-CN"?"该用户权限不足.":"Permission Denied!");
                            }else if(res.sts == 9){
                                window.location = "/login.esp";
                                return;
                            }
                            switch(data.node.data.type){
                                case 'DVB-C':
                                    editnitc(data, res);
                                    globalObj._prgoptflag = 0;
                                    globalObj._editnodekey = data.node.key;
                                    dialog_nitc.dialog( "open" );
                                    break;
                                case 'DVB-S':
                                    editnits(data, res);
                                    globalObj._prgoptflag = 0;
                                    globalObj._editnodekey = data.node.key;
                                    dialog_nits.dialog( "open" );
                                    break;
                                case 'DVB-T':
                                    editnitt(data, res);
                                    globalObj._prgoptflag = 0;
                                    globalObj._editnodekey = data.node.key;
                                    dialog_nitt.dialog( "open" );
                                    break;
                            }
                        },
                        error : function(err) {
                        }
                    });
                    break;
                } case '#deletestr': {
                    var oristreamid = data.node.data.streamid;
                    $.ajax({
                        type: "GET",
                        async:false,
                        url: "http://"+globalObj.localip+":4000/do/nitController/delstr?channel=2&streamid="+oristreamid,
                        //data: ,
                        dataType: "json",
                        success: function(data){
                            if(data.sts == 0){
                                dialog_NIT.dialog( "close" );
                                return;
                            }else if(data.sts == 5){
                                alert(globalObj._nv == "zh-CN"?"该用户权限不足.":"Permission Denied!");
                            }else if(data.sts == 9){
                                window.location = "/login.esp";
                                return;
                            }
                        },
                        error : function(err) {
                        }
                    });
                    data.node.remove();
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
			"OK": function() {
			  dialog_desc.dialog( "close" );
			},
			"Cancel": function() {
			  dialog_desc.dialog( "close" );
			}
		}
	});

    //编辑NIT右键菜单弹出对话框
    var dialog_nitc = $( "#dialog-NITC" ).dialog({
        autoOpen: false,
        height: 500,
        width: 800,
        modal: true,
        buttons: {
            "ADD":function() {
                $('#tbl_nitc').DataTable().row.add( [
                    0,
                    0
                ] ).draw();
            } ,
            "DELETE":function() {
                $('#tbl_nitc').DataTable().row('.selected').remove().draw( false );
            },
            "OK": function() {
                var oristreamid = 0;
                if(globalObj._channel == 1 && globalObj._prgoptflag == 0){
                    oristreamid = $("#out_tree").fancytree("getTree").getNodeByKey(globalObj._editnodekey).data.streamid;
                }else if(globalObj._channel == 2 && globalObj._prgoptflag == 0){
                    oristreamid = $("#out_tree2").fancytree("getTree").getNodeByKey(globalObj._editnodekey).data.streamid;
                }
                var data = $('#tbl_nitc').DataTable().$('input, select').serialize();
                var jsonstr;
                if(data == ""){
                    jsonstr = '{"channel":' + globalObj._channel +',"flag":' + globalObj._prgoptflag
                    +',"oristreamid":' + oristreamid +',"streamid":' + parseInt($('.nitc_id').val())
                    + ',"netid":' + parseInt($('.nitc_netid').val()) + ',"hz":' + parseInt($('.nitc_hz').val())
                    + ',"ksm":' + parseInt($('.nitc_ksm').val()) + ',"fecout":' + $('#nitc_fecout').val() + ',"cnt":'+$('#tbl_nitc').DataTable().$('tr').length
                    + ',"fecin":' + $('#nitc_fecin').val() + ',"qam":' + $('#nitc_qam').val() + '}';
                }else{
                    jsonstr = '{"channel":' + globalObj._channel +',"flag":' + globalObj._prgoptflag
                    +',"oristreamid":' + oristreamid +',"streamid":' + parseInt($('.nitc_id').val())
                    + ',"netid":' + parseInt($('.nitc_netid').val()) + ',"hz":' + parseInt($('.nitc_hz').val())
                    + ',"ksm":' + parseInt($('.nitc_ksm').val()) + ',"fecout":' + $('#nitc_fecout').val() + ',"cnt":'+$('#tbl_nitc').DataTable().$('tr').length
                    + ',"fecin":' + $('#nitc_fecin').val() + ',"qam":' + $('#nitc_qam').val() + ',"' + data.replace(/&/g, '","').replace(/=/g, '":"') + '"}';
                }
                $.ajax({
                    type: "GET",
                    async:false,
                    url: "http://"+globalObj.localip+":4000/do/nitController/addnitcstream",
                    data: JSON.parse(jsonstr),
                    dataType: "json",
                    success: function(data){
                        if(data.sts == 0){
                            alert(globalObj._nv == "zh-CN"?"数据输入错误!!":"DATA ERROR!!");
                            return;
                        }else if(data.sts == 5){
                            alert(globalObj._nv == "zh-CN"?"该用户权限不足.":"Permission Denied!");
                        }
                        //获取制表信息
                        createTable();
                    },
                    error : function(err) {
                    }
                });
                dialog_nitc.dialog( "close" );
            },
            "CANCEL": function() {
                dialog_nitc.dialog( "close" );
            }
        }
    });

    //编辑NIT右键菜单弹出对话框
    var dialog_nits = $( "#dialog-NITS" ).dialog({
        autoOpen: false,
        height: 550,
        width: 800,
        modal: true,
        buttons: {
            "ADD":function() {
                $('#tbl_nits').DataTable().row.add( [
                    0,
                    0
                ] ).draw();
            } ,
            "DELETE":function() {
                $('#tbl_nits').DataTable().row('.selected').remove().draw( false );
            },
            "OK": function() {
                var oristreamid = 0;
                if(globalObj._channel == 1 && globalObj._prgoptflag == 0){
                    oristreamid = $("#out_tree").fancytree("getTree").getNodeByKey(globalObj._editnodekey).data.streamid;
                }else if(globalObj._channel == 2 && globalObj._prgoptflag == 0){
                    oristreamid = $("#out_tree2").fancytree("getTree").getNodeByKey(globalObj._editnodekey).data.streamid;
                }
                var data = $('#tbl_nits').DataTable().$('input, select').serialize();
                var jsonstr;
                if(data == ""){
                    jsonstr = '{"channel":' + globalObj._channel +',"flag":' + globalObj._prgoptflag
                    +',"oristreamid":' + oristreamid +',"streamid":' + parseInt($('.nits_id').val())
                    + ',"netid":' + parseInt($('.nits_netid').val()) + ',"hz":' + parseInt($('.nits_hz').val())
                    + ',"ksm":' + parseInt($('.nits_ksm').val()) + ',"path":' + parseInt($('.nits_path').val())
                    + ',"pol":' + $('#nits_pol').val() + ',"cnt":'+$('#tbl_nits').DataTable().$('tr').length
                    + ',"fecin":' + $('#nits_fecin').val() + ',"qpsk":' + $('#nits_qpsk').val() + ',"tip":' + $('#nits_tip').val() + '}';
                }else{
                    jsonstr = '{"channel":' + globalObj._channel +',"flag":' + globalObj._prgoptflag
                    +',"oristreamid":' + oristreamid +',"streamid":' + parseInt($('.nits_id').val())
                    + ',"netid":' + parseInt($('.nits_netid').val()) + ',"hz":' + parseInt($('.nits_hz').val())
                    + ',"ksm":' + parseInt($('.nits_ksm').val()) + ',"path":' + parseInt($('.nits_path').val())
                    + ',"pol":' + $('#nits_pol').val() + ',"cnt":'+$('#tbl_nits').DataTable().$('tr').length
                    + ',"fecin":' + $('#nits_fecin').val() + ',"qpsk":' + $('#nits_qpsk').val() + ',"tip":' + $('#nits_tip').val() + ',"' + data.replace(/&/g, '","').replace(/=/g, '":"') + '"}';
                }
                $.ajax({
                    type: "GET",
                    async:false,
                    url: "http://"+globalObj.localip+":4000/do/nitController/addnitsstream",
                    data: JSON.parse(jsonstr),
                    dataType: "json",
                    success: function(data){
                        if(data.sts == 0){
                            alert(globalObj._nv == "zh-CN"?"数据输入错误!!":"DATA ERROR!!");
                            return;
                        }else if(data.sts == 5){
                            alert(globalObj._nv == "zh-CN"?"该用户权限不足.":"Permission Denied!");
                        }
                        //获取制表信息
                        createTable();
                    },
                    error : function(err) {
                    }
                });
                dialog_nits.dialog( "close" );
            },
            "CANCEL": function() {
                dialog_nits.dialog( "close" );
            }
        }
    });

    //编辑NIT右键菜单弹出对话框
    var dialog_nitt = $( "#dialog-NITT" ).dialog({
        autoOpen: false,
        height: 550,
        width: 800,
        modal: true,
        buttons: {
            "ADD":function() {
                $('#tbl_nitt').DataTable().row.add( [
                    0,
                    0
                ] ).draw();
            } ,
            "DELETE":function() {
                $('#tbl_nitt').DataTable().row('.selected').remove().draw( false );
            },
            "OK": function() {
                var oristreamid = 0;
                if(globalObj._channel == 1 && globalObj._prgoptflag == 0){
                    oristreamid = $("#out_tree").fancytree("getTree").getNodeByKey(globalObj._editnodekey).data.streamid;
                }else if(globalObj._channel == 2 && globalObj._prgoptflag == 0){
                    oristreamid = $("#out_tree2").fancytree("getTree").getNodeByKey(globalObj._editnodekey).data.streamid;
                }
                var data = $('#tbl_nitt').DataTable().$('input, select').serialize();
                var jsonstr;
                if(data == ""){
                    jsonstr = '{"channel":' + globalObj._channel +',"flag":' + globalObj._prgoptflag
                    +',"oristreamid":' + oristreamid +',"streamid":' + parseInt($('.nitt_id').val())
                    + ',"netid":' + parseInt($('.nitt_netid').val()) + ',"hz":' + parseInt($('.nitt_hz').val())
                    + ',"cnt":'+$('#tbl_nitt').DataTable().$('tr').length + ',"bandwidth":' + $('#nitt_bandwidth').val()
                    + ',"constell":' + $('#nitt_constell').val() + ',"transmissionMode":' + $('#nitt_transmissionMode').val()
                    + ',"hierarchy":' + $('#nitt_hierarchy').val() + ',"hp":' + $('#nitt_hp').val()
                    + ',"lp":' + $('#nitt_lp').val() + ',"gi":' + $('#nitt_gi').val() + '}';
                }else{
                    jsonstr = '{"channel":' + globalObj._channel +',"flag":' + globalObj._prgoptflag
                    +',"oristreamid":' + oristreamid +',"streamid":' + parseInt($('.nitt_id').val())
                    + ',"netid":' + parseInt($('.nitt_netid').val()) + ',"hz":' + parseInt($('.nitt_hz').val())
                    + ',"cnt":'+$('#tbl_nitt').DataTable().$('tr').length + ',"bandwidth":' + $('#nitt_bandwidth').val()
                    + ',"constell":' + $('#nitt_constell').val() + ',"transmissionMode":' + $('#nitt_transmissionMode').val()
                    + ',"hierarchy":' + $('#nitt_hierarchy').val() + ',"hp":' + $('#nitt_hp').val()
                    + ',"lp":' + $('#nitt_lp').val() + ',"gi":' + $('#nitt_gi').val() + ',"' + data.replace(/&/g, '","').replace(/=/g, '":"') + '"}';
                }
                $.ajax({
                    type: "GET",
                    async:false,
                    url: "http://"+globalObj.localip+":4000/do/nitController/addnittstream",
                    data: JSON.parse(jsonstr),
                    dataType: "json",
                    success: function(data){
                        if(data.sts == 0){
                            alert(globalObj._nv == "zh-CN"?"数据输入错误!!":"DATA ERROR!!");
                            return;
                        }else if(data.sts == 5){
                            alert(globalObj._nv == "zh-CN"?"该用户权限不足.":"Permission Denied!");
                        }
                        //获取制表信息
                        createTable();
                    },
                    error : function(err) {
                    }
                });
                dialog_nitt.dialog( "close" );
            },
            "CANCEL": function() {
                dialog_nitt.dialog( "close" );
            }
        }
    });

	//PID透传右键菜单弹出对话框
	var dialog_pid = $( "#dialog-pid" ).dialog({
		autoOpen: false,
		width: 750,
		modal: true,
		buttons: {
			"ADD": function() {
				var index = Number(globalObj._tbl_pid[0].rows[globalObj._tbl_pid[0].rows.length - 1].firstChild.textContent);
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
			"DELETE": function() {
				$('#tbl_pid').DataTable().row('.selected').remove().draw( false );
			},
			"OK": function() {
				var jsonstr;
				var data = $('#tbl_pid').DataTable().$('input').serialize();
				if(data == ""){
					jsonstr = '{"channel":' + globalObj._channel + ',"pidcnt":'+$('#tbl_pid').DataTable().$('tr').length + '}';
				}else{
					jsonstr = '{"channel":' + globalObj._channel + ',"pidcnt":'+$('#tbl_pid').DataTable().$('tr').length + ',"' + data.replace(/&/g, '","').replace(/=/g, '":"') +  '"}';
				}				
				//下发配置
				$.ajax({
					 type: "GET",
					 async:false,
					 url: "http://"+globalObj.localip+":4000/do/programs/setpidtransinfo",
					 data: JSON.parse(jsonstr),
					 dataType: "json",
					 success: function(data){
						if(data.sts == 0){
                            alert(globalObj._nv == "zh-CN"?"数据输入错误!!":"DATA ERROR!!");
                            return;
                        }else if(data.sts == 5){
                            alert(globalObj._nv == "zh-CN"?"该用户权限不足.":"Permission Denied!");
                        }
						dialog_pid.dialog( "close" );
                         $('#tbl_pid').DataTable().destroy();
                         $('#tbl_pid').empty();
					 },    
					 error : function(err) {    
					 }   
				});
				
			},
			"CANCEL": function() {
				dialog_pid.dialog( "close" );
                $('#tbl_pid').DataTable().destroy();
                $('#tbl_pid').empty();
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
			"OK": function() {
                var nitid = parseInt($('.nit_id')[0].value, 16);
                if($('.nit_name').val().length > 32){
                    alert(globalObj._nv == "zh-CN"?"名称太长，最多32字符.":"Name is too long, 32 length limited!");
                    return;
                }
                if(!isNaN(nitid)){
                    if(nitid < 0 || nitid > 0xffff){
                        return;
                    }
                }else{
                    return;
                }
                var jsonstr = '{"channel":' + globalObj._channel + ',"flag":' + globalObj._prgoptflag + ',"netid":' + nitid + ',"name":"' + $('.nit_name').val() + '"}';
                $.ajax({
                    type: "GET",
                    async:false,
                    url: "http://"+globalObj.localip+":4000/do/nitController/addnitinfo",
                    data: JSON.parse(jsonstr),
                    dataType: "json",
                    success: function(data){
                        if(data.sts == 0){
                            dialog_NIT.dialog( "close" );
                            return;
                        }else if(data.sts == 5){
                            alert(globalObj._nv == "zh-CN"?"该用户权限不足.":"Permission Denied!");
                        }else if(data.sts == 9){
                            window.location = "/login.esp";
                            return;
                        }
                        //获取制表信息
                        createTable();
                        dialog_NIT.dialog( "close" );
                    },
                    error : function(err) {
                    }
                });
			},
			"CANCEL": function() {
			    dialog_NIT.dialog( "close" );
			}
		}
	});
	
	//编辑节目右键菜单弹出对话框
	var dialog_edit = $( "#dialog-form" ).dialog({
		autoOpen: false,
		height: 500,
		width: 900,
		modal: true,
		buttons: {
			"ADD":function() {
                var tblindex = Number(globalObj._tbl_edit[0].rows[globalObj._tbl_edit[0].rows.length - 1].firstChild.textContent);
                if(isNaN(tblindex)){
                    tblindex = 0;
                }
				$('#tbl_editprg').DataTable().row.add( [
                    tblindex + 1,
					Number($('.prg_prc').val()),
					'MPEG2 Audio',
					200,
					200
				] ).draw();
			} ,
			"DELETE":function() {
				$('#tbl_editprg').DataTable().row('.selected').remove().draw( false );
			},
			"OK": function() {
				var strindex = "";
                if($('.prg_name')[0].value == ""){
					alert(globalObj._nv == "zh-CN"?"节目名称不能为空.":"Name can not be null!");
                    return false;
                }
				for(var i=1;i<globalObj._tbl_edit[0].rows.length;i++){
					strindex += ',"index'+(i-1)+'":' + globalObj._tbl_edit[0].rows[i].firstChild.textContent;
				}
				var data = $('#tbl_editprg').DataTable().$('input, select').serialize();
                if($('#tbl_editprg').DataTable().$('tr').length >0){
                    var jsonstr = '{"channel":' + globalObj._channel + ',"chnid":' + $('.prg_prc').val() + ',"isuserprg":' + globalObj._isuserprg + ',"prgoptflag":' + globalObj._prgoptflag + ',"servicetype":' + $('#r_servicetype').val() + ',"providername":"' + $('.prg_merchant').val() + '","prgname":"' + $('.prg_name').val() + '","prgNum":' + Number($('.prg_no').val()) + ',"orgiralindex":"' +  globalObj._index.toString(16) + '","pmtpid":"' +  $('.prg_pid').val() + '","oldpcrpid":"' + $('.prg_prc1').val() + '","newpcrpid":"' + $('.prg_prc2').val()+ '","streamcnt":'+$('#tbl_editprg').DataTable().$('tr').length+',"' +data.replace(/&/g, '","').replace(/=/g, '":"') + '"' + strindex + '}';
                }else{
                    var jsonstr = '{"channel":' + globalObj._channel + ',"chnid":' + $('.prg_prc').val() + ',"isuserprg":' + globalObj._isuserprg + ',"prgoptflag":' + globalObj._prgoptflag + ',"servicetype":' + $('#r_servicetype').val() + ',"providername":"' + $('.prg_merchant').val() + '","prgname":"' + $('.prg_name').val() + '","prgNum":' + Number($('.prg_no').val()) + ',"orgiralindex":"' +  globalObj._index.toString(16) + '","pmtpid":"' +  $('.prg_pid').val() + '","oldpcrpid":"' + $('.prg_prc1').val() + '","newpcrpid":"' + $('.prg_prc2').val()+ '","streamcnt":'+$('#tbl_editprg').DataTable().$('tr').length+ '}';
                }

				//下发配置
				$.ajax({
					 type: "GET",
					 async:false,
					 url: "http://"+globalObj.localip+":4000/do/programs/setprginfo",
					 data: JSON.parse(jsonstr),
					 dataType: "json",
					 success: function(data){
						if(data.sts == 1){
                            var tmptree;
                            var snt;
                            if(globalObj._channel == 1){
                                tmptree = $("#channel").fancytree("getTree");
                                if(globalObj._prgoptflag == 1){
                                    globalObj._selectcount++;
                                }
                                snt = globalObj._selectcount;
                            }else if(globalObj._channel == 2){
                                tmptree = $("#channel2").fancytree("getTree");
                                if(globalObj._prgoptflag == 1){
                                    globalObj._selectcount2++;
                                }
                                snt = globalObj._selectcount2;
                            }
                            //获取输出通道信息
                            readoutprgs(tmptree, snt);
                            $('.prg_prc').attr("disabled", true);
						}else if(data.sts == 5){
                            alert(globalObj._nv == "zh-CN"?"该用户权限不足.":"Permission Denied!");
                        }
					 }
				});
				dialog_edit.dialog( "close" );
			},
			"CANCEL": function() {
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
			"OK": function() {
				var jsonstr = '{"channel":' +$('#tag_channel')[0].textContent + ',"networkId":'+ Number($('.item_netid').val()) + ',"streamId":' + Number($('.item_transid').val()) + ',"oringal_networkid":' + Number($('.item_orignetid').val()) + ',"outputRate":' + Number($('.item_out').val())+ ',"isAutoRaiseVersion":' + ($('.autoinc_ver')[0].checked == true?1:0) + ',"version":' + Number($('.item_version')[0].textContent) + ',"isAutoRankPAT":' + ($('.pat_auto')[0].checked == true?1:0) + ',"isNeedSend_cat":' + ($('.sl_cat')[0].checked == true?1:0) + ',"isNeedSend_nit":' + ($('.sl_nit')[0].checked == true?1:0) + ',"isNeedSend_pat":' + ($('.sl_pat')[0].checked == true?1:0) + ',"isNeedSend_pmt":' + ($('.sl_pmt')[0].checked == true?1:0) + ',"isNeedSend_sdt":' + ($('.sl_sdt')[0].checked == true?1:0) + '}';
				//下发配置
				$.ajax({
					 type: "GET",
					 async:false,
					 url: "http://"+globalObj.localip+":4000/do/programs/setchanneloutinfo",
					 data: JSON.parse(jsonstr),
					 dataType: "json",
					 success: function(data){
						if(data.sts == 5){
                            alert(globalObj._nv == "zh-CN"?"该用户权限不足.":"Permission Denied!");
						}
					 },    
					 error : function(err) {    
					 }   
				});
				dig_itmes.dialog( "close" );
			},
			"CANCEL": function() {
				dig_itmes.dialog( "close" );
			},
			"APPLY":function() {
                var jsonstr = '{"channel":' +$('#tag_channel')[0].textContent + ',"networkId":'+ Number($('.item_netid').val()) + ',"streamId":' + Number($('.item_transid').val()) + ',"oringal_networkid":' + Number($('.item_orignetid').val()) + ',"outputRate":' + Number($('.item_out').val())+ ',"isAutoRaiseVersion":' + ($('.autoinc_ver')[0].checked == true?1:0) + ',"version":' + Number($('.item_version')[0].textContent) + ',"isAutoRankPAT":' + ($('.pat_auto')[0].checked == true?1:0) + ',"isNeedSend_cat":' + ($('.sl_cat')[0].checked == true?1:0) + ',"isNeedSend_nit":' + ($('.sl_nit')[0].checked == true?1:0) + ',"isNeedSend_pat":' + ($('.sl_pat')[0].checked == true?1:0) + ',"isNeedSend_pmt":' + ($('.sl_pmt')[0].checked == true?1:0) + ',"isNeedSend_sdt":' + ($('.sl_sdt')[0].checked == true?1:0) + '}';
                //下发配置
                $.ajax({
                    type: "GET",
                    async:false,
                    url: "http://"+globalObj.localip+":4000/do/programs/setchanneloutinfo",
                    data: JSON.parse(jsonstr),
                    dataType: "json",
                    success: function(data){
                        if(data.sts == 5){
                            alert(globalObj._nv == "zh-CN"?"该用户权限不足.":"Permission Denied!");
                        }
                    },
                    error : function(err) {
                    }
                });
			}
		}
	});

    var dig_notification = $( "#progress-notification" ).dialog({
        autoOpen: false,
        modal: true,
        width: 300
    });

    var progressbar = $( "#progressbar" ),
        progressLabel = $( ".progress-label" );
    var progress_dialog = $( "#progress-dialog" ).dialog({
        autoOpen: false,
        closeOnEscape: false,
        modal: true,
        resizable: false
    });
    progressbar.progressbar({
        value: true,
        change: function() {
            progressLabel.text( globalObj._nv == "zh-CN"?("正在搜索第 "+ $('.progress-label')[0].dataset.ch + " 通道"):("Searching "+ $('.progress-label')[0].dataset.ch + " Channel"));
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
    if(globalObj.timerID != undefined){
        clearInterval(globalObj.timerID);
    }
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
						+'<td><input type="text" value="" /><label>&nbsp &nbsp(eg:192.168.1.103)</label></td>'
					+'</tr>'
					+'<tr>'
						+'<td><label>源端口号</label></td>'
						+'<td><input type="text" value="" /><label>&nbsp &nbsp(0~65535)</label></td>'
					+'</tr>'
					+'<tr>'
						+'<td><label>源MAC</label></td>'
						+'<td><input type="text" value="" /><label>&nbsp &nbsp(值为16进制  eg:00:11:22:33:44:55)</label></td>'
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
    if(globalObj.timerID != undefined){
        clearInterval(globalObj.timerID);
    }
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
						+'<td><input type="text" value="" /><label>&nbsp &nbsp(eg:192.168.1.103)</label></td>'
					+'</tr>'
					+'<tr>'
						+'<td><label>目的端口号</label></td>'
						+'<td><input type="text" value="" /><label>&nbsp &nbsp(0~65535)</label></td>'
					+'</tr>'
					+'<tr>'
						+'<td><label>目的MAC</label></td>'
						+'<td><input type="text" value="" /><label>&nbsp &nbsp(值为16进制  eg:00:11:22:33:44:55)</label></td>'
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
