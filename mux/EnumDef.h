
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

/*	typedef enum SearchingStatus
    {
        Error, noSearched = 0, searching = 1, Ok = 2
    }SearchingStatus
*/
#endif