#ifndef __LIBIDT_SDK_H___
#define __LIBIDT_SDK_H___


#define IN
#define OUT
#define IN_OUT
#include "IDTDef.h"

/**
 * define the unsigned char as BYTE
 */
typedef unsigned char BYTE;
/**
 * Define the USB hot-plug callback function to monitor the info when plug in/out the reader. <br/>
 * It should be registered using the registerHotplugCallBk,
 */
typedef void (*pMessageHotplug)(int, int);

/**
 * Define the send command callback function to monitor the sending command into the reader. <br/>
 * It should be registered using the registerLogCallBk,
 */
typedef void (*pSendDataLog)(unsigned char *, int);
/**
 * Define the read response callback function to monitor the reading response from the reader. <br/>
 * It should be registered using the registerLogCallBk,
 */
typedef void (*pReadDataLog)(unsigned char *, int);
/**
 * Define the EMV callback function to get the transaction message/data/result. <br/>
 * It should be registered using the emv_registerCallBk,
 */
typedef void (*pEMV_callBack)(int, int,unsigned char *, int,IDTTransactionData*,EMV_Callback*,int);
/**
 * Define the MSR callback function to get the MSR card data <br/>
 * It should be registered using the msr_registerCallBk,
 */
typedef void (*pMSR_callBack)(int, IDTMSRData);
/**
 * Define the MSR callback function to get the MSR card data pointer<br/>
 * It should be registered using the msr_registerCallBk,
 */
typedef void (*pMSR_callBackp)(int, IDTMSRData *);
/**
 * Define the PINPad callback function to get the input PIN Pad data <br/>
 * It should be registered using the pin_registerCallBk,
 */
typedef void (*pPIN_callBack)(int, IDTPINData *);
/**
 * Define the LCD callback function to get the input LCDItem <br/>
 * It should be registered using the lcd_registerCallBk,
 */
typedef void (*pLCD_callBack)(int,
                              IDTLCDItem *);
/**
 * Define the firmware update callback function to get the status of firmware update <br/>
 * It should be registered using the device_registerFWCallBk,
 */
typedef void (*pFW_callBack)(int, int, int, int, int);


