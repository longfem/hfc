
#ifndef ENUM_DEFINE_H
#define ENUM_DEFINE_H 


	typedef enum MsgTypeEm
	{ 
		normal, warnning, msgerror, success
	}MsgTypeEm;

	typedef enum AlarmLevelEm
	{ 
		lv1, lv2, none
	}AlarmLevelEm;

	typedef enum UserGroupEm
	{ 
		invalid, guest, user, admin
	}UserGroupEm;

	typedef enum ConnectTypeEm
	{
		TCP = 1, UDP = 2, COM = 3
	}ConnectTypeEm;

	typedef enum DevNoticeCmdEm
	{
		disconnect, reconnect, warning, msg, good
	}DevNoticeCmdEm;

	typedef enum ErrorTypeEm
	{
		ok, error, length, cmd
	}ErrorTypeEm;


	typedef enum PsiTableType
	{
		pat = 1, pmt = 2, sdt = 3, cat = 4, nit = 5
	}PsiTableType;

	typedef enum DesTypeEnum
    {
        video_stream = 2,
        audio_stream = 3,
        hierarchy = 4,
        registration = 5,
        data_stream_alignment = 6,
        target_background_grid = 7,
        video_window = 8,
        CA = 9,
        ISO_639_language = 10,
        system_clock = 11,
        multiplex_buffer_utilization = 12,
        copyright = 13,
        maximum_bitrate = 14,
        private_data_indicator = 15,
        smoothing_buffer = 16,
        STD = 17,
        IBP = 18,

        MPEG4_video = 27,
        MPEG4_audio = 28,
        IOD = 29,
        SL = 30,
        FMC = 31,
        External_ES_ID = 32,
        MuxCode = 33,
        FmxBufferSize = 34,
        MultiplexBuffer = 35,
        Content_labeling = 36,
        Metadata_pointer = 37,
        Metadata = 38,
        Metadata_STD = 39,
        AVC_video = 40,
        IPMP = 41,
        AVC_timing_and_HRD = 42,
        // ------- SI 描述符
        Network_name_name = 0x40,
        service_list = 0x41,
        stuffing = 0x42,
        satellite_delivery_system = 0x43,
        cable_delivery_system = 0x44,
        bouquet_name = 0x47,
        server = 0x48,
        coutry_availability = 0x49,
        linkage = 0x4a,
        NVOD_reference = 0x4b,
        time_shifted_service = 0x4c,
        short_event = 0x4d,
        extended_event = 0x4e,
        time_shifted_event = 0x4f,
        component = 0x50,
        mosaic = 0x51,
        stream_identifier = 0x52,
        CA_identifier = 0x53,
        content = 0x54,
        parental_rating = 0x55,
        teletext = 0x56,
        telephone = 0x57,
        local_time_offset = 0x58,
        subtitling = 0x59,
        terrestrial_delivery_system = 0x5a,
        multilingual_network_name = 0x5b,
        multilingual_bouquet_name = 0x5c,
        multilingual_service_name = 0x5d,
        multilingual_component_name = 0x5e,
        private_data_specifier = 0x5f,
        service_move = 0x60,
        short_smoothing_buffer = 0x61,
        frequency_list = 0x62,
        partial_transport_stream = 0x63,
        data_broadcast = 0x64,
        CA_system = 0x65,
        data_broadcast_id = 0x66,
        unknown = 0x0
    }DesTypeEnum;

/*	typedef enum SearchingStatus
    {
        Error, noSearched = 0, searching = 1, Ok = 2
    }SearchingStatus
*/
#endif