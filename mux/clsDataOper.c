
#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "datastructdef.h"
#include "communicate.h"
#include "clsDataOper.h"


static int LittleFormat_fromBytes(int offset, int length,  unsigned char *inBytes)
{
	int rtnInt = 0;
	for (int i = 0; i < length; i++)
	{
		rtnInt += inBytes[offset++] << (i * 8);
	}
	return rtnInt;
}

static int LittleFormat_intToBytes(int intValue,  unsigned char *outBytes, int offset, int bytesLen)
{
	for (int i = 0; i < bytesLen; i++)
	{
		outBytes[offset++] = (char)((intValue) & (0xFF));
		intValue >>= 8;
	}

	return bytesLen;
}

static unsigned int LittleFormat_uintfromBytes(int offset, unsigned char *inBytes)
{
	int i=0;
	unsigned int rtnInt = 0;
	for (i = 0; i < 4; i++)
	{
		rtnInt += (unsigned int)(inBytes[offset++] << (i * 8));
	}
	//rtnInt = inBytes[0] + inBytes[1] * 0x100 + inBytes[2] * 0x10000 + inBytes[3] * 0x1000000;

	return rtnInt;
}

static ushort LittleFormat_pidFromBytes(int offset, unsigned char *inBytes)
{
	return (ushort)(((inBytes[offset + 1] & 0x1f) << 8) | inBytes[offset]);
}

static int LittleFormat_pidToBytes(int intValue, unsigned char *outBytes, int offset)
{
	outBytes[offset + 1] = (byte)((intValue >> 8) & 0x1f);
	outBytes[offset] = (byte)intValue;
	return 2;
}

// ------------------------

static int BigFormat_fromBytes(int offset, int length, unsigned char *inBytes)
{
	int rtnInt = 0;
	for (int i = 0; i < length; i++)
	{
		rtnInt <<= 8;
		rtnInt += inBytes[offset++];
	}
	return rtnInt;
}

static int BigFormat_intToBytes(int intValue, unsigned char *outBytes, int offset, int bytesLen)
{
	for (int i = bytesLen; i > 0; i--)
	{
		outBytes[offset + i - 1] = (byte)(intValue);
		intValue >>= 8;
	}

	return bytesLen;
}

static int BigFormat_uintToBytes(uint intValue, unsigned char *outBytes, int offset, int bytesLen)
{
	for (int i = bytesLen; i > 0; i--)
	{
		outBytes[offset + i - 1] = (byte)(intValue);
		intValue >>= 8;
	}

	return bytesLen;
}

static bool GetIntFromHex(string hexStr, out int outIntNum)
{
	outIntNum = 0;
	if (hexStr == null || hexStr == string.Empty)
		return false;
	if(hexStr.StartsWith("0x") || hexStr.StartsWith("0X"))
	{
		hexStr = hexStr.Substring(2);
	}
	return int.TryParse(hexStr,System.Globalization.NumberStyles.HexNumber, null, out outIntNum);
}

static bool HexStringBySpaceToBytes(string hexStr, out unsigned char *outBytes)
{
	outBytes = null;
	string[] subStrs = hexStr.Split(new char[]{' '}, StringSplitOptions.RemoveEmptyEntries);
	if (subStrs == null || subStrs.Length == 0)
		return false;
	outBytes = new byte[subStrs.Length];
	try
	{
		for(int i = 0; i < subStrs.Length; i++)
		{
			outBytes[i] = byte.Parse(subStrs[i], System.Globalization.NumberStyles.HexNumber);
		}
		return true;
	}
	catch
	{
		return false;
	}			
}

static ushort BigFormat_pidFromBytes(int offset, unsigned char *inBytes)
{
	return (ushort)(((inBytes[offset] & 0x1f) << 8) | inBytes[offset + 1]);
}

static int BigFormat_pidToBytes(int intValue, unsigned char *outBytes, int offset)
{
	outBytes[offset] = (byte)((intValue >> 8) & 0x1f);
	outBytes[offset + 1] = (byte)intValue;
	return 2;
}

// --------------------------

static string Bytes2HexString(unsigned char *inBytes, int offset, int length, char splitChar) // eg. splitChar == ','
{
	string rtnStr = string.Empty;
	if (inBytes != null && inBytes.Length >= offset + length)
	{
		for (int i = 0; i < length - 1; i++)
		{
			rtnStr += inBytes[offset++].ToString("X2") + splitChar;
		}
		rtnStr += inBytes[offset++].ToString("X2");
	}
	return rtnStr;
}

static unsigned char *HexString2Bytes(string hexString, char splitChar)
{
	string[] subStr = hexString.Split(new char[] { splitChar }, StringSplitOptions.RemoveEmptyEntries);

	List<byte> outBytesList = new List<byte>();
	for (int i = 0; i < subStr.Length; i++)
	{
		byte tmpByte;
		string tmpStr = subStr[i];
		if (tmpStr.Length % 2 == 1) // 偶数位数字长度补齐
		{
			tmpStr = "0" + tmpStr;
		}
		while(tmpStr.Length > 2)
		{
			string subTmpStr = tmpStr.Substring(0, 2);
			if (!byte.TryParse(subTmpStr, System.Globalization.NumberStyles.HexNumber, null, out tmpByte))
				return null;
			outBytesList.Add(tmpByte);
			tmpStr = tmpStr.Substring(2, tmpStr.Length - 2);
		}
		if (!byte.TryParse(tmpStr, System.Globalization.NumberStyles.HexNumber, null, out tmpByte))
			return null;
		outBytesList.Add(tmpByte);			
	}
	unsigned char *outBytes = new byte[outBytesList.Count];
	for (int i = 0; i < outBytes.Length; i++)
	{
		outBytes[i] = outBytesList[i];
	}
	return outBytes;
}

// ----------------------------
static byte Bcd_code2Byte(byte codeByte)
{
	byte rtnByte = byte.Parse(codeByte.ToString("x"));
	return rtnByte;
}

static byte Bcd_codeFromByte(byte dataByte)
{
	byte rtnByte = byte.Parse(dataByte.ToString(), System.Globalization.NumberStyles.HexNumber);
	return rtnByte;
}

static int HexFromString(string valueString)
{
	int rtnValue;
	if (valueString != null && valueString != string.Empty)
	{
		if (valueString.StartsWith("0x") || valueString.StartsWith("0X"))
			valueString = valueString.Substring(2, valueString.Length - 2);

		if (int.TryParse(valueString, System.Globalization.NumberStyles.HexNumber, null, out rtnValue))
			return rtnValue;
	}
	return 0x1fff;
}