#ifdef __cplusplus
extern "C" {
#endif
/**
 *To register the USB HID hot-plug callback function which implemented in the application to monitor the hotplug message from the SDK.
 */

void registerHotplugCallBk(pMessageHotplug pMsgHotplug);

/**
 *To register the log callback function which implemented in the application to monitor sending/reading data between application and reader.
 */
void registerLogCallBk(pSendDataLog pFSend, pReadDataLog pFRead);
/**
 *To register the emv callback function to get the EMV processing response.
 */
void emv_registerCallBk(pEMV_callBack pEMVf);
/**
 *To register the msr callback function to get the MSR card data.
 */
void msr_registerCallBk(pMSR_callBack pMSRf);
/**
 *To register the msr callback function to get the MSR card data pointer.
 */
void msr_registerCallBkp(pMSR_callBackp pMSRf);
/**
 *To register the pin callback function to get the PINPad data.
 */
void pin_registerCallBk(pPIN_callBack pPINf);
/**
 *To register the lcd callback function to get the LCDItem.
 */
void lcd_registerCallBk(pLCD_callBack pLCDf);
/**
 *To register the firmware update callback function to get the status of firmware update.
 */
void device_registerFWCallBk(pFW_callBack pFWf);
/**
 * To register the RKI callback function to get the
 * RKI status.
 */
void device_registerRKICallBk(pRKI_callBack pRKIf);
/**
 *To Get SDK version
 *@return return the SDK version string
 */
char *SDK_Version();

/**
 * Set the path to use when searching for ID TECH's libraries.
 * If this is not set, the libraries will be searched for with the system's default procedures.
 *
 * @param absoluteLibraryPath The absolute path to ID TECH's libraries.
 */
void setAbsoluteLibraryPath(const char *absoluteLibraryPath);

/**
 * Initial the device <br/>
 * It will detect the device and trying connect. <br/>
 * The connect status can be checked by device_isConnected(). <br/>
 * @return RETURN_CODE:  Values can be parsed with device_getErrorString()
 */
int device_init();
/**
 * Close the device <br/>
 */
void device_close();
/**
 * Review the return code description.<br/>
 * @param returnCode  the response result.
 * @param description
 * @retval the string for description of response result

 - 0: "no error, beginning task";
 - 1: "no response from reader";
 - 2: "invalid response data";
 - 3: "time out for task or CMD";
 - 4: "wrong parameter";
 - 5: "SDK is doing MSR or ICC task";
 - 6: "SDK is doing PINPad task";
 - 7: "SDK is doing CTLS task";
 - 8: "SDK is doing EMV task";
 - 9: "SDK is doing Other task";
 - 10: "err response or data";
 - 11: "no reader attached";
 - 12: "mono audio is enabled";
 - 13: "did connection";
 - 14: "audio volume is too low";
 - 15: "task or CMD be canceled";
 - 16: "UF wrong string format";
 - 17: "UF file not found";
 - 18: "UF wrong file format";
 - 19: "Attempt to contact online host failed";
 - 20: "Attempt to perform RKI failed";
 - 22: "Buffer size is not enough";
 - 0X300: "Key Type(TDES) of Session Key is not same as the related Master Key.";
 - 0X400: "Related Key was not loaded.";
 - 0X500: "Key Same.";
 - 0X501: "Key is all zero";
 - 0X502: "TR-31 format error";
 - 0X702: "PAN is Error Key.";
 - 0X705: "No Internal MSR PAN (or Internal MSR PAN is erased timeout)";
 - 0X0C01: "Incorrect Frame Tag";
 - 0X0C02: "Incorrect Frame Type";
 - 0X0C03: "Unknown Frame Type";
 - 0X0C04: "Unknown Command";
 - 0X0C05: "Unknown Sub-Command";
 - 0X0C06: "CRC Error";
 - 0X0C07: "Failed";
 - 0X0C08: "Timeout";
 - 0X0C0A: "Incorrect Parameter";
 - 0X0C0B: "Command Not Supported";
 - 0X0C0C: "Sub-Command Not Supported";
 - 0X0C0D: "Parameter Not Supported / Status Abort Command";
 - 0X0C0F: "Sub-Command Not Allowed";
 - 0X0D01: "Incorrect Header Tag";
 - 0X0D02: "Unknown Command";
 - 0X0D03: "Unknown Sub-Command";
 - 0X0D04: "CRC Error in Frame";
 - 0X0D05: "Incorrect Parameter";
 - 0X0D06: "Parameter Not Supported";
 - 0X0D07: "Mal-formatted Data";
 - 0X0D08: "Timeout";
 - 0X0D0A: "Failed / NACK";
 - 0X0D0B: "Command not Allowed";
 - 0X0D0C: "Sub-Command not Allowed";
 - 0X0D0D: "Buffer Overflow (Data Length too large for reader buffer)";
 - 0X0D0E: "User Interface Event";
 - 0X0D11: "Communication type not supported, VT-1, burst, etc.";
 - 0X0D12: "Secure interface is not functional or is in an intermediate state.";
 - 0X0D13: "Data field is not mod 8";
 - 0X0D14: "Pad  - 0X80 not found where expected";
 - 0X0D15: "Specified key type is invalid";
 - 0X0D1: "Could not retrieve key from the SAM(InitSecureComm)";
 - 0X0D17: "Hash code problem";
 - 0X0D18: "Could not store the key into the SAM(InstallKey)";
 - 0X0D19: "Frame is too large";
 - 0X0D1A: "Unit powered up in authentication state but POS must resend the InitSecureComm command";
 - 0X0D1B: "The EEPROM may not be initialized because SecCommInterface does not make sense";
 - 0X0D1C: "Problem encoding APDU";
 - 0X0D20: "Unsupported Index(ILM) SAM Transceiver error - problem communicating with the SAM(Key Mgr)";
 - 0X0D2: "Unexpected Sequence Counter in multiple frames for single bitmap(ILM) Length error in data returned from the SAM(Key Mgr)";
 - 0X0D22: "Improper bit map(ILM)";
 - 0X0D23: "Request Online Authorization";
 - 0X0D24: "ViVOCard3 raw data read successful";
 - 0X0D25: "Message index not available(ILM) ViVOcomm activate transaction card type(ViVOcomm)";
 - 0X0D26: "Version Information Mismatch(ILM)";
 - 0X0D27: "Not sending commands in correct index message index(ILM)";
 - 0X0D28: "Time out or next expected message not received(ILM)";
 - 0X0D29: "ILM languages not available for viewing(ILM)";
 - 0X0D2A: "Other language not supported(ILM)";
 - 0X0D41: "Unknown Error from SAM";
 - 0X0D42: "Invalid data detected by SAM";
 - 0X0D43: "Incomplete data detected by SAM";
 - 0X0D44: "Reserved";
 - 0X0D45: "Invalid key hash algorithm";
 - 0X0D46: "Invalid key encryption algorithm";
 - 0X0D47: "Invalid modulus length";
 - 0X0D48: "Invalid exponent";
 - 0X0D49: "Key already exists";
 - 0X0D4A: "No space for new RID";
 - 0X0D4B: "Key not found";
 - 0X0D4C: "Crypto not responding";
 - 0X0D4D: "Crypto communication error";
 - 0X0D4E: "Module-specific error for Key Manager";
 - 0X0D4F: "All key slots are full (maximum number of keys has been installed)";
 - 0X0D50: "Auto-Switch OK";
 - 0X0D51: "Auto-Switch failed";
 - 0X0D90: "Account DUKPT Key not exist";
 - 0X0D91: "Account DUKPT Key KSN exausted";
 - 0X0D00: "This Key had been loaded.";
 - 0X0E00: "Base Time was loaded.";
 - 0X0F00: "Encryption Or Decryption Failed.";
 - 0X1000: "Battery Low Warning (It is High Priority Response while Battery is Low.)";
 - 0X1800: "Send 'Cancel Command' after send 'Get Encrypted PIN' & 'Get Numeric' & 'Get Amount';
 - 0X1900: "Press 'Cancel' key after send 'Get Encrypted PIN' & 'Get Numeric' & 'Get Amount';
 - 0X30FF: "Security Chip is not connect";
 - 0X3000: "Security Chip is deactivation & Device is In Removal Legally State.";
 - 0X3101: "Security Chip is activation &  Device is In Removal Legally State.";
 - 0X5500: "No Admin DUKPT Key.";
 - 0X5501: "Admin  DUKPT Key STOP.";
 - 0X5502: "Admin DUKPT Key KSN is Error.";
 - 0X5503: "Get Authentication Code1 Failed.";
 - 0X5504: "Validate Authentication Code Error.";
 - 0X5505: "Encrypt or Decrypt data failed.";
 - 0X5506: "Not Support the New Key Type.";
 - 0X5507: "New Key Index is Error.";
 - 0X5508: "Step Error.";
 - 0X5509: "KSN Error";
 - 0X550A: "MAC Error.";
 - 0X550B: "Key Usage Error.";
 - 0X550C: "Mode Of Use Error.";
 - 0X550F: "Other Error.";
 - 0X6000: "Save or Config Failed / Or Read Config Error.";
 - 0X6200: "No Serial Number.";
 - 0X6900: "Invalid Command - Protocol is right, but task ID is invalid.";
 - 0X6A01: "Unsupported Command - Protocol and task ID are right, but command is invalid - In this State";
 - 0X6A00: "Unsupported Command - Protocol and task ID are right, but command is invalid.";
 - 0X6B00: "Unknown parameter in command - Protocol task ID and command are right, but parameter is invalid.";
 - 0X6C00: "Unknown parameter in command - Protocol task ID and command are right, but length is out of the requirement.";
 - 0X7200: "Device is suspend (MKSK suspend or press password suspend).";
 - 0X7300: "PIN DUKPT is STOP (21 bit 1).";
 - 0X7400: "Device is Busy.";
 - 0XE100: "Can not enter sleep mode";
 - 0XE200: "File has existed";
 - 0XE300: "File has not existed";
 - 0XE313: "IO line low -- Card error after session start";
 - 0XE400: "Open File Error";
 - 0XE500: "SmartCard Error";
 - 0XE600: "Get MSR Card data is error";
 - 0XE700: "Command time out";
 - 0XE800: "File read or write is error";
 - 0XE900: "Active 1850 error!";
 - 0XEA00: "Load bootloader error";
 - 0XEF00: "Protocol Error- STX or ETX or check error.";
 - 0XEB00: "Picture is not exist";
 - 0X2C02: "No Microprocessor ICC seated";
 - 0X2C06: "no card seated to request ATR";
 - 0X2D01: "Card Not Supported,";
 - 0X2D03: "Card Not Supported, wants CRC";
 - 0X690D: "Command not supported on reader without ICC support";
 - 0X8100: "ICC error time out on power-up";
 - 0X8200: "invalid TS character received - Wrong operation step";
 - 0X8300: "Decode MSR Error";
 - 0X8400: "TriMagII no Response";
 - 0X8500: "No Swipe MSR Card";
 - 0X8510: "No Financial Card";
 - 0X8600: "Unsupported F, D, or combination of F and D";
 - 0X8700: "protocol not supported EMV TD1 out of range";
 - 0X8800: "power not at proper level";
 - 0X8900: "ATR length too long";
 - 0X8B01: "EMV invalid TA1 byte value";
 - 0X8B02: "EMV TB1 required";
 - 0X8B03: "EMV Unsupported TB1 only 00 allowed";
 - 0X8B04: "EMV Card Error, invalid BWI or CWI";
 - 0X8B06: "EMV TB2 not allowed in ATR";
 - 0X8B07: "EMV TC2 out of range";
 - 0X8B08: "EMV TC2 out of range";
 - 0X8B09: "per EMV96 TA3 must be >  - 0XF";
 - 0X8B10: "ICC error on power-up";
 - 0X8B11: "EMV T=1 then TB3 required";
 - 0X8B12: "Card Error, invalid BWI or CWI";
 - 0X8B13: "Card Error, invalid BWI or CWI";
 - 0X8B17: "EMV TC1/TB3 conflict-";
 - 0X8B20: "EMV TD2 out of range must be T=1";
 - 0X8C00: "TCK error";
 - 0XA304: "connector has no voltage setting";
 - 0XA305: "ICC error on power-up invalid (SBLK(IFSD) exchange";
 - 0XE301: "ICC error after session start";
 - 0XFF00: "Request to go online";
 - 0XFF01: "EMV: Accept the offline transaction";
 - 0XFF02: "EMV: Decline the offline transaction";
 - 0XFF03: "EMV: Accept the online transaction";
 - 0XFF04: "EMV: Decline the online transaction";
 - 0XFF05: "EMV: Application may fallback to magstripe technology";
 - 0XFF06: "EMV: ICC detected tah the conditions of use are not satisfied";
 - 0XFF07: "EMV: ICC didn't accept transaction";
 - 0XFF08: "EMV: Transaction was cancelled";
 - 0XFF09: "EMV: Application was not selected by kernel or ICC format error or ICC missing data error";
 - 0XFF0A: "EMV: Transaction is terminated";
 - 0XFF0B: "EMV: Other EMV Error";
 - 0XFFFF: "NO RESPONSE";
 - 0XF002: "ICC communication timeout";
 - 0XF003: "ICC communication Error";
 - 0XF00F: "ICC Card Seated and Highest Priority, disable MSR work request";
 - 0XF200: "AID List / Application Data is not exist";
 - 0XF201: "Terminal Data is not exist";
 - 0XF202: "TLV format is error";
 - 0XF203: "AID List is full";
 - 0XF204: "Any CA Key is not exist";
 - 0XF205: "CA Key RID is not exist";
 - 0XF206: "CA Key Index it not exist";
 - 0XF207: "CA Key is full";
 - 0XF208: "CA Key Hash Value is Error";
 - 0XF209: "Transaction  format error";
 - 0XF20A: "The command will not be processing";
 - 0XF20B: "CRL is not exist";
 - 0XF20C: "CRL number  exceed max number";
 - 0XF20D: "Amount,Other Amount,Trasaction Type  are  missing";
 - 0XF20E: "The Identification of algorithm is mistake";
 - 0XF20F: "No Financial Card";
 - 0XF210: "In Encrypt Result state, TLV total Length is greater than Max Length";
 - 0X1001: "INVALID ARG";
 - 0X1002: "FILE_OPEN_FAILED";
 - 0X1003: "FILE OPERATION_FAILED";
 - 0X2001: "MEMORY_NOT_ENOUGH";
 - 0X3002: "SMARTCARD_FAIL";
 - 0X3003: "SMARTCARD_INIT_FAILED";
 - 0X3004: "FALLBACK_SITUATION";
 - 0X3005: "SMARTCARD_ABSENT";
 - 0X3006: "SMARTCARD_TIMEOUT";
 - 0X5001: "EMV_PARSING_TAGS_FAILED";
 - 0X5002: "EMV_DUPLICATE_CARD_DATA_ELEMENT";
 - 0X5003: "EMV_DATA_FORMAT_INCORRECT";
 - 0X5004: "EMV_NO_TERM_APP";
 - 0X5005: "EMV_NO_MATCHING_APP";
 - 0X5006: "EMV_MISSING_MANDATORY_OBJECT";
 - 0X5007: "EMV_APP_SELECTION_RETRY";
 - 0X5008: "EMV_GET_AMOUNT_ERROR";
 - 0X5009: "EMV_CARD_REJECTED";
 - 0X5010: "EMV_AIP_NOT_RECEIVED";
 - 0X5011: "EMV_AFL_NOT_RECEIVED";
 - 0X5012: "EMV_AFL_LEN_OUT_OF_RANGE";
 - 0X5013: "EMV_SFI_OUT_OF_RANGE";
 - 0X5014: "EMV_AFL_INCORRECT";
 - 0X5015: "EMV_EXP_DATE_INCORRECT";
 - 0X5016: "EMV_EFF_DATE_INCORRECT";
 - 0X5017: "EMV_ISS_COD_TBL_OUT_OF_RANGE";
 - 0X5018: "EMV_CRYPTOGRAM_TYPE_INCORRECT";
 - 0X5019: "EMV_PSE_NOT_SUPPORTED_BY_CARD";
 - 0X5020: "EMV_USER_SELECTED_LANGUAGE";
 - 0X5021: "EMV_SERVICE_NOT_ALLOWED";
 - 0X5022: "EMV_NO_TAG_FOUND";
 - 0X5023: "EMV_CARD_BLOCKED";
 - 0X5024: "EMV_LEN_INCORRECT";
 - 0X5025: "CARD_COM_ERROR";
 - 0X5026: "EMV_TSC_NOT_INCREASED";
 - 0X5027: "EMV_HASH_INCORRECT";
 - 0X5028: "EMV_NO_ARC";
 - 0X5029: "EMV_INVALID_ARC";
 - 0X5030: "EMV_NO_ONLINE_COMM";
 - 0X5031: "TRAN_TYPE_INCORRECT";
 - 0X5032: "EMV_APP_NO_SUPPORT";
 - 0X5033: "EMV_APP_NOT_SELECT";
 - 0X5034: "EMV_LANG_NOT_SELECT";
 - 0X5035: "EMV_NO_TERM_DATA";
 - 0X6001: "CVM_TYPE_UNKNOWN";
 - 0X6002: "CVM_AIP_NOT_SUPPORTED";
 - 0X6003: "CVM_TAG_8E_MISSING";
 - 0X6004: "CVM_TAG_8E_FORMAT_ERROR";
 - 0X6005: "CVM_CODE_IS_NOT_SUPPORTED";
 - 0X6006: "CVM_COND_CODE_IS_NOT_SUPPORTED";
 - 0X6007: "NO_MORE_CVM";
 - 0X6008: "PIN_BYPASSED_BEFORE";
 - 0X7001: "PK_BUFFER_SIZE_TOO_BIG";
 - 0X7002: "PK_FILE_WRITE_ERROR";
 - 0X7003: "PK_HASH_ERROR";
 - 0X8001: "NO_CARD_HOLDER_CONFIRMATION";
 - 0X8002: "GET_ONLINE_PIN";
 - 0XD000: "Data not exist";
 - 0XD001: "Data access error";
 - 0XD100: "RID not exist";
 - 0XD101: "RID existed";
 - 0XD102: "Index not exist";
 - 0XD200: "Maximum exceeded";
 - 0XD201: "Hash error";
 - 0XD205: "System Busy";
 - 0X0E01: "Unable to go online";
 - 0X0E02: "Technical Issue";
 - 0X0E03: "Declined";
 - 0X0E04: "Issuer Referral transaction";
 - 0X0F01: "Decline the online transaction";
 - 0X0F02: "Request to go online";
 - 0X0F03: "Transaction is terminated";
 - 0X0F05: "Application was not selected by kernel or ICC format error or ICC missing data error";
 - 0X0F07: "ICC didn't accept transaction";
 - 0X0F0A: "Application may fallback to magstripe technology";
 - 0X0F0C: "Transaction was cancelled";
 - 0X0F0D: "Timeout";
 - 0X0F0F: "Other EMV Error";
 - 0X0F10: "Accept the offline transaction";
 - 0X0F11: "Decline the offline transaction";
 - 0X0F21: "ICC detected tah the conditions of use are not satisfied";
 - 0X0F22: "No app were found on card matching terminal configuration";
 - 0X0F23: "Terminal file does not exist";
 - 0X0F24: "CAPK file does not exist";
 - 0X0F25: "CRL Entry does not exist";
 - 0X0FFE: "code when blocking is disabled";
 - 0X0FFF: "code when command is not applicable on the selected device";
 - 0XF005: "ICC Encrypted C-APDU Data Structure Length Error Or Format Error.";
 - 0XBBE0: "CM100 Success";
 - 0XBBE1: "CM100 Parameter Error";
 - 0XBBE2: "CM100 Low Output Buffer";
 - 0XBBE3: "CM100 Card Not Found";
 - 0XBBE4: "CM100 Collision Card Exists";
 - 0XBBE5: "CM100 Too Many Cards Exist";
 - 0XBBE6: "CM100 Saved Data Does Not Exist";
 - 0XBBE8: "CM100 No Data Available";
 - 0XBBE9: "CM100 Invalid CID Returned";
 - 0XBBEA: "CM100 Invalid Card Exists";
 - 0XBBEC: "CM100 Command Unsupported";
 - 0XBBED: "CM100 Error In Command Process";
 - 0XBBEE: "CM100 Invalid Command";

 - 0X9031: "Unknown command";
 - 0X9032: "Wrong parameter (such as the length of the command is incorrect)";

 - 0X9038: "Wait (the command couldnt be finished in BWT)";
 - 0X9039: "Busy (a previously command has not been finished)";
 - 0X903A: "Number of retries over limit";

 - 0X9040: "Invalid Manufacturing system data";
 - 0X9041: "Not authenticated";
 - 0X9042: "Invalid Master DUKPT Key";
 - 0X9043: "Invalid MAC Key";
 - 0X9044: "Reserved for future use";
 - 0X9045: "Reserved for future use";
 - 0X9046: "Invalid DATA DUKPT Key";
 - 0X9047: "Invalid PIN Pairing DUKPT Key";
 - 0X9048: "Invalid DATA Pairing DUKPT Key";
 - 0X9049: "No nonce generated";
 - 0X9949: "No GUID available.  Perform getVersion first.";
 - 0X9950: "MAC Calculation unsuccessful. Check BDK value.";

 - 0X904A: "Not ready";
 - 0X904B: "Not MAC data";

 - 0X9050: "Invalid Certificate";
 - 0X9051: "Duplicate key detected";
 - 0X9052: "AT checks failed";
 - 0X9053: "TR34 checks failed";
 - 0X9054: "TR31 checks failed";
 - 0X9055: "MAC checks failed";
 - 0X9056: "Firmware download failed";

 - 0X9060: "Log is full";
 - 0X9061: "Removal sensor unengaged";
 - 0X9062: "Any hardware problems";

 - 0X9070: "ICC communication timeout";
 - 0X9071: "ICC data error (such check sum error)";
 - 0X9072: "Smart Card not powered up";
 */
void device_getResponseCodeString(IN int returnCode, OUT char* despcrition);

/**
 * Review the return code description.<br/>
 * @param returnCode  the response result.
 * @param description
 * @retval the string for description of response result

 - 0: "no error, beginning task";
 - 1: "no response from reader";
 - 2: "invalid response data";

  - 01: " Incorrect Header Tag";
  - 02: " Unknown Command";
  - 03: " Unknown Sub-Command";
  - 04: " CRC Error in Frame";
  - 05: " Incorrect Parameter";
  - 06: " Parameter Not Supported";
  - 07: " Mal-formatted Data";
  - 08: " Timeout";
  - 0A: " Failed / NACK";
  - 0B: " Command not Allowed";
  - 0C: " Sub-Command not Allowed";
  - 0D: " Buffer Overflow (Data Length too large for reader buffer)";
  - 0E: " User Interface Event";
  - 10: " Need clear firmware(apply in boot loader only)";
  - 11: " Communication type not supported, VT-1, burst, etc. Need encrypted firmware (apply in boot loader only)";
  - 12: " Secure interface is not functional or is in an intermediate state.";
  - 13: " Data field is not mod 8";
  - 14: " Pad 0x80 not found where expected";
  - 15: " Specified key type is invalid";
  - 16: " Could not retrieve key from the SAM (InitSecureComm)";
  - 17: " Hash code problem";
  - 18: " Could not store the key into the SAM (InstallKey)";
  - 19: " Frame is too large";
  - 1A: " Unit powered up in authentication state but POS must resend the InitSecureComm command";
  - 1B: " The EEPROM may not be initialized because SecCommInterface does not make sense";
  - 1C: " Problem encoding APDU Module-Specific Status Codes ";
  - 20: " Unsupported Index (ILM) SAM Transceiver error - problem communicating with the SAM (Key Mgr)";
  - 21: " Unexpected Sequence Counter in multiple frames for single bitmap (ILM)Length error in data returned from the SAM (Key Mgr)
  - 22: " Improper bit map (ILM)";
  - 23: " Request Online Authorization";
  - 24: " ViVOCard3 raw data read successful";
  - 25: " Message index not available (ILM) ViVOcomm activate transaction card type (ViVOcomm)";
  - 26: " Version Information Mismatch (ILM)";
  - 27: " Not sending commands in correct index message index (ILM)";
  - 28: " Time out or next expected message not received (ILM)";
  - 29: " ILM languages not available for viewing (ILM)";
  - 2A: " Other language not supported (ILM)";
  - 41: " from 41 to 4F,	 Module-specific errors for Key Manager";
 
  - 50: " Auto-Switch OK";
  - 51: " Auto-Switch failed";
  - 70: " Antenna Error 80h Use another card";
  - 81: " Insert or swipe card";
  - 90: " Data encryption Key does not exist";
  - 91: " Data encryption Key KSN exhausted";

*/


void device_getIDGStatusCodeString(IN int returnCode, OUT char* despcrition);


/**
 * Check the device conntected status
 * @return DEVICE_DISCONNECT=0, or DEVICE_CONNECTED = 1
 */
int device_isConnected();
/**
* Polls device for Firmware Version
*
* @param firmwareVersion Response returned of Firmware Version
*
* @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString()
*
*/
int device_getFirmwareVersion(OUT char* firmwareVersion);


int device_getCurrentDeviceType();


/**
* Send a Command to device
*
* Sends a command  to the device .
*
* @param cmd buffer of command to execute.
* @param cmdLen, the length of the buffer cmd.
*
* @param data buffer of IDG command data.
* @param dataLen, the length of the buffer data.
*
* @param response Response data
* @param respLen, the length of Response data

* @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString()
*/

int device_SendDataCommand(IN BYTE* cmd, IN int cmdLen, IN BYTE* data, IN int dataLen, OUT BYTE* response, IN_OUT int *respLen);
/**
* Reboot Device
*Executes a command to restart the device.
- Card data is cleared, resetting card status bits.
- Response data of the previous command is cleared.
- Resetting firmware.
*
* @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString()
*/
int device_rebootDevice();
/**
 * Update Firmware
 * Updates the firmware of the Spectrum Pro K21 HUB or Maxq1050.
 * @param firmwareData Signed binary data of a firmware file provided by IDTech
 * @param firmwareDataLen Length of firmwareData
 * @param firmwareName Firmware name. Must be one of the following two strings (with appropriate version information)
 *  - "SP K21 APP Vx.xx.xxx"
 *  - "SP MAX APP Vx.xx.xxx"
 * @param encryptionType Encryption type
 - 0 : Plaintext
 - 1 : TDES ECB, PKCS#5 padding
 - 2 : TDES CBC, PKCS#5, IV is all 0
 * @param keyBlob Encrypted firmware session key blob, TR-31 Rev B, wrapped by FW Key (Optional, none if firmware is plaintext)
 * @param keyBlobLen Length of keyBlob
 * @return RETURN_CODE:  Values can be parsed with errorCode.getErrorString()
 *
 * Firmware update status is returned in the callback with the following values:
 * sender = SPECTRUM_PRO
 * state = DeviceState.FirmwareUpdate
 * data = File Progress.  Two bytes, with byte[0] = current block, and byte[1] = total blocks. 0x0310 = block 3 of 16
 * transactionResultCode:
 * - RETURN_CODE_DO_SUCCESS = Firmware Update Completed Successfully
 * - RETURN_CODE_BLOCK_TRANSFER_SUCCESS = Current block transferred successfully
 * - Any other return code represents an error condition
 *
 */
int device_updateFirmware(IN BYTE *firmwareData, IN int firmwareDataLen, IN char *firmwareName, IN int encryptionType, IN BYTE *keyBlob, IN int keyBlobLen);
/**
 * Polls device for Model Number
 *
 * @param sNumber  Returns Model Number
 *
 * @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString
 *
 */
int config_getModelNumber(OUT char* sNumber);
/**
 * Polls device for Serial Number
 *
 * @param sNumber  Returns Serial Number
 *
 * @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString
 *
 */
int config_getSerialNumber(OUT char* sNumber);

/**
* Poll Card Reader
*
Provides information about the state of the Card Reader

    @param status Six bytes indicating card reader information
    Byte 0:
     - Bit 0: Device Manufacturing CA data valid
     - Bit 1: Device Manufacturing Secure data valid
     - Bit 2: HOST_CR_MASTER_DUKPT Key valid
     - Bit 3: HOST_CR_MAC Keys valid (Authenticated)
     - Bit 4: RFU
     - Bit 5: RFU
     - Bit 6: DATA_DUKPT Key Valid
     - Bit 7: Key is initialized (MFK and RSA Key pairs)

    Byte 1:
     - Bit 0: Firmware Key Valid
     - Bit 1: RFU
     - Bit 2: CR_PINPAD_MASTER_DUKPT Key valid
     - Bit 3: CR_PINPAD_MAC Keys valid (Authenticated)
     - Bit 4: DATA Pairing DUKPT Key valid
     - Bit 5: PIN Pairing DUKPT Key Valid
     - Bit 6: RFU
     - Bit 7: RFU

    Byte 2:
     - Bit 0: RFU
     - Bit 1: Tamper Switch #1 Error
     - Bit 2: Battery Backup Error
     - Bit 3: Temperature Error
     - Bit 4: Voltage Sensor Error
     - Bit 5: Firmware Authentication Error
     - Bit 6: Tamper Switch #2 Error
     - Bit 7: Removal Tamper Error

    Byte 3:
     - Battery Voltage (example 0x32 = 3.2V, 0x24 = 2.4V)

    Byte 4:
     - Bit 0: Log is Full
     - Bit 1: Mag Data Present
     - Bit 2: Card Insert
     - Bit 3: Removal Sensor connected
     - Bit 4: Card Seated
     - Bit 5: Latch Mechanism Active
     - Bit 6: Removal Sensor Active
     - Bit 7: Tamper Detector Active

    Byte 5:
     - Bit 0: SAM Available
     - Bit 1: Chip Card Reader Available
     - Bit 2: Host Connected
     - Bit 3: Contactless Available
     - Bit 4: PINPAD connected
     - Bit 5: MSR Header connected
     - Bit 6: RFU
     - Bit 7: Production Unit

* @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString()
*/
int device_pollCardReader(OUT BYTE* status);
/**
 * Get DUKPT KSN
 *
 * Returns the KSN for the provided key index
  *
  * @param type Key type:
   - 0: Key Encryption Key (Master Key or KEK)
   - 2: Data Encryption Key (DEK)
   - 5: MAC Key (MAK)
   - 10: RKL Key Encryption Key (REK)
   - 20: HSM DUKPT Key

 * @param KSN  Key Serial Number

 * @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString()

 */
int device_getSpectrumProKSN(IN int type, OUT BYTE* KSN);
/**
  * Calibrate reference parameters
  * @param delta Delta value (0x02 standard default value)
  * @return RETURN_CODE:  Values can be parsed with device_getErrorString()
  */
int device_calibrateParameters(BYTE delta);
/**
 * Power On ICC
 *
 * Power up the currently selected microprocessor card in the ICC reader
 *@param ATR, the ATR data response when succeeded power on ICC,
 *@param inLen, the length of ATR data
 *
 * @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString()
 */
int icc_powerOnICC(OUT BYTE* ATR, IN_OUT int* inLen);
/**
 * Power Off ICC
 *
 * Powers down the ICC
 *
 * @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString()

 If Success, empty
 If Failure, ASCII encoded data of error string
 */
int icc_powerOffICC();
/**
 *
 * Get Reader Status
 * Returns the reader status
 * @param status Pointer that will return with the ICCReaderStatus results.
   bit 0:  0 = ICC Power Not Ready, 1 = ICC Powered
   bit 1:  0 = Card not seated, 1 = card seated
 * @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString
 */
int icc_getICCReaderStatus(OUT BYTE *status);
/**
 * Polls device for EMV Kernel Version
 *
 * @param version Response returned of Kernel Version
 *
 * @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString()
 *
 */
int emv_getEMVKernelVersion(OUT char* version);
/**
* Get EMV Kernel check value info
*
* @param checkValue Response returned of Kernel check value info
* @param checkValueLen the length of checkValue
*
* @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString
*
*/
int emv_getEMVKernelCheckValue(OUT BYTE* checkValue, IN_OUT int *checkValueLen);
/**
 * Get EMV Kernel configuration check value info
 *
 * @param checkValue Response returned of Kernel configuration check value info
 * @param checkValueLen the length of checkValue
 * @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString
 *
 */
int emv_getEMVConfigurationCheckValue(OUT BYTE* checkValue, IN_OUT int *checkValueLen);
/**
Allow fallback for EMV transactions.  Default is TRUE
@param allow TRUE = allow fallback, FALSE = don't allow fallback
*/
void emv_allowFallback(IN int allow);

    
/**
* Start EMV Transaction Request
*
Authorizes the EMV transaction for an ICC card

The tags will be returned in the callback routine.

@param amount Transaction amount value  (tag value 9F02) - SEE IMPORTANT NOTE BELOW
@param amtOther Other amount value, if any  (tag value 9F03) - SEE IMPORTANT NOTE BELOW
@param exponent Number of characters after decimile point
@param type Transaction type (tag value 9C).
@param timeout Timeout value in seconds.
@param tags Any other tags to be included in the request (Maximum Length = 500 bytes).  Passed as a byte array.  Example, tag 9F0C with amount 0x000000000100 would be 0x9F0C06000000000100
    If tags 9F02 (amount),9F03 (other amount), or 9C (transaction type) are included, they will take priority over these values supplied as individual parameters to this method.
@param tagsLen The length of tags
@param forceOnline TRUE = do not allow offline approval,  FALSE = allow ICC to approve offline if terminal capable
Note:  To request tags to be  included in default response, use tag DFEE1A, and specify tag list.  Example four tags 9F02, 9F36, 95, 9F37 to be included in response = DFEE1A079F029F369f9F37

@return RETURN_CODE:  Values can be parsed with device_getResponseCodeString

>>>>>IMPORTANT: parameters for amount and amtOther MUST BE PASSED AS A DOUBLE VALUE WITH DECIMAL POINT.  Example, do not pass 1, but instead pass 1.0 or 1.00. Otherwise, results will be unpredictable
 */
int emv_startTransaction(IN double amount, IN double amtOther, IN int exponent, IN int type, IN int timeout, IN BYTE* tags, IN int tagsLen, IN int forceOnline);
/**
* Authenticate EMV Transaction Request
*
Authenticates the EMV transaction for an ICC card.  Execute this after receiving response with result code 0x10 to emv_startTransaction

The tags will be returned in the callback routine.

 * @param updatedTLV  TLV stream that can be used to update the following values:
     - 9F02: Amount
     - 9F03: Other amount
     - 9C: Transaction type
     - 5F57: Account type
    In addition tag DFEE1A can be sent to specify tag list to include in results. Example four tags 9F02, 9F36, 95, 9F37 to be included in response = DFEE1A079F029F36959F37
 * @param updatedTLVLen
 * @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString
*/
int emv_authenticateTransaction(IN BYTE* updatedTLV, IN int updatedTLVLen);
/**
* Complete EMV Transaction Request
*
*
* Completes the EMV transaction for an ICC card when online authorization request is received from emv_authenticateTransaction
*
* The tags will be returned in the callback routine.

* @param commError Communication error with host.  Set to TRUE(1) if host was unreachable, or FALSE(0) if host response received.  If Communication error, authCode, iad, tlvScripts can be null.
* @param authCode Authorization code from host.  Two bytes.  Example 0x3030.  (Tag value 8A).  Required
* @param authCodeLen the length of authCode
* @param iad Issuer Authentication Data, if any.  Example 0x11223344556677883030 (tag value 91).
* @param iadLen the length of iadLen
* @param tlvScripts 71/72 scripts, if any
* @param tlvScriptsLen the length of tlvScriptsLen
* @param tlv  Additional TVL data to return with transaction results (if any)
* @param tlvLen the length of tlv
* @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString
*/
int emv_completeTransaction(IN int commError, IN BYTE* authCode, IN int authCodeLen,
		IN BYTE* iad, IN int iadLen, IN BYTE* tlvScripts, IN int tlvScriptsLen, IN BYTE* tlv, IN int tlvLen);
/**
* Cancel EMV Transaction
*
* Cancels the currently executing EMV transaction.
* @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString()
*/
int emv_cancelTransaction();

/**
 * Retrieve Transaction Results

 *
 Retrieves specified EMV tags from the currently executing transaction.

 * @param tags Tags to be retrieved.  Example 0x9F028A will retrieve tags 9F02 and 8A
 * @param tagsLen Length of tag list
 * @param cardData All requested tags returned as unencrypted, encrypted and masked TLV data in IDTTransactionData object

 * @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString()

 */
int emv_retrieveTransactionResult(IN BYTE* tags, IN int tagsLen, IDTTransactionData * cardData);
/**
* Callback Response LCD Display
*
Provides menu selection responses to the kernel after a callback was received with DeviceState.EMVCallback, and
callbackType = EMV_CALLBACK_TYPE.EMV_CALLBACK_TYPE_LCD, and lcd_displayMode = EMV_LCD_DISPLAY_MODE_MENU, EMV_LCD_DISPLAY_MODE_PROMPT,
or EMV_LCD_DISPLAY_MODE_LANGUAGE_SELECT

* @param type If Cancel key pressed during menu selection, then value is EMV_LCD_DISPLAY_MODE_CANCEL.  Otherwise, value can be EMV_LCD_DISPLAY_MODE_MENU, EMV_LCD_DISPLAY_MODE_PROMPT,
or EMV_LCD_DISPLAY_MODE_LANGUAGE_SELECT
* @param selection If type = EMV_LCD_DISPLAY_MODE_MENU or EMV_LCD_DISPLAY_MODE_LANGUAGE_SELECT, provide the selection ID line number. Otherwise, if type = EMV_LCD_DISPLAY_MODE_PROMPT
supply either 0x43 ('C') for Cancel, or 0x45 ('E') for Enter/accept

* @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString()

*/
int emv_callbackResponseLCD(IN int type, byte selection);
/**
* Callback Response MSR Entry
*
Provides MSR information to kernel after a callback was received with DeviceState.EMVCallback, and
    callbackType = EMV_CALLBACK_MSR

* @param MSR Swiped track data
* @param MSRLen the length of Swiped track data

* @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString()
*/
int emv_callbackResponseMSR(IN BYTE* MSR, IN_OUT int MSRLen);
/**
 * Retrieve Application Data by AID
 *
 Retrieves the Application Data as specified by the AID name passed as a parameter.

 * @param AID Name of ApplicationID. Must be between 5 and 16 bytes
 * @param AIDLen the length of AID data buffer.
 * @param tlv  The TLV elements of the requested AID
 * @param tlvLen the length of tlv data buffer.

 * @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString()
 */
int emv_retrieveApplicationData(IN BYTE* AID, IN int AIDLen, OUT BYTE* tlv, IN_OUT int *tlvLen);

/**
 * Set Application Data by AID
 *
 Sets the Application Data as specified by the application name and TLV data

 * @param name Application name, 10-32 ASCII hex characters representing 5-16 bytes  Example "a0000000031010"
 * @param nameLen the length of name data buffer of Application name,
 * @param tlv  Application data in TLV format
 * @param tlvLen the length of tlv data buffer
 * @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString()

 */
int emv_setApplicationData(IN BYTE* name, IN int nameLen, IN BYTE* tlv, IN int tlvLen);
/**
 * Set Application Data by TLV
 *
 * Sets the Application Data as specified by the TLV data
 * @param tlv  Application data in TLV format
 * 		The first tag of the TLV data must be the group number (DFEE2D).
 * 		The second tag of the TLV data must be the AID (9F06)
 * 		Example valid TLV, for Group #2, AID a0000000035010:
 * 		"dfee2d01029f0607a0000000051010ffe10101ffe50110ffe30114ffe20106"
 * @param tlvLen the length of tlv data buffer
 * @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString()
 */
int emv_setApplicationDataTLV(IN BYTE* tlv, IN int tlvLen);
/**
 * Remove Application Data by AID
 *
 Removes the Application Data as specified by the AID name passed as a parameter

 * @param AID Name of ApplicationID Must be between 5 and 16 bytes
 * @param AIDLen the length of AID data buffer
 * @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString()
 */
int emv_removeApplicationData(IN BYTE* AID, IN int AIDLen);
/**
 * Remove All Application Data
*
Removes all the Application Data


* @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString()

*/
int emv_removeAllApplicationData();
/**
 * Retrieve AID list
 *
 Returns all the AID names installed on the terminal.

 * @param AIDList  array of AID name byte arrays
 * @param AIDListLen  the length of AIDList array buffer

 * @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString()

 */
int emv_retrieveAIDList(OUT BYTE * AIDList, IN_OUT int *AIDListLen);

/**
 * Retrieve Terminal Data
 *
 Retrieves the Terminal Data.

 * @param tlv Response returned as a TLV
 * @param tlvLen the length of tlv data buffer
 * @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString()


 */
int emv_retrieveTerminalData(OUT BYTE* tlv, IN_OUT int *tlvLen);
/**
* Set Terminal Data
*
Sets the Terminal Data as specified by the TerminalData structure passed as a parameter

 * @param tlv TerminalData configuration file
 * @param tlvLen the length of tlv data buffer

 * @retval RETURN_CODE:  Return codes listed as typedef enum in IDTCommon:RETURN_CODE.  Values can be parsed with device_getResponseCodeString:()

 */
int emv_setTerminalData(IN BYTE* tlv, IN int tlvLen);
/**
 Sets the terminal major configuration in ICS .
 @param configuration A configuration value, range 1-5
 - 1 = 1C
 - 2 = 2C
 - 3 = 3C
 - 4 = 4C
 - 5 = 5C

 *  @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString()
 */
int emv_setTerminalMajorConfiguration(IN int configuration);
/**
 * Remove Terminal Data
 *
 Removes the Terminal Data

* @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString()

 */
int emv_removeTerminalData();
/**
 * Retrieve Certificate Authority Public Key
 *
 Retrieves the CAPK as specified by the RID/Index  passed as a parameter.

 * @param capk 6 bytes CAPK = 5 bytes RID + 1 byte Index
 * @param capkLen the length of capk data buffer
 * @param key Response returned as a CAKey format:
    [5 bytes RID][1 byte Index][1 byte Hash Algorithm][1 byte Encryption Algorithm]
    [20 bytes HashValue][4 bytes Public Key Exponent][2 bytes Modulus Length][Variable bytes Modulus]
    Where:
     - Hash Algorithm: The only algorithm supported is SHA-1.The value is set to 0x01
     - Encryption Algorithm: The encryption algorithm in which this key is used. Currently support only one type: RSA. The value is set to 0x01.
     - HashValue: Which is calculated using SHA-1 over the following fields: RID & Index & Modulus & Exponent
     - Public Key Exponent: Actually, the real length of the exponent is either one byte or 3 bytes. It can have two values: 3 (Format is 0x00 00 00 03), or  65537 (Format is 0x00 01 00 01)
     - Modulus Length: LenL LenH Indicated the length of the next field.
     - Modulus: This is the modulus field of the public key. Its length is specified in the field above.
 * @param keyLen the length of key data buffer
 * * @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString()
 */
int emv_retrieveCAPK(IN BYTE * capk, IN int capkLen, OUT BYTE * key, IN_OUT int *keyLen);
/**
 * Set Certificate Authority Public Key
 *
 Sets the CAPK as specified by the CAKey structure

 * @param capk CAKey format:
    [5 bytes RID][1 byte Index][1 byte Hash Algorithm][1 byte Encryption Algorithm][20 bytes HashValue][4 bytes Public Key Exponent][2 bytes Modulus Length][Variable bytes Modulus]
    Where:
     - Hash Algorithm: The only algorithm supported is SHA-1.The value is set to 0x01
     - Encryption Algorithm: The encryption algorithm in which this key is used. Currently support only one type: RSA. The value is set to 0x01.
     - HashValue: Which is calculated using SHA-1 over the following fields: RID & Index & Modulus & Exponent
     - Public Key Exponent: Actually, the real length of the exponent is either one byte or 3 bytes. It can have two values: 3 (Format is 0x00 00 00 03), or  65537 (Format is 0x00 01 00 01)
     - Modulus Length: LenL LenH Indicated the length of the next field.
     - Modulus: This is the modulus field of the public key. Its length is specified in the field above.
 * @param capkLen the length of capk data buffer
 * @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString()

 */
int emv_setCAPK(IN BYTE * capk, IN int capkLen);
/**
 * Remove Certificate Authority Public Key
 *
 Removes the CAPK as specified by the RID/Index

 * @param capk 6 byte CAPK =  5 bytes RID + 1 byte INDEX
 * @param capkLen the length of capk data buffer
 * @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString()

 */
int emv_removeCAPK(IN BYTE * capk, IN int capkLen);
/**
* Remove All Certificate Authority Public Key
 *
 Removes all the CAPK

* @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString()

*/
int emv_removeAllCAPK();
/**
 * Retrieve the Certificate Authority Public Key list
 *
 Returns all the CAPK RID and Index installed on the terminal.

 * @param keys [key1][key2]...[keyn], each key 6 bytes where
    key = 5 bytes RID + 1 byte index
 * @param keysLen the length of keys data buffer
 * @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString()
 */
int emv_retrieveCAPKList(OUT BYTE * keys, IN_OUT int *keysLen);
/**
Gets the terminal ID as printable characters .
* @param terminalID Terminal ID string

* @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString()
*/
int emv_retrieveTerminalID(OUT char* terminalID);
/**
Sets the terminal ID as printable characters .
* @param terminalID Terminal ID to set

* @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString()
*/
int emv_setTerminalID(IN char* terminalID);
/**
 * Retrieve the Certificate Revocation List
 *
 Returns the CRL entries on the terminal.

 * @param list [CRL1][CRL2]...[CRLn], each CRL 9 bytes where
    CRL = 5 bytes RID + 1 byte index + 3 bytes serial number
 * @param lssLen the length of list data buffer
 * @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString()
 */
int emv_retrieveCRL(OUT BYTE* list, IN_OUT int* lssLen);

/**
 * Set Certificate Revocation List
 *
 Sets the CRL

 * @param list CRL Entries containing the RID, Index, and serial numbers to set
    [CRL1][CRL2]...[CRLn] where each [CRL]  is 9 bytes:
    [5 bytes RID][1 byte CAPK Index][3 bytes serial number]
 * @param lssLen the length of list data buffer
 * @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString()

 */
int emv_setCRL(IN BYTE* list, IN int lsLen);

/**
 * Retrieve the Certificate Revocation List
 *
 * Returns the CRL entries on the terminal.

 * @param list [CRL1][CRL2]...[CRLn], each CRL 9 bytes where
    CRL = 5 bytes RID + 1 byte index + 3 bytes serial number
 * @param lssLen the length of list data buffer
 * @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString()
 */
int emv_removeCRL(IN BYTE* list, IN int lsLen);
/**
 * Remove All Certificate Revocation List Entries
 *
 * Removes all CRLEntry entries


 * @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString()

*/
int emv_removeAllCRL();

/**
 * Clear MSR Data
 * Clears the MSR Data buffer
 * @return RETURN_CODE:  Values can be parsed with device_getErrorString()
 */
int msr_clearMSRData();
/**
 * Get MSR Data
 * Reads the MSR Data buffer
 * @param card Card data
 * @return RETURN_CODE:  Values can be parsed with device_getErrorString()
 */
int msr_getMSRData(OUT BYTE* reData, IN_OUT int *reLen);
/**
 * Disable MSR Swipe
 * Cancels MSR swipe request.
 * @return RETURN_CODE:  Values can be parsed with device_getErrorString()
 */
int msr_cancelMSRSwipe();
/**
 * Start MSR Swipe
 * Enables MSR, waiting for swipe to occur. Allows track selection. Returns IDTMSRData instance to deviceDelegate::swipeMSRData:()
 * @param timeout Swipe Timeout Value
 * @return RETURN_CODE:  Values can be parsed with device_getErrorString()
 */
int msr_startMSRSwipe(IN int _timeout);
/**
  * Flush Track Data
  * Clears any track data being retained in memory by future PIN Block request.
  * @return RETURN_CODE:  Values can be parsed with device_getErrorString()
  */
int msr_flushTrackData();
/**
 * Parser the MSR data from the buffer into IDTMSTData structure
 *@param resData MSR card data buffer
 *@param resLen the length of resData
 *@param cardData the parser result with IDTMSTData structure
 */
void parseMSRData(IN BYTE *resData, IN int resLen, IN_OUT IDTMSRData *cardData);

/**
* Get Encrypted PIN

* Requests PIN Entry
* @param mode
- 0x00- Cancel: Cancels PIN entry = also can execute pin_cancelPINEntry(). All other parameters for this method will be ignored
- 0x01- Online PIN DUKPT
- 0x02- Online PIN MKSK
- 0x03- Offline PIN  (No need to define PAN Source or ICC PAN)
* @param PANSource
- 0x00- ICC:  PAN Captured from ICC and must be provided in iccPAN parameter
- 0x01- MSR:  PAN Captured from MSR swipe and will be inserted by Spectrum Pro. No need to provide iccPAN parameter.
* @param iccPAN  PAN captured from ICC.  When PAN captured from MSR, this parameter will be ignored
* @param iccPANLen  the length of iccPAN
* @param startTimeout  The amount of time allowed to start PIN entry before timeout
* @param entryTimeout  The amount of time to enter the PIN after first digit selected before timeout
* @param language  Valid values "EN" for English, "ES" for Spanish, "ZH" for Chinese, "FR" for French
* @param languageLen the length of language
* @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString()
*
*/
int pin_getPIN(IN int mode, IN int PANSource, IN char* iccPAN, IN int iccPANLen, IN int startTimeout, IN int entryTimeout, IN char* language, IN int languageLen);
/**
 * Cancel PIN Entry
 *
 Cancels PIN entry request
 *
 * @return RETURN_CODE:  Values can be parsed with device_getResponseCodeString()
 */
int pin_cancelPINEntry();

/**
 * Parse the PIN block data from the buffer into IDTPINData structure
 *@param resData PIN card data buffer
 *@param resLen the length of resData
 *@param cardData the parser result with IDTPINData structure
 */
void parsePINBlockData(IN BYTE *resData, IN int resLen, IN_OUT IDTPINData *cardData);

/**
 * Parse the PIN data from the buffer into IDTPINData structure
 *@param resData PIN card data buffer
 *@param resLen the length of resData
 *@param cardData the parser result with IDTPINData structure
 */
void parsePINData(IN BYTE *resData, IN int resLen, IN_OUT IDTPINData *cardData);

#ifdef __cplusplus
}
#endif


#endif

/*! \file libIDT_SpectrumPro.h
 \brief SpectrumPro API.

 Spectrum Pro Global API methods.
 */

/*! \def IN
  INPUT parameter.
 */

/*! \def OUT
  OUTPUT parameter.
 */

/*! \def IN_OUT
  INPUT / OUTPUT PARAMETER.
 */

/*! \def _DATA_BUF_LEN
 DATA BUFFER LENGTH
 */
