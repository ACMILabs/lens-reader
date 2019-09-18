//IDTDef.h

#ifndef __IDTECHSDK_DEF_H___
#define __IDTECHSDK_DEF_H___

#ifndef LIBPOS_TYPE_H
#define LIBPOS_TYPE_H

//#ifndef int
//#define int   char
//#endif

#ifndef null
#define null    0
#endif

#ifndef NULL
#define NULL    0
#endif

#define IN
#define OUT
#define IN_OUT

typedef unsigned char BYTE;
typedef unsigned char byte;
typedef void  *HANDLE;

#endif //LIBPOS_TYPE_H

#ifndef __cplusplus
#ifndef _BOOL
#ifndef __MACH__


#endif
#endif
#endif

#define USB_CONNECTION_MAX 32
//#define SHARED_SYMBOLS extern
#ifdef __MACH__
  #include <stdbool.h>
  #include "TargetConditionals.h"
  //    #define INLINE inline
  //    #define EXPORT_SYMBOLS extern
  //    #define SHARED_SYMBOLS extern
#endif

#ifdef __linux
  //    #define INLINE inline
  //    #define EXPORT_SYMBOLS extern
  //    #define SHARED_SYMBOLS extern
#endif

#if defined(WIN32) || defined(_WIN32)
  #include <windows.h>

  #define INLINE//                      __inline
  //#define EXPORT_SYMBOLS              extern
  #define EXPORT_SYMBOLS                __declspec(dllexport)
  #define SHARED_SYMBOLS                __declspec(dllexport)
  #define STDCALL_L2                    __stdcall

  #define USE_WINSCARD_API

  typedef unsigned char DL_UINT8;
  typedef unsigned short DL_UINT16;
#else
  #ifdef NEO2
    #define INLINE
    #define EXPORT_SYMBOLS
    #define SHARED_SYMBOLS
  #else
    #define INLINE inline
    #define EXPORT_SYMBOLS extern
    #define SHARED_SYMBOLS extern
  #endif

  typedef unsigned char DL_UINT8;
  typedef unsigned int DL_UINT16;
#endif

#define _DATA_BUF_LEN 1024
#define _CMD_BUF_LEN 1024
#define _FIRMWARE_BUF_LEN 1700000
#define _FIRMWARE_CMD_LEN 10000
#define _MAX_TAG_COUNT 100
#define _CMD_DATA_MAX_LEN 4096
#define _LOG_MESSAGE_LEN 8192
#define _MAX_SYMBOL_LENGTH 1024
#define __FILE_NAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define kDL_ISO8583_MAX_FIELD_IDX 128
#define MAXIMUM_FIELD_USAGE_LENGTH 1024
#define MAX_LOG_BUFFER_SIZE 8192

#define HANDLE_LCD_MSG 0

#if defined(WIN32) || defined(_WIN32) || defined(__MACH__)
  SHARED_SYMBOLS int isPINBusy;
  SHARED_SYMBOLS int isMSRBusy;
  extern BYTE logLevel;
  extern BYTE logEnabled;
  extern int outputType;
#else
  EXPORT_SYMBOLS int isPINBusy;
  EXPORT_SYMBOLS int isMSRBusy;
  EXPORT_SYMBOLS BYTE logLevel;
  EXPORT_SYMBOLS BYTE logEnabled;
  EXPORT_SYMBOLS int outputType;
#endif

//ver: 1.00

enum DEVICE_TYPE
{
  IDT_DEVICE_UNKNOWN=0,
  IDT_DEVICE_AUGUSTA_HID,
  IDT_DEVICE_AUGUSTA_KB,
  IDT_DEVICE_AUGUSTA_S_HID,
  IDT_DEVICE_AUGUSTA_S_KB,
  IDT_DEVICE_AUGUSTA_S_TTK_HID,
  IDT_DEVICE_SPECTRUM_PRO,
  IDT_DEVICE_MINISMART_II,
  IDT_DEVICE_L100,
  IDT_DEVICE_UNIPAY,
  IDT_DEVICE_UNIPAY_I_V,
  IDT_DEVICE_VP3300_AJ,
  IDT_DEVICE_KIOSK_III,
  IDT_DEVICE_KIOSK_III_S,
  IDT_DEVICE_VENDI,
  IDT_DEVICE_VP3300_USB,
  IDT_DEVICE_UNIPAY_I_V_TTK,
  IDT_DEVICE_VP3300_BT,
  IDT_DEVICE_VP8800,
  IDT_DEVICE_NEO2,
  IDT_DEVICE_MINISMART_II_COM = IDT_DEVICE_NEO2+5,
  IDT_DEVICE_SPECTRUM_PRO_COM,
  IDT_DEVICE_KIOSK_III_COM,
  IDT_DEVICE_KIOSK_III_S_COM,
  IDT_DEVICE_NEO2_COM,
  IDT_DEVICE_MAX_DEVICES = IDT_DEVICE_NEO2_COM+5
};

enum DEVICE_INTERFACE
{
  DEVICE_INTERFACE_UNKNOWN=0,
  PROTOCOL_USBHID,
  PROTOCOL_USBKB,
  PROTOCOL_SERIAL,
  PROTOCOL_BLUETOOTH
};

enum DEVICE_PROTOCOL
{
  DEVICE_PROTOCOL_UNKNOWN=0,
  PROTOCOL_NGA,
  PROTOCOL_IDG,
  PROTOCOL_ITP
};

enum DEVICE_STATUS
{
  DEVICE_DISCONNECT=0,
  DEVICE_CONNECTED,
  DEVICE_FIRMWARE_UPDATE
};

enum DEVICE_HOT_PLUG_MSG
{
  DEVICE_PLUGGED_OUT=0,
  DEVICE_PLUGGED_IN
};

enum EMV_LCD_DISPLAY_MODE
{
  EMV_LCD_DISPLAY_MODE_CANCEL = 0,
  EMV_LCD_DISPLAY_MODE_MENU = 1,
  EMV_LCD_DISPLAY_MODE_PROMPT = 2,
  EMV_LCD_DISPLAY_MODE_MESSAGE = 3,
  EMV_LCD_DISPLAY_MODE_PAUSE = 4,
  EMV_LCD_DISPLAY_MODE_LANGUAGE_SELECT = 8,
  EMV_LCD_DISPLAY_MODE_CLEAR_SCREEN = 16
};

enum EMV_LCD_DISPLAY_MESSAGE_MODE
{
  EMV_LCD_DISPLAY_MESSAGE_MODE_1 = 0,
  EMV_LCD_DISPLAY_MESSAGE_MODE_2 = 1,
  EMV_LCD_DISPLAY_MESSAGE_MODE_3 = 2,
  EMV_LCD_DISPLAY_MESSAGE_MODE_4 = 3
};

enum MSR_callBack_type
{
  MSR_callBack_type_ERR=1,
  MSR_callBack_type_RETURN_CODE,
  MSR_callBack_type_TIMEOUT,
  MSR_callBack_type_CARD_READ_ERR,
  MSR_callBack_type_TERMINATED,
  MSR_callBack_type_FALLBACK_TO_CONTACT,
  MSR_callBack_type_ERR_CODE
};

enum PIN_callBack_type
{
  PIN_callBack_type_ERR=1,
  PIN_callBack_type_RETURN_CODE,
  PIN_callBack_type_TIMEOUT,
  PIN_callBack_type_ERR_CODE,
  PIN_callBack_type_KEY_INPUT,
  PIN_callBack_type_Cancelled
};

enum LCD_callBack_type
{
  LCD_callBack_type_ERR=1,
  LCD_callBack_type_RETURN_CODE,
  LCD_callBack_type_TIMEOUT,
  LCD_callBack_type_ERR_CODE,
  LCD_callBack_type_BUTTON_EVENT,
  LCD_callBack_type_Cancelled
};

/**
 * Define Card type.
 */
enum CAPTURE_ENCODE_TYPE
{
  CAPTURE_ENCODE_TYPE_ISOABA,
  CAPTURE_ENCODE_TYPE_AAMVA,
  CAPTURE_ENCODE_TYPE_Unknown,
  CAPTURE_ENCODE_TYPE_Other,
  CAPTURE_ENCODE_TYPE_Raw,
  CAPTURE_ENCODE_TYPE_Jis_II,
  CAPTURE_ENCODE_TYPE_Jis_I,
  CAPTURE_ENCODE_TYPE_Jis_II_Security,
  CAPTURE_ENCODE_TYPE_Contactless_Visa_Kernel1,
  CAPTURE_ENCODE_TYPE_Contactless_MasterCard,
  CAPTURE_ENCODE_TYPE_Contactless_Visa_Kernel3,
  CAPTURE_ENCODE_TYPE_Contactless_AmericanExpress,
  CAPTURE_ENCODE_TYPE_Contactless_JCB,
  CAPTURE_ENCODE_TYPE_Contactless_Discover,
  CAPTURE_ENCODE_TYPE_Contactless_UnionPay,
  CAPTURE_ENCODE_TYPE_Contactless_Others,
  CAPTURE_ENCODE_TYPE_Manual_Entry_Enhanced_Mode,
  CAPTURE_ENCODE_TYPE_JisI_II
};

/**
 * Define encrypted type.
 */
enum CAPTURE_ENCRYPT_TYPE
{
  CAPTURE_ENCRYPT_TYPE_TDES,
  CAPTURE_ENCRYPT_TYPE_AES,
  CAPTURE_ENCRYPT_TYPE_TRANSARMOR,
  CAPTURE_ENCRYPT_TYPE_VOLTAGE,
  CAPTURE_ENCRYPT_TYPE_VISA_FPE,
  CAPTURE_ENCRYPT_TYPE_VERIFONE_FPE,
  CAPTURE_ENCRYPT_TYPE_NONE
};

enum EMV_PIN_MODE
{
  EMV_PIN_MODE_CANCEL=0,
  EMV_PIN_MODE_ONLINE_DUKPT=1,
  EMV_PIN_MODE_ONLINE_MKSK=2,
  EMV_PIN_MODE_OFFLINE=3
};

enum EMV_CALLBACK_TYPE
{
  EMV_CALLBACK_TYPE_LCD=1,
  EMV_CALLBACK_TYPE_PINPAD=2,
  EMV_CALLBACK_MSR=3,
  EMV_callBack_type_ERR,
  EMV_callBack_type_RETURN_CODE
};

/**
* Define EMV_CALLBACK_TYPES.
*/
enum EMV_ENCRYPTION_MODE
{
  EMV_ENCRYPTION_MODE_TDES = 0,
  EMV_ENCRYPTION_MODE_AES = 1
};

enum EMV_RESULT_CODE
{
  EMV_RESULT_CODE_APPROVED_OFFLINE = 0,
  EMV_RESULT_CODE_DECLINED_OFFLINE = 1,
  EMV_RESULT_CODE_APPROVED = 2,
  EMV_RESULT_CODE_DECLINED = 3,
  EMV_RESULT_CODE_GO_ONLINE = 4,
  EMV_RESULT_CODE_CALL_YOUR_BANK = 5,
  EMV_RESULT_CODE_NOT_ACCEPTED = 6,
  EMV_RESULT_CODE_FALLBACK_TO_MSR = 7,
  EMV_RESULT_CODE_TIMEOUT = 8,
  EMV_RESULT_CODE_GO_ONLINE_CTLS = 9,
  EMV_RESULT_CODE_AUTHENTICATE_TRANSACTION    = 0x0010,
  EMV_RESULT_CODE_TRANSACTION_CANCELED        = 0x0012,
  EMV_RESULT_CODE_SWIPE_NON_ICC = 0x11,
  EMV_RESULT_CODE_CTLS_TWO_CARDS = 0x7A,
  EMV_RESULT_CODE_CTLS_TERMINATE = 0x7E,
  EMV_RESULT_CODE_CTLS_TERMINATE_TRY_ANOTHER = 0x7D,
  EMV_RESULT_CODE_UNABLE_TO_REACH_HOST = 0xFF,
  EMV_RESULT_CODE_FILE_ARG_INVALID = 0x1001,
  EMV_RESULT_CODE_FILE_OPEN_FAILED = 0x1002,
  EMV_RESULT_CODE_FILE_OPERATION_FAILED = 0x1003,
  EMV_RESULT_CODE_MEMORY_NOT_ENOUGH = 0x2001,
  EMV_RESULT_CODE_SMARTCARD_OK = 0x3001,
  EMV_RESULT_CODE_SMARTCARD_FAIL = 0x3002,
  EMV_RESULT_CODE_SMARTCARD_INIT_FAILED = 0x3003,
  EMV_RESULT_CODE_FALLBACK_SITUATION = 0x3004,
  EMV_RESULT_CODE_SMARTCARD_ABSENT = 0x3005,
  EMV_RESULT_CODE_SMARTCARD_TIMEOUT = 0x3006,
  EMV_RESULT_CODE_MSR_CARD_ERROR = 0x3007,
  EMV_RESULT_CODE_PARSING_TAGS_FAILED = 0x5001,
  EMV_RESULT_CODE_CARD_DATA_ELEMENT_DUPLICATE = 0x5002,
  EMV_RESULT_CODE_DATA_FORMAT_INCORRECT = 0x5003,
  EMV_RESULT_CODE_APP_NO_TERM = 0x5004,
  EMV_RESULT_CODE_APP_NO_MATCHING = 0x5005,
  EMV_RESULT_CODE_MANDATORY_OBJECT_MISSING = 0x5006,
  EMV_RESULT_CODE_APP_SELECTION_RETRY = 0x5007,
  EMV_RESULT_CODE_AMOUNT_ERROR_GET = 0x5008,
  EMV_RESULT_CODE_CARD_REJECTED = 0x5009,
  EMV_RESULT_CODE_AIP_NOT_RECEIVED = 0x5010,
  EMV_RESULT_CODE_AFL_NOT_RECEIVED = 0x5011,
  EMV_RESULT_CODE_AFL_LEN_OUT_OF_RANGE = 0x5012,
  EMV_RESULT_CODE_SFI_OUT_OF_RANGE = 0x5013,
  EMV_RESULT_CODE_AFL_INCORRECT = 0x5014,
  EMV_RESULT_CODE_EXP_DATE_INCORRECT = 0x5015,
  EMV_RESULT_CODE_EFF_DATE_INCORRECT = 0x5016,
  EMV_RESULT_CODE_ISS_COD_TBL_OUT_OF_RANGE = 0x5017,
  EMV_RESULT_CODE_CRYPTOGRAM_TYPE_INCORRECT = 0x5018,
  EMV_RESULT_CODE_PSE_BY_CARD_NOT_SUPPORTED = 0x5019,
  EMV_RESULT_CODE_USER_LANGUAGE_SELECTED = 0x5020,
  EMV_RESULT_CODE_SERVICE_NOT_ALLOWED = 0x5021,
  EMV_RESULT_CODE_NO_TAG_FOUND = 0x5022,
  EMV_RESULT_CODE_CARD_BLOCKED = 0x5023,
  EMV_RESULT_CODE_LEN_INCORRECT = 0x5024,
  EMV_RESULT_CODE_CARD_COM_ERROR = 0x5025,
  EMV_RESULT_CODE_TSC_NOT_INCREASED = 0x5026,
  EMV_RESULT_CODE_HASH_INCORRECT = 0x5027,
  EMV_RESULT_CODE_ARC_NOT_PRESENCED = 0x5028,
  EMV_RESULT_CODE_ARC_INVALID = 0x5029,
  EMV_RESULT_CODE_COMM_NO_ONLINE = 0x5030,
  EMV_RESULT_CODE_TRAN_TYPE_INCORRECT = 0x5031,
  EMV_RESULT_CODE_APP_NO_SUPPORT = 0x5032,
  EMV_RESULT_CODE_APP_NOT_SELECT = 0x5033,
  EMV_RESULT_CODE_LANG_NOT_SELECT = 0x5034,
  EMV_RESULT_CODE_TERM_DATA_NOT_PRESENCED = 0x5035,
  EMV_RESULT_CODE_CVM_TYPE_UNKNOWN = 0x6001,
  EMV_RESULT_CODE_CVM_AIP_NOT_SUPPORTED = 0x6002,
  EMV_RESULT_CODE_CVM_TAG_8E_MISSING = 0x6003,
  EMV_RESULT_CODE_CVM_TAG_8E_FORMAT_ERROR = 0x6004,
  EMV_RESULT_CODE_CVM_CODE_IS_NOT_SUPPORTED = 0x6005,
  EMV_RESULT_CODE_CVM_COND_CODE_IS_NOT_SUPPORTED = 0x6006,
  EMV_RESULT_CODE_CVM_NO_MORE = 0x6007,
  EMV_RESULT_CODE_PIN_BYPASSED_BEFORE = 0x6008,
  EMV_RESULT_CODE_UNKONWN = 0xffff

};

// enum LOG_LEVEL {
//     RAW_COMMUNICATION_DATA = 0x01,
//     LOG_LEVEL_FUNCTION_ENTRY_AND_EXIT = 0x02,
//      INFORMATION = 0x04
// };

typedef enum LOG_LEVEL
{
  LOG_LEVEL_FUNCTION_ENTRY_AND_EXIT = 0x01,
  LOG_LEVEL_INFORMATION = 0x02,
  LOG_LEVEL_ERROR = 0x04,
  LOG_LEVEL_WARNING = 0x08,
  LOG_LEVEL_DEBUG = 0x10
} LOG_LEVEL;

enum COMM_TYPE
{
  SEND = 0,
  READ = 1
};

enum FORMAT_TYPE
{
  PRINTF_FORMAT = 0,
  BYTE_ARRAY_FORMAT = 1
};

typedef enum RESOURCE_TYPE
{
  RESOURCE_FILE = 1, // Start at one since NULL is mapped to 0 because of a macro
  RESOURCE_USB,
  RESOURCE_RS232,
  RESOURCE_VIRTUAL
} RESOURCE_TYPE;

/**
 * Class for LCD Message
 */
typedef struct __EMV_Callback
{
  /**
   * Swipe Timeout

   Timeout value waiting for MSR Swipe
  */
  int msr_swipeTimeout;

  /**
   * MSR Message

   Message to display during swipe request
  */
  int msr_displayMessage;

  /**
   * PIN Mode.<br/>
   0- EMV_PIN_MODE_CANCEL: Entry cancel through command. No response required
   1- EMV_PIN_MODE_ONLINE_DUKPT:  PIN_DUKPT_KEY required as response
   2- EMV_PIN_MODE_ONLINE_MKSK:  PIN_SESSION_KEY required as response
   3- EMV_PIN_MODE_OFFLINE: PIN_PAIRING_DUKPT_KEY required as response,
      unless devices does not implement pairing function, then plaintext
      PIN required as response
  */
  int pin_pinMode;
  /**
   * PIN Entry Start Timeout

   Timeout value waiting for PIN entry to start
  */
  int pin_entryStartTimeout;

  /**
   * PIN Entry Interval
   Timeout value of interval between input each PIN
  */
  int pin_entryInterval;

  /**
   * PIN KSN

   Pairing DUKPT KSN
  */
  unsigned char pin_KSN[50];

  /**
   * PIN KSN Len

	Length of Pairing DUKPT KSN
  */
  int pin_KSNLen;

        /**
   * Truncated PAN

   Truncated PAN
  */
  unsigned char pin_truncatedPAN[_DATA_BUF_LEN];


  /**
   * Callback Type.<br/>
   1- EMV_CALLBACK_TYPE_LCD: LCD Display Hardware Event
   2- EMV_CALLBACK_TYPE_PINPAD:  Pinpad Hardware Event
   3- EMV_CALLBACK_MSR:  MSR Hardware Event
  */
  int callbackType;
  /**
   * Display Mode.<br/>
   1- LCD_DISPLAY_MODE_MENU: Menu selection, response required with selected
      menu index #, or 0 to cancel
   2- LCD_DISPLAY_MODE_PROMPT:  Message Prompt, response required 'E' for
      Enter/Accept, or 'C' for cancel
   3- LCD_DISPLAY_MODE_MESSAGE:  Display Message, no response required
   8- LCD_DISPLAY_MODE_LANGUAGE_SELECT: Language selection, response required
      with selected language index #
   16 - LCD_DISPLAY_MODE_CLEAR_SCREEN: Request to clear LCD screen
        of information
  */
  int lcd_displayMode;

  /**
     * Message Mode for VP8800.<br/>
     1- EMV_LCD_DISPLAY_MODE_MESSAGE: Mode 2 and Mode 3 that LCD messages will be sent to SDK
     2- EMV_LCD_DISPLAY_MODE_PAUSE:  Mode 4 that LCD messages will be sent to SDK and wait for SDK to respond
    */
  int lcd_messageMode;

  /**
   * Keypad Entry Timeout

   If Normal Display or Menu Display, Total timeout for keypad entry, in
   second default is 30 seconds.
  */
  int lcd_entryTimeout;


  /**
   * Keypad Entry Timeout Minor

   If Normal Display or Menu Display, minor timeout during each keypad entry,
   in second, little endian, default is 10 seconds.
   Note: Minor timeout will erase all previous keypad entry.

  */
  int lcd_entryTimeoutMinor;

  /**
   * Message Language

   2 Bytes
   - EN - English (default)
   - ES - Spanish
   - ZH - Chinese
   - FR - French

  */
  char language[5];



  /**
   * Display Message

   repeatable combination of [Line][Message][0x1C]
   [Line] - Display line number (1-First Line, n-nth Line), Maximum 16 lines.
   -The lower 7 bits is for line number.
   -The MSB is to indicate following message is a Message String or Message ID.
     -MSB - 0: Message String. (It is valid for "Menu Display"ï¿½ and
        "Language Menu Display")
     -MSB - 1: Message ID. (It is only valid for "Menu Display")
             [Message] - Message String or Message ID.
   Message String:
     "Menu Display"ï¿½ : character in the range of 0x20 - 0x7f,
                       Maximum 16 characters
     - "Language Menu Display"ï¿½ : 2 bytes Language ID
       EN - English (default)
       ES - Spanish
       ZH - Chinese
       FR - French

  */
  //        unsigned char lcd_messages[1024];
  BYTE lcd_messages[_DATA_BUF_LEN*4];

  /**
   * Backlight Timeout

   If Normal Display or Menu Display, Total timeout for keypad entry,
   in second default is 30 seconds.  0x0000 = backlight off, 0xFFFF = backlight
   on
  */
  short lcd_backlightTimeout;


  /**
   * Mask Entry

   If True, keypad entry should be masked with '*'
  */
  int maskEntry;

} EMV_Callback;

enum EVENT_TRANSACTION_DATA_Types
{
  EVENT_TRANSACTION_DATA_UNKNOWN,
  EVENT_TRANSACTION_DATA_CARD_DATA,
  EVENT_TRANSACTION_DATA_EMV_DATA,
  EVENT_TRANSACTION_DATA_MSR_CANCEL_KEY,
  EVENT_TRANSACTION_DATA_MSR_BACKSPACE_KEY,
  EVENT_TRANSACTION_DATA_MSR_ENTER_KEY,
  EVENT_TRANSACTION_DATA_MSR_DATA_ERROR,
  EVENT_TRANSACTION_PIN_DATA
};

/**
 * Define notification type.
 */
enum EVENT_NOTIFICATION_Types
{
  EVENT_NOTIFICATION_UNKNOWN,
  EVENT_NOTIFICATION_ICC_Card_Not_Seated,
  EVENT_NOTIFICATION_ICC_Card_Seated,
  EVENT_NOTIFICATION_MSR_Swipe_Card
};

enum CTLS_APPLICATION
{
  CTLS_APPLICATION_NONE = 0,
  CTLS_APPLICATION_MASTERCARD = 1,
  CTLS_APPLICATION_VISA = 2,
  CTLS_APPLICATION_AMEX = 3,
  CTLS_APPLICATION_DISCOVER = 4,
  CTLS_APPLICATION_SPEEDPASS = 5,
  CTLS_APPLICATION_GIFT_CARD = 6,
  CTLS_APPLICATION_DINERS_CLUB = 7,
  CTLS_APPLICATION_EN_ROUTE = 8,
  CTLS_APPLICATION_JCB = 9,
  CTLS_APPLICATION_VIVO_DIAGNOSTIC = 10,
  CTLS_APPLICATION_HID = 11,
  CTLS_APPLICATION_MSR_SWIPE = 12,
  CTLS_APPLICATION_RESERVED = 13,
  CTLS_APPLICATION_DES_FIRE_TRACK_DATA = 14,
  CTLS_APPLICATION_DES_FIRE_RAW_DATA = 15,
  CTLS_APPLICATION_RBS = 17,
  CTLS_APPLICATION_VIVO_COMM = 20
};

/**
 * Transaction Data structure.<br/>
 */
typedef struct __IDTTransactionData
{
  /**
   * Transaction Data type,please see EVENT_TRANSACTION_DATA_Types
   * for more information.<br/>
   */
  int Event;

  /**
   * Event Notification type,please see EVENT_NOTIFICATION_Types
   * for more information.<br/>
   */
  int Notification;

  /**
   * Get the card data raw data.<br/>
   * Containing complete unparsed transaction data as received from device.
   */
  unsigned char msr_rawData[_DATA_BUF_LEN*4];
  /**
   * Get the length of msr_rawData.
   */
  int msr_rawDataLen;

  /**
   * Get the swiped card Track1 encrypted data.<br/>
   * A byte array containing Track1 encrypted data.
   */
  unsigned char msr_encTrack1[_DATA_BUF_LEN*4];
  /**
   * Get the length of msr_encTrack1.
   */
  int msr_encTrack1Len;

  /**
   * Get the swiped card Track2 encrypted data.<br/>
   * A byte array containing Track2 encrypted data.
   */
  unsigned char msr_encTrack2[_DATA_BUF_LEN*4];
  /**
   * Get the length of msr_encTrack2.
   */
  int msr_encTrack2Len;

  /**
   * Get the swiped card Track3 encrypted data.<br/>
   * A byte array containing Track3 encrypted data.
   */
  unsigned char msr_encTrack3[_DATA_BUF_LEN*4];
  /**
   * Get the length of msr_encTrack3Len.
   */
  int msr_encTrack3Len;

  /**
   * Get the swiped card Track1 data.<br/>
   * A string containing Track1 masked data expressed as hex characters.
   */
  unsigned char msr_track1[_DATA_BUF_LEN*4];

  /**
   * Get the swiped card Track2 data.<br/>
   * A string containing Track2 masked data expressed as hex characters.
   */
  unsigned char msr_track2[_DATA_BUF_LEN*4];

  /**
   * Get the swiped card Track3 data.<br/>
   * A string containing Track3 masked data expressed as hex characters.
   */
  unsigned char msr_track3[_DATA_BUF_LEN*4];

  /**
   * Get the Reader Serial Number.<br/>
   */
  unsigned char* device_RSN;
  /**
   * Get the length of device_RSNLen.
   */
  int device_RSNLen;
  /**
   * Get the swiped card KSN (Key Serial Number).<br/>
   * A byte array containing 10 bytes.
   */
  unsigned char msr_KSN[10];
  /**
   * Get the length of msr_KSN.
   */
  int msr_KSNLen;

  /**
   * Get the swiped card length of Track1 data.
   */
  int msr_track1Length;

  /**
   * Get the swiped card length of Track2 data.
   */
  int msr_track2Length;

  /**
   * Get the swiped card length of Track3 data.
   */
  int msr_track3Length;


  /**
   * Get the swiped card type,please see CAPTURE_ENCODE_TYPE for
   * more information.<br/>
   * MSR card type:<br/>
   *                   CAPTURE_ENCODE_TYPE_ISOABA:ISO/ABA format<br/>
   *                   CAPTURE_ENCODE_TYPE_AAMVA:AAMVA format<br/>
   *                   CAPTURE_ENCODE_TYPE_Other:Other<br/>
   *                   CAPTURE_ENCODE_TYPE_Raw:Raw; undecoded format<br/>
   *                   CAPTURE_ENCODE_TYPE_JisI_II:JIS I or JIS II
   *
   */
  int msr_cardType;


  /**
   * Get the swiped card decoded status.<br/>
   *          0x00:decoded data success;<br/>
   *                    Bit0:1-track1 data error;<br/>
   *                    Bit1:1-track2 data error;<br/>
   *                    Bit2:1-track3 data error;<br/>
   *                    Bit3:1-track1 encrypted data error;<br/>
   *                    Bit4:1-track2 encrypted data error;<br/>
   *                    Bit5:1-track3 encrypted data error;<br/>
   *                    Bit6:1-KSN error;<br/>
   *
   */
  byte msr_captureEncodeStatus;


  /**
   * Get the swiped card encrypted type,please see CAPTURE_ENCRYPT_TYPE
   * for more information.<br/>
   *          CAPTURE_ENCRYPT_TYPE_TDES:TDES;<br/>
   *                    CAPTURE_ENCRYPT_TYPE_AES:AES;
   *
   */
  int msr_captureEncryptType;

  /**
   * Contains error code when data is not returned
   */
  int msr_errorCode;
  /**
   * For some Error Codes, the RF State Code indicates the exact Reader-Card
     command that failed. This helps determine the exact place where
     the failure occurred.
   */
  int emv_rfStateCode;
  /**
   * Get the swiped card ICC Status.<br/>
   * A flag to determine if ICC is present on card.
   */
  int iccPresent;
  /**
   * Get the swiped card Session ID.<br/>
   * A byte array to get session ID, if exists.
   */
  unsigned char msr_sessionID[_DATA_BUF_LEN];
  /**
   * Get the length of msr_sessionID.
   */
  int msr_sessionIDLen;
  /**
   * TRUE = Data Variant key used<br/>
   * FALSE = PIN Variant key used
   */
  int msr_isDataVariant;
  /**
   * Get the swiped card Track1 hash data.<br/>
   * A byte array containing Track1 hash data.
   */
  unsigned char msr_hashTrack1[_DATA_BUF_LEN*4];
  /**
   * Get the length of msr_hashTrack1.
   */
  int msr_hashTrack1Len;
  /**
   * Get the swiped card Track2 hash data.<br/>
   * A byte array containing Track2 hash data.
   */
  unsigned char msr_hashTrack2[_DATA_BUF_LEN*4];
  /**
   * Get the length of msr_hashTrack2Len.
   */
  int msr_hashTrack2Len;
  /**
   * Get the swiped card Track3 hash data.<br/>
   * A byte array containing Track3 hash data.
   */
  unsigned char msr_hashTrack3[_DATA_BUF_LEN*4];
  /**
   * Get the length of msr_hashTrack3.
   */
  int msr_hashTrack3Len;
  /**
   * Track data was captured via CTLS interface
   */
  int isCTLS;
  /**
   * CTLS Application
   */
  int ctlsApplication;
  /**
   *  clearing record TLV
   */
  unsigned char emv_clearingRecord[_DATA_BUF_LEN*4];
  /**
   * Get the length of emv_clearingRecord.
   */
  int emv_clearingRecordLen;
  /**
   *  Encrypted Tags TLV
   */
  unsigned char emv_encryptedTags[_DATA_BUF_LEN*4];
  /**
   * Get the length of emv_encryptedTags.
   */
  int emv_encryptedTagsLen;
  /**
   *  Unencrypted Tags TLV
   */
  unsigned char emv_unencryptedTags[_DATA_BUF_LEN*4];
  /**
   * Get the length of emv_unencryptedTags.
   */
  int emv_unencryptedTagsLen;
  /**
   *  EMV Result Code
   */
  int emv_resultCode;
  /**
   *  EMV Encryption Mode
   */
  int emv_encryptionMode;
  /**
   *  Masked Tags TLV
   */
  unsigned char emv_maskedTags[_DATA_BUF_LEN*4];
  /**
   * Get the length of emv_maskedTags.
   */
  int emv_maskedTagsLen;
  /**
   *  Advise
   */
  int emv_hasAdvise;
  /**
   *  Reversal
   */
  int emv_hasReversal;
  /**
   *  PIN block from PINPAD
   */
  unsigned char pin_pinblock[_DATA_BUF_LEN];
  /**
   * Get the length of pin_pinblock.
   */
  int pin_pinblockLen;
  /**
   *  KSN for Pinblock
   */
  unsigned char pin_KSN[10];
  /**
   * Get the length of pin_KSN.
   */
  int pin_KSNLen;
  /**
   *  KSN for Pinblock
   */
  unsigned char pin_KeyEntry[_DATA_BUF_LEN];
  /**
   * Get the length of pin_KeyEntry.
   */
  int pin_KeyEntryLen;

  /**
   * SW1
   */
  unsigned char SW1;
  /**
   * SW2
   */
  unsigned char SW2;
  /**
   *  Message Authentication Code
   */
  unsigned char mac[_DATA_BUF_LEN];
  /**
   * Get the length of mac.
   */
  int macLen;
  /**
   *  Message Authentication Code Key Serial Number
   */
  unsigned char macKSN[10];
  /**
   * Get the length of macKSN.
   */
  int macKSNLen;

  /**
    * Fast EMV string.
    */
  char fastEMV[_DATA_BUF_LEN*4];

}IDTTransactionData;

    /**
 * IDTTag structure definition
 */
typedef struct _IDTTag
{
  int tagLen;
  /**
   * The tag name
   */
  BYTE Tag[10];
  /**
   * The length of the value
   */
  int valueLen;
  /**
   * The tag value
   */
  BYTE value[_DATA_BUF_LEN*4];

} IDTTag;

/**
  * IDTMSRData structure definition
  */
typedef struct _IDTMSRData
{
  /**
   * MSR type,please see EVENT_MSR_Types for more information.<br/>
   */
  BYTE cardDataFlag;

  /**
   * Get the swiped card data.<br/>
   * Containing complete unparsed swipe data as received from MSR.<br/>
   * NOTE: <br/>
   * Just refer to this item cardData if the card data is the clear data.<br/>
   *
   */
  BYTE cardData[_DATA_BUF_LEN*4];
  /**
   * Get the length of cardData
   */
  int cardDataLen;

  /**
   * CTLS Application
   */
  int ctlsApplication;

  /**
   * Track data was captured via CTLS interface
   */
  int isCTLS;

  /**
   * Card Track 1 decode status
   */
  BYTE t1DecodeStatus;
  /**
   * Card Track 2 decode status
   */
  BYTE t2DecodeStatus;
  /**
   * Card Track 3 decode status
   */
  BYTE t3DecodeStatus;

  /**
   * Get the swiped card Track1 encrypted data.<br/>
   * A byte array containing Track1 encrypted data.
   */
  BYTE encTrack1[_DATA_BUF_LEN*4];
  /**
   * Get the length of encTrack1
   */
  int encTrack1Len;

  /**
   * Get the swiped card Track2 encrypted data.<br/>
   * A byte array containing Track2 encrypted data.
   */
  BYTE encTrack2[_DATA_BUF_LEN*4];
  /**
   * Get the length of encTrack2
   */
  int encTrack2Len;

  /**
   * Get the swiped card Track3 encrypted data.<br/>
   * A byte array containing Track3 encrypted data.
   */
  BYTE encTrack3[_DATA_BUF_LEN*4];
  /**
   * Get the length of encTrack3
   */
  int encTrack3Len;

  /**
   * Get the swiped card Track1 data.<br/>
   * A string containing Track1 masked data expressed as hex characters.
   */
  char track1[_DATA_BUF_LEN*4];

  /**
   * Get the swiped card Track2 data.<br/>
   * A string containing Track2 masked data expressed as hex characters.
   */
  char track2[_DATA_BUF_LEN*4];

  /**
   * Get the swiped card Track3 data.<br/>
   * A string containing Track3 masked data expressed as hex characters.
   */
  char track3[_DATA_BUF_LEN*4];

  /**
   * Get the Reader Serial Number.<br/>
   */
  char serialNumber[10];
  /**
   * Get the length of serialNumber
   */
  int serialNumberLen;
  /**
   * Get the swiped card KSN (Key Serial Number).<br/>
   * A byte array containing up to 11 bytes.
   */
  BYTE KSN[11];
  /**
   * Get the length of KSN
   */
  int KSNLen;

  /**
   * Get the swiped card length of Track1 data.
   */
  int track1Length;

  /**
   * Get the swiped card length of Track2 data.
   */
  int track2Length;

  /**
   * Get the swiped card length of Track3 data.
   */
  int track3Length;

  /**
   * Determines if ICC is present in card (service code starts with "2" or "6").
   */
  int iccPresent;
  /**
   * Get the swiped card type,please see CAPTURE_ENCODE_TYPE for more
   * information.<br/>
   * MSR card type:<br/>
   *      CAPTURE_ENCODE_TYPE_ISOABA:ISO/ABA format<br/>
   *      CAPTURE_ENCODE_TYPE_AAMVA:AAMVA format<br/>
   *      CAPTURE_ENCODE_TYPE_Other:Other<br/>
   *      CAPTURE_ENCODE_TYPE_Raw:Raw; undecoded format<br/>
   *      CAPTURE_ENCODE_TYPE_JisI_II:JIS I or JIS II
   *
   */
  BYTE cardType;


  /**
   * Get the swiped card decoded status.<br/>
   *          0x00:decoded data success;<br/>
   *              Bit0:1-track1 data error;<br/>
   *              Bit1:1-track2 data error;<br/>
   *              Bit2:1-track3 data error;<br/>
   *              Bit3:1-track1 encrypted data error;<br/>
   *              Bit4:1-track2 encrypted data error;<br/>
   *              Bit5:1-track3 encrypted data error;<br/>
   *              Bit6:1-KSN error;<br/>
   *
   */
  BYTE captureEncodeStatus;


  /**
   * Get the swiped card encrypted type,please see CAPTURE_ENCRYPT_TYPE
   * for more information.<br/>
   *          CAPTURE_ENCRYPT_TYPE_TDES:TDES;<br/>
   *                    CAPTURE_ENCRYPT_TYPE_AES:AES;
   *
   */
  BYTE captureEncryptType;

  /**
   * The flag to indicate the availability of the swiped card
   * Clearing Record data.
   */
  BYTE hasDE055;

  /**
   * Get the swiped card length of Clearing Record data.
   */
  int DE055Len;

  /**
   * Get the swiped card of DE055 data.
   */
  BYTE DE055Data[_DATA_BUF_LEN*2];

  /**
   * Get the swiped card length of TLV data.
   */
  int TLVLen;

  /**
   * Get the swiped card TLV data.
   */
  BYTE TLVData[_DATA_BUF_LEN*4];

  /**
   * Get the length of optional bytes.
   */
  int optionalBytesLen;

  /**
   * Get optional bytes of the swiped card data.
   */
  BYTE optionalBytes[_DATA_BUF_LEN*4];
  /**
   * TRUE = Data Variant key used<br/>
   * FALSE = PIN Variant key used
   */
  int isDataVariant;
  /**
   * Get the length of sessionID.
   */
  int sessionIDLen;

  /**
   * Get optional bytes of the session ID.
   */
  BYTE sessionID[_DATA_BUF_LEN*4];
  /**
   * Get the length of track 1 hash.
   */
  int track1HashLen;

  /**
   * Get optional bytes of the track 1 hash.
   */
  BYTE track1Hash[_DATA_BUF_LEN*4];
  /**
   * Get the length of track 2 hash.
   */
  int track2HashLen;

  /**
   * Get optional bytes of the track 2 hash.
   */
  BYTE track2Hash[_DATA_BUF_LEN*4];
  /**
   * Get the length of track 3 hash.
   */
  int track3HashLen;

  /**
   * Get optional bytes of the track 3 hash.
   */
  BYTE track3Hash[_DATA_BUF_LEN*4];

  /**
   * Get unencrypted tags.
   */
  IDTTag unencryptedTagArray[_MAX_TAG_COUNT];
  /**
   * Get the count of unencrypted tags.
   */
  int unencryptedTagCount;
  /**
   * Get masked tags.
   */
  IDTTag maskedTagArray[_MAX_TAG_COUNT];
  /**
   * Get the count of masked tags.
   */
  int maskedTagCount;
  /**
   * Get encrypted tags.
   */
  IDTTag encryptedTagArray[_MAX_TAG_COUNT];
  /**
   * Get the count of encrypted tags.
   */
  int encryptedTagCount;
  /**
   * Get the length of raw track data.
   */
  int rawTrackDataLen;
  /**
   * Get the raw track data.
   */
  BYTE rawTrackData[_DATA_BUF_LEN*4];

  /**
   *  Message Authentication Code
   */
  unsigned char mac[_DATA_BUF_LEN];
  /**
   * Get the length of mac.
   */
  int macLen;
  /**
   *  Message Authentication Code Key Serial Number
   */
  unsigned char macKSN[10];
  /**
   * Get the length of macKSN.
   */
  int macKSNLen;

  /**
   * Error code from the tag DFEE25 if available
   */
  int errorCode;

  /**
	* Fast EMV string.
	*/
  char fastEMV[_DATA_BUF_LEN*4];

} IDTMSRData;

/**
 * IDTPINData structure definition
 */
typedef struct _IDTPINData
{
  /**
   * Get the swiped card KSN (Key Serial Number).<br/>
   * A byte array containing 10 bytes.
   */
  BYTE KSN[12];

  /**
   * Get the length of KSN.
   */
  int KSNLen;

  /**
   * Get the length of PIN block data.
   */
  int pinBlockLen;

  /**
   * Get the PIN block data.
   */
  BYTE pinBlockData[_DATA_BUF_LEN];

  /**
   * This initial vector is used for all encryptions in this command. If
   * encryption is off this field will be filled with zeros (00h).
   *    Format: binary, most significant byte first
   */
  BYTE InitVector[16];
  /**
   * Length of the initial vector. When encryption is off, this value will be 0.
   */
  int InitVectorLen;
  /**
   * Complete PAN, either plain or encrypted.
   *  Format: ASCII (no null terminator), if plain text
   *                    Binary (most significant byte first), if encrypted.
   *    Supports AES in the new format.
   */
  BYTE PAN[64];
  /**
   * Length of the complete PAN, whether encrypted or not. If encrypted this
     will always be a multiple of 8 bytes
   */
  int PANLen;
  /**
   * First digits of unencrypted PAN, in plain text.
   *    Format: ASCII (no null terminator)
   */
  BYTE FirstPANDigits[16];
  /**
   * Number of first PAN digits in plain text.
   */
  int FirstPANDigitsLen;
  /**
   * Last digits of unencrypted PAN, in plain text.
   *    Format: ASCII (no null terminator)
   */
  BYTE LastPANDigits[16];
  /**
   * Number of last PAN digits in plain text.
   */
  int LastPANDigitsLen;
  /**
   * Complete Expiry Date string
   *    Format: ASCII (no null terminator), if plain text
   *                Binary (most significant byte first), if encrypted
   */
  BYTE ExpiryDate[16];
  /**
   * Length of Expiry Date string. Encrypted date will always be a
     multiple of 8 bytes.
   */
  int ExpiryDateLen;
  /**
   * Complete CSC
   *    Format: ASCII (no null terminator), if plain text
   *                    Binary (most significant byte first), if encrypted
   */
  BYTE CSC[16];
  /**
   * Length of CSC (if requested). Encrypted CSC will always be a multiple
     of 8 bytes.
   */
  int CSCLen;
  /**
   * MAC Status Byte
   *    Bit 0:  If 1SHA-256. If 0SHA-1 (Note: If no optional status
                byte, use default SHA-1)
   *    Bit 1: If 1Encryption type TDES.
   *               If 0Encryption type AES
   *    Bit 4, 3, 2: 000RFU
   *    Bit 5:   If 1 MAC Value Length, MAC Value and MAC Key KSN
   *                     If 0 No MAC Value Length, MAC Value and MAC Key KSN.
   *    Bit 6:  RFU
   *    Bit 7:  RFU
   */
  BYTE MACStatus;
  /**
   * Authenticate message from Initial Vector field to MAC Value
   * Length field
   *    Will not be available when encryption is off.
   */
  BYTE MACValue[_DATA_BUF_LEN];
  /**
   * Will not be available when encryption is off.
   */
  int MACValueLen;
  /**
   * Complete CSC
   *    Format: ASCII (no null terminator), if plain text
   *                    Binary (most significant byte first), if encrypted
   */
  BYTE MACKSN[10];
  /**
   * Will not be available when encryption is off.
   */
  int MACKSNLen;

  /**
   * PIN data from the keypad for the function pin_getEncryptedPIN()
   * If Get Encrypted PIN with DUKPT Key under Triple DES: 20 ASCII code
   *    KSN + 16 ASCII code Encrypted PIN block
   * If Get Encrypted PIN with MKSK: 16 ASCII code Encrypted PIN block
   * A byte array containing 10 bytes.
   */
  BYTE PIN[36];

  /**
   * Get the length of PIN data.
   */
  int PINLen;
  /**
   * The error code if available when PIN_callBack_type is
   * PIN_callBack_type_ERROR_CODE.
   */
  int errorCode;
  /**
   * The ASCII code of the function key if available when PIN_callBack_type
   * is PIN_callBack_type_KEY_INPUT.
   */
  char functionKeyCode[2];
  /**
	* The 4-byte binary code of the function key if available when PIN_callBack_type
	* is PIN_callBack_type_KEY_INPUT.
	*/
  BYTE functionKey[4];
  /**
   * The ASCII key input string from pin_promptForKeyInput() or
   * pin_promptForAmountInput().
   */
  char keyString[_DATA_BUF_LEN];
  /**
   * The length of keyString.
   */
  int keyStringLen;
} IDTPINData;

/**
 * IDTScreenInfo structure definition
 */
typedef struct _IDTScreenInfo
{
  /**
   * Screen name.
   */
  char screenName[100];

  /**
   * Screen name length.
   */
  int screenNameLen;

  /**
   * Screen ID.
   */
  int screenID;
}IDTScreenInfo;

/**
 * IDTObjectInfo structure definition
 */
typedef struct _IDTObjectInfo
{
  /**
   * Object name.
   */
  char objectName[100];

  /**
   * Object name length.
   */
  int objectNameLen;

  /**
   * Object ID.
   */
  int objectID;
}IDTObjectInfo;

/**
 * IDTLCDItem structure definition
 */
typedef struct _IDTLCDItem
{
  /**
   * Screen name.
   */
  char screenName[100];

  /**
   * Screen name length.
   */
  int screenNameLen;

  /**
   * Screen ID.
   */
  int screenID;

  /**
   * Object name.
   */
  char objectName[100];

  /**
   * Object name length.
   */
  int objectNameLen;

  /**
   * Object ID.
   */
  int objectID;

  /**
   * Top-Left x
   */
  int topLeftX;

  /**
   * Top-Left y
   */
  int topLeftY;

  /**
   * Bottom-Right x
   */
  int bottomRightX;

  /**
   * Bottom-Right y
   */
  int bottomRightY;

  /**
    * The flag indicates if the button is long pressed.
    */
  int longPressed;
} IDTLCDItem;

enum CIPHER_MODE
{
  CBC,    // Cipher Block Chaining
  CFB,    // Cipher Feedback
  ECB,    // Electronic Codebook
  OFB     // Output Feedback
};

/*
 * Stucture for AES & TDES options
 */
typedef struct _IDTCipherSettings
{
  /**
   * contains the initial vector
   * default value is all 0x00 when IDTCipherSettings is NULL
   */
  BYTE iv[_DATA_BUF_LEN];

  /**
   * length of the initial vector
   */
  int ivLen;

  /**
   * cipher mode to use (CBC, CFB, ECB, OFB)
   * default value is CBC when IDTCipherSettings is NULL
   */
  enum CIPHER_MODE mode;

  /**
   * 1 for DES
   * 0 for Triple DES
   * default value is 0 when IDTCipherSettings is NULL
   * Does not apply when used with AES functions
   */
  int des;

  /**
   * 1 for PKCS#7
   * 0 for no padding (input length has to be a multiple of block size)
   * default value is 1 when IDTCipherSettings is NULL
   */
  int padding;
} IDTCipherSettings;

/**
 * Stucture for IDT Crypto Data
 */
typedef struct _IDTCryptoData
{
  /**
   * (required) - contains either BDK (16 or 24 bytes) or IPEK (16 bytes)
   */
  BYTE key[25];

  /**
   * (required) - either BDK or IPEK Length
   */
  int keyLen;

  /**
   * (required) - contains ksn (10 bytes)
   */
  BYTE ksn[11];

  /**
   * (required) - 1 if key varible is IPEK, 0 for BDK
   */
  int isIPEK;

  /**
   * (required) - 1 for decryption, 0 for encryption
   */
  int isDecryption;

  /**
   * (required) - 1 for TDES, 0 for AES
   */
  int isTDES;

  /**
   * (required) - 1 for processing PIN numbers, 0 for other data
   */
  int isPin;

  /**
   * (required when not processing PIN numbers) - 0 for Data, 1 for PIN,
   * and 2 for MAC
   */
  int keyVariant;

  /**
   * (required when using TDES)
   * type 0 (pads key with 0xf) or
   * type 3 (pads with random value from 0x0 to 0xf)
   */
  int pinBlockType;

  /**
   * (required expect PIN encryption) - store input data to process
   */
  BYTE dataToProcess[_DATA_BUF_LEN];

  /**
   * (required expect PIN encryption) - length of input data to process
   */
  int dataToProcessLen;

  /**
   * (required expect PIN decryption) - store output processed result
   */
  BYTE dataResults[_DATA_BUF_LEN];

  /**
   * (required expect PIN decryption) - length of output processed result
   */
  int dataResultsLen;

  /**
   * (required when processing PIN) - stores PIN number for both PIN number
   * encryption and decryption (must be between 1 - 14 numbers)
   */
  char pin[15];

  /**
   * (required when processing PIN) - for processing PIN numbers, contains
   * PAN number (must be at least 13 numbers)
   */
  char pan[_DATA_BUF_LEN];
} IDTCryptoData;

typedef struct _IDTDeviceHandle
{
  /**
   * The handle of the devive.
   */
  HANDLE handle;

  /**
   * The comport number of the RS232 device.
   */
  int comportNumber;

  /**
   * The baud rate of the RS232 device.
   */
  int baudrate;

  /**
   * Device type.
   */
  int deviceType;

  /**
   * if the device is active
   */
  int active;

  /**
   * if the device is connected
   */
  int connected;

  /**
   * name of the device
   */
  char name[30];

  /**
   * PID of the NEO2 device
   */
  int pid;
} IDTDeviceHandle;

// ISO8583 structures taken from dl_iso8583_common.h

struct DL_ISO8583_FIELD_DEF_S
{
  DL_UINT8   dataField;
  DL_UINT8   fieldType;
  DL_UINT16  len;     /* length for fixed size / max-len for variables */
  DL_UINT8   varLen;  /* number of variable length digits - e.g. 0-4   */
  char fieldUsage[MAXIMUM_FIELD_USAGE_LENGTH];
};

typedef struct DL_ISO8583_FIELD_DEF_S DL_ISO8583_FIELD_DEF;

struct DL_ISO8583_HANDLER_S
{
  DL_ISO8583_FIELD_DEF *fieldArr;
  DL_UINT8              fieldItems;
  char                  version[16]; // A year depicting the ISO8583 version
};
typedef struct DL_ISO8583_HANDLER_S DL_ISO8583_HANDLER;

struct DL_ISO8583_MSG_FIELD_S
{
  DL_UINT16  len; /* used mainly for binary fields               */
  DL_UINT8  *ptr; /* null terminated data bytes (even if binary) */
};
typedef struct DL_ISO8583_MSG_FIELD_S DL_ISO8583_MSG_FIELD;

struct DL_ISO8583_MSG_S
{
  /* static memory details */
  DL_UINT8 *sPtrNext; /* next static point - NULL if dynamic mode  */
  DL_UINT8 *sPtrEnd;  /* end of the static buffer (if static mode) */

  /* NB bitmap is not stored, it is implied if the field is set */

  /* fields */
  DL_ISO8583_MSG_FIELD field[1+kDL_ISO8583_MAX_FIELD_IDX];
};
typedef struct DL_ISO8583_MSG_S DL_ISO8583_MSG;

/**
 * RequestCSR structure definition
 */
typedef struct _RequestCSR
{
  char encKeysName[200];
  int encKeysNameLen;
  char encKeysCSR[200];
  int encKeysCSRLen;
  char encSigningName[200];
  int encSigningNameLen;
  char encSigningCSR[200];
  int encSigningCSRLen;
  char encAppsName[200];
  int encAppsNameLen;
  char encAppsCSR[200];
  int encAppsCSRLen;
} RequestCSR;

enum DeviceState
{
  TransactionData,
  Connected,
  ConnectionFailed,
  DataReceived,
  DataSent,
  Disconnected,
  SwipeCanceled,
  ToConnect,
  ToSwipe,
  ToTap,
  CommandTimeout,
  SwipeTimeout,
  DeviceTimeout,
  EMVCallback,
  TransactionCancelled,
  TransactionFailed,
  MSRDecodeError,
  DefaultDeviceTypeChange,
  Notification,
  PINpadKeypress,
  PINCancelled,
  PINTimeout
};

/*
enum RETURN_CODE
{
    RETURN_CODE_DO_SUCCESS = 1,             // no error, beginning task
    RETURN_CODE_ERR_DISCONNECT,         // no response from reader
    RETURN_CODE_ERR_RESPONSE_FORMAT,       //!< invalid response data
    RETURN_CODE_ERR_TIMEDOUT,           // time out for task or CMD
    RETURN_CODE_ERR_INVALID_PARAMETER,  // wrong parameter
    RETURN_CODE_ERR_BUF_NOT_ENOUGH,   //the back buffer length is not enough
    RETURN_CODE_SDK_BUSY_EMV,        // SDK is doing EMV task
    RETURN_CODE_ERR_OTHER,              // SDK is doing Other task
    RETURN_CODE_FAILED,                 //  or err data
    RETURN_CODE_CONNECTED,           //!< did connection
    RETURN_CODE_CANCELED,           //!< task or CMD be canceled
};
*/
enum RETURN_CODE
{
  RETURN_CODE_DO_SUCCESS = 0,         //!< no error, beginning task
  RETURN_CODE_ERR_DISCONNECT,         //!< no response from reader
  RETURN_CODE_ERR_CMD_RESPONSE,       //!< invalid response data
  RETURN_CODE_ERR_TIMEDOUT,           //!< time out for task or CMD
  RETURN_CODE_ERR_INVALID_PARAMETER,  //!< wrong parameter
  RETURN_CODE_SDK_BUSY_MSR,           //!< SDK is doing MSR or ICC task
  RETURN_CODE_SDK_BUSY_PINPAD,        //!< SDK is doing PINPad task
  RETURN_CODE_SDK_BUSY_CTLS,          //!< SDK is doing CTLS task
  RETURN_CODE_SDK_BUSY_EMV,           //!< SDK is doing EMV task
  RETURN_CODE_ERR_OTHER,              //!< SDK is doing Other task
  RETURN_CODE_FAILED,                 //!< err response or data
  RETURN_CODE_NOT_ATTACHED,           //!< no reader attached
  RETURN_CODE_MONO_AUDIO,             //!< mono audio is enabled
  RETURN_CODE_CONNECTED,              //!< did connection
  RETURN_CODE_LOW_VOLUME,             //!< audio volume is too low
  RETURN_CODE_CANCELED,               //!< task or CMD be canceled
  RETURN_CODE_INVALID_STR,            //!< UF wrong string format
  RETURN_CODE_NO_FILE,                //!< UF file not found
  RETURN_CODE_INVALID_FILE,           //!< UF wrong file format
  RETURN_CODE_HOST_UNREACHABLE,       //!< Attempt to contact online host failed
  RETURN_CODE_RKI_FAILURE,            //!< Attempt to perform RKI failed
  RETURN_CODE_MISSING_DLL,            //!< DLL is missing for the target device
  RETURN_CODE_ERR_BUF_NOT_ENOUGH,     //!< not enough memory for the buffer
  RETURN_CODE_ERR_CMD_NOT_SUPPORTED,  //!< the command is not supported
  RETURN_CODE_ERR_CMD_NOT_ALLOWED,    //!< the command is not allowed
  RETURN_CODE_FW_INIT_SUCCESS,        //!< firmware initialized successfully
  RETURN_CODE_BLOCK_TRANSFER_SUCCESS = 0x2B,    //!< firmware write block successfully

  RETURN_CODE_EMV_PIN = 0x31,
  RETURN_CODE_EMV_SIGNATURE = 0x32,

  RETURN_CODE_P2_DECLINED = 0X36, //!< No Advice or Reversal Required (Declined)

  RETURN_CODE_RKI_TIMEOUT = 0x101,
  RETURN_CODE_RKI_INITIALIZATION_ERROR = 0x0102,
  RETURN_CODE_RKI_AUTHENTICATION_ERROR = 0x0103,
  RETURN_CODE_RKI_TR31_ERROR = 0x0104,

#if !UNIPAY_SHOULD_SKIP_THIS
  RETURN_CODE_P1_INCORRECT_FRAME_TAG = 0X0C01, //!< Incorrect Frame Tag
  RETURN_CODE_P1_INCORRECT_FRAME_TYPE = 0X0C02, //!< Incorrect Frame Type
  RETURN_CODE_P1_UNKNOWN_FRAME_TYPE = 0X0C03, //!< Unknown Frame Type
  RETURN_CODE_P1_UNKNOWN_COMMAND = 0X0C04, //!< Unknown Command
  RETURN_CODE_P1_UNKNOWN_SUB_COMMAND = 0X0C05, //!< Unknown Sub-Command
  RETURN_CODE_P1_CRC_ERROR = 0X0C06, //!< CRC Error
  RETURN_CODE_P1_FAILED = 0X0C07, //!< Failed
  RETURN_CODE_P1_TIMEOUT = 0X0C08, //!< Timeout
  RETURN_CODE_P1_INCORRECT_PARAMETER = 0X0C0A, //!< Incorrect Parameter
  RETURN_CODE_P1_COMMAND_NOT_SUPPORTED = 0X0C0B, //!< Command Not Supported
  RETURN_CODE_P1_SUB_COMMAND_NOT_SUPPORTED = 0X0C0C, //!< Sub-Command Not Supported
  RETURN_CODE_P1_STATUS_ABORT_COMMAND = 0X0C0D, //!< Parameter Not Supported / Status Abort Command
  RETURN_CODE_P1_COMMAND_NOT_ALLOWED = 0X0C0F, //!< Sub-Command Not Allowed
  RETURN_CODE_P2_ = 0X0D01, //!< Incorrect Header Tag
  RETURN_CODE_P2_UNKNOWN_COMMAND = 0X0D02, //!< Unknown Command
  RETURN_CODE_P2_UNKNOWN_SUB_COMMAND = 0X0D03, //!< Unknown Sub-Command
  RETURN_CODE_P2_CRC_ERROR = 0X0D04, //!< CRC Error in Frame
  RETURN_CODE_P2_INCORRECT_PARAMETER = 0X0D05, //!< Incorrect Parameter
  RETURN_CODE_P2_PARAMETER_NOT_SUPPORTED = 0X0D06, //!< Parameter Not Supported
  RETURN_CODE_P2_MAL_FORMATTED_DATA = 0X0D07, //!< Mal-formatted Data
  RETURN_CODE_P2_TIMEOUT = 0X0D08, //!< Timeout
  RETURN_CODE_P2_FAILED = 0X0D0A, //!< Failed / NACK
  RETURN_CODE_P2_COMMAND_NOT_ALLOWED = 0X0D0B, //!< Command not Allowed
  RETURN_CODE_P2_SUB_COMMAND_NOT_ALLOWED = 0X0D0C, //!< Sub-Command not Allowed
  RETURN_CODE_P2_BUFFER_OVERFLOW = 0X0D0D, //!< Buffer Overflow (Data Length too large for reader buffer)
  RETURN_CODE_P2_USER_INTERFACE_EVENT = 0X0D0E, //!< User Interface Event
  RETURN_CODE_P2_COMM_TYPE_NOT_SUPPORTED = 0X0D11, //!< Communication type not supported, VT-1, burst, etc.
  RETURN_CODE_P2_SECURE_INTERFACE_NOT_FUNCTIONAL = 0X0D12, //!< Secure interface is not functional or is in an intermediate state.
  RETURN_CODE_P2_DATA_FIELD_NOT_MOD_8 = 0X0D13, //!< Data field is not mod 8
  RETURN_CODE_P2_PADDING_UNEXPECTED = 0X0D14, //!< Pad 0x80 not found where expected
  RETURN_CODE_P2_KEY_TYPE_INVALID = 0X0D15, //!< Specified key type is invalid
  RETURN_CODE_P2_COULD_NOT_RETRIEVE_KEY = 0X0D16, //!< Could not retrieve key from the SAM(InitSecureComm)
  RETURN_CODE_P2_HASH_CODE_ERROR = 0X0D17, //!< Hash code problem
  RETURN_CODE_P2_COUND_NOT_STORE_KEY = 0X0D18, //!< Could not store the key into the SAM(InstallKey)
  RETURN_CODE_P2_FRAME_TOO_LARGE = 0X0D19, //!< Frame is too large
  RETURN_CODE_P2_RESEND_INITSECURECOMM_COMMAND = 0X0D1A, //!< Unit powered up in authentication state but POS must resend the InitSecureComm command
  RETURN_CODE_P2_EEPROM_NOT_INITIALIZED = 0X0D1B, //!< The EEPROM may not be initialized because SecCommInterface does not make sense
  RETURN_CODE_P2_APDU_ENCODING_ERROR = 0X0D1C, //!< Problem encoding APDU
  RETURN_CODE_P2_SAM_COMM_ERROR = 0X0D20, //!< Unsupported Index(ILM) SAM Transceiver error - problem communicating with the SAM(Key Mgr)
  RETURN_CODE_P2_SEQUENCE_COUNTER_ERROR = 0X0D21, //!< Unexpected Sequence Counter in multiple frames for single bitmap(ILM) Length error in data returned from the SAM(Key Mgr)
  RETURN_CODE_P2_IMPROPER_BITMAP = 0X0D22, //!< Improper bit map(ILM)
  RETURN_CODE_P2_REQUEST_ONLINE_AUTHORIZATION = 0X0D23, //!< Request Online Authorization
  RETURN_CODE_P2_RAW_DATA_READ_SUCCESSFUL = 0X0D24, //!< ViVOCard3 raw data read successful
  RETURN_CODE_P2_MESSAGE_INDEX_NOT_AVAILABLE = 0X0D25, //!< Message index not available(ILM) ViVOcomm activate transaction card type(ViVOcomm)
  RETURN_CODE_P2_VERSION_INFORMATION_MISMATCH = 0X0D26, //!< Version Information Mismatch(ILM)
  RETURN_CODE_P2_INCORRECT_MESSAGE_INDEX = 0X0D27, //!< Not sending commands in correct index message index(ILM)
  RETURN_CODE_P2_NEXT_MESSAGE_NOT_RECEIVED = 0X0D28, //!< Time out or next expected message not received(ILM)
  RETURN_CODE_P2_ILM_LANGUAGE_NOT_AVAILABLE = 0X0D29, //!< ILM languages not available for viewing(ILM)
  RETURN_CODE_P2_OTHER_LANGUAGE_NOT_SUPPORTED = 0X0D2A, //!< Other language not supported(ILM)
  RETURN_CODE_UNKNOWN_ERROR_FROM_SAM = 0X0D41, //!< Unknown Error from SAM
  RETURN_CODE_INVALID_DATA_DETECTED_BY_SAM = 0X0D42, //!< Invalid data detected by SAM
  RETURN_CODE_INCOMPLETE_DATA_DETECTED_BY_SAM = 0X0D43, //!< Incomplete data detected by SAM
  RETURN_CODE_RESERVED = 0X0D44, //!< Reserved
  RETURN_CODE_INVALID_KEY_HASH_ALGORITHM = 0X0D45, //!< Invalid key hash algorithm
  RETURN_CODE_INVALID_KEY_ENCRYPTION_ALRORITHM = 0X0D46, //!< Invalid key encryption algorithm
  RETURN_CODE_INVALID_MODULUS_LENGTH = 0X0D47, //!< Invalid modulus length
  RETURN_CODE_INVALID_EXPONENT = 0X0D48, //!< Invalid exponent
  RETURN_CODE_KEY_ALREADY_EXISTS = 0X0D49, //!< Key already exists
  RETURN_CODE_NO_SPACE_FOR_NEW_RID = 0X0D4A, //!< No space for new RID
  RETURN_CODE_KEY_NOT_FOUND = 0X0D4B, //!< Key not found
  RETURN_CODE_CRYPTO_NOT_RESPONDING = 0X0D4C, //!< Crypto not responding
  RETURN_CODE_CRYPTO_COMMUNICATION_ERROR = 0X0D4D, //!< Crypto communication error
  RETURN_CODE_P2_KEY_MANAGER_ERROR_4E = 0X0D4E, //!< Module-specific error for Key Manager
  RETURN_CODE_ALL_KEY_SLOTS_FULL = 0X0D4F, //!< All key slots are full (maximum number of keys has been installed)
  RETURN_CODE_P2_AUTO_SWITCH_OK = 0X0D50, //!< Auto-Switch OK
  RETURN_CODE_P2_AUTO_SWITCH_FAILED = 0X0D51, //!< Auto-Switch failed
  RETURN_CODE_P2_DATA_DOES_NOT_EXIST = 0X0D60, //!< Data doesn't exist
  RETURN_CODE_P2_DATA_FULL = 0X0D61, //!< Data Full
  RETURN_CODE_P2_WRITE_FLASH_ERROR = 0X0D62, //!< Write Flash Error
  RETURN_CODE_P2_OK_AND_HAVE_NEXT_COMMAND = 0X0D63, //!< OK and have next command
  RETURN_CODE_P2_ACCOUNT_DUKPT_KWY_NOT_EXIST = 0X0D90, //!< Account DUKPT Key not exist
  RETURN_CODE_P2_ACCOUNT_DUKPT_KEY_KSN_EXHAUSTED = 0X0D91, //!< Account DUKPT Key KSN exausted

  RETURN_CODE_UNKNOWN_COMMAND = 0X9031, // Unknown command
  RETURN_CODE_WRONG_PARAMETER = 0X9032, // Wrong parameter (such as the length of the command is incorrect)

  RETURN_CODE_WAIT = 0X9038, //!< Wait (the command couldn't be finished in BWT)
  RETURN_CODE_BUSY = 0X9039, //!< Busy (a previously command has not been finished)
  RETURN_CODE_RETRIES_OVER_LIMIT = 0X903A, //!< Number of retries over limit
  RETURN_CODE_TIMEOUT = 0X8100, //!< Time out

  RETURN_CODE_INVALID_MAN_SYSTEM_DATA = 0X9040, //!< Invalid Manufacturing system data
  RETURN_CODE_NOT_AUTHENTICATED = 0X9041, //!< Not authenticated
  RETURN_CODE_INVALID_MASTER_DUKPT_KEY = 0X9042, //!< Invalid Master DUKPT Key
  RETURN_CODE_INVALID_MAC_KEY = 0X9043, //!< Invalid MAC Key
  RETURN_CODE_RESERVED_FOR_FUTURE_USE = 0X9044, //!< Reserved for future use
  RETURN_CODE_RESERVED_FOR_FUTURE_USE_ = 0X9045, //!< Reserved for future use
  RETURN_CODE_INVALID_DATA_DUKPT_KEY = 0X9046, //!< Invalid DATA DUKPT Key
  RETURN_CODE_INVALID_PIN_PARING_DUKPT_KEY = 0X9047, //!< Invalid PIN Pairing DUKPT Key
  RETURN_CODE_INVALID_DATA_PAIRING_DUKPT_KEY = 0X9048, //!< Invalid DATA Pairing DUKPT Key
  RETURN_CODE_NO_NONCE_GENERATED = 0X9049, //!< No nonce generated
  RETURN_CODE_NO_GUID_AVAILABLE = 0X9949, //!< No GUID available.  Perform getVersion first.
  RETURN_CODE_NO_MAC_CALCULATION = 0X9950, //!< MAC Calculation unsuccessful. Check BDK value.

  RETURN_CODE_NOT_READY = 0X904A, //!< Not ready
  RETURN_CODE_MSR_DATA_FAILED = 0X904B, //!< MSR Data Failed

  RETURN_CODE_INVALID_CERTIFICATE = 0X9050, //!< Invalid Certificate
  RETURN_CODE_DUPLICATE_KEY_DETECTED = 0X9051, //!< Duplicate key detected
  RETURN_CODE_AT_CHECKS_FAILED = 0X9052, //!< AT checks failed
  RETURN_CODE_TR34_CHECKS_FAILED = 0X9053, //!< TR34 checks failed
  RETURN_CODE_TR31_CHECKS_FAILED = 0X9054, //!< TR31 checks failed
  RETURN_CODE_AMC_CHECKS_FAILED = 0X9055, //!< MAC checks failed
  RETURN_CODE_FIRMWARE_DOWNLOAD_FAILED = 0X9056, //!< Firmware download failed

  RETURN_CODE_LOG_IS_FULL = 0X9060, //!< Log is full
  RETURN_CODE_REMOVAL_SENSOR_UNENGAGED = 0X9061, //!< Removal sensor unengaged
  RETURN_CODE_HARDWARE_PROBLEM = 0X9062, //!< Any hardware problems

  RETURN_CODE_ICC_COMMUNICATION_TIMEOUT = 0X9070, //!< ICC communication timeout
  RETURN_CODE_IFF_DATA_ERROR = 0X9071, //!< ICC data error (such check sum error)
  RETURN_CODE_SMART_CARD_NOT_POWERED_UP = 0X9072, //!< Smart Card not powered up
  RETURN_CODE_NO_AID = 0xF200, //!< No AID or No Application Data
  RETURN_CODE_NO_TERMINAL_DATA = 0xF201, //!< No Terminal Data
  RETURN_CODE_WRONG_TLV_FORMAT = 0xF202, //!< Wrong TLV format
  RETURN_CODE_AID_LIST_FULL = 0xF203, //!< AID list is full, maxim is 16
  RETURN_CODE_NO_CA_KEY = 0xF204, //!< No any CA Key
  RETURN_CODE_NO_CA_KEY_RID = 0xF205, //!< No CA Key  RID
  RETURN_CODE_NO_CA_KEY_INDEX = 0xF206, //!< No CA Key  Index
  RETURN_CODE_CA_KEY_LIST_FULL = 0xF207, //!< CA Key  list is full, maxim is 96
  RETURN_CODE_CA_KEY_HASH_ERROR = 0xF208, //!< Wrong CA Key hash
  RETURN_CODE_COMMAND_FORMAT_ERROR = 0xF209, //!< Wrong Transaction Command Format
  RETURN_CODE_UNEXPECTED_COMMAND = 0xF20A, //!< Unexpected Command
  RETURN_CODE_NO_CRL = 0xF20B, //!< No CRL
  RETURN_CODE_CRL_LIST_FULL = 0xF20C, //!< CRL list is full, maxim is 30
  RETURN_CODE_MISSING_REQUIRED_PARAMETERS = 0xF20D, //!< No amount, other amount and transaction type in Transaction Command
  RETURN_CODE_CA_INCORRECT_HASH_ALGORITHM = 0xF20E, //!< Wrong CA Hash and Encryption algorithm


  RETURN_CODE_EMV_AUTHORIZATION_ACCEPTED = 0x0E00,          //!< Authorization Accepted
  RETURN_CODE_EMV_AUTHORIZATION_UNABLE_TO_GO_ONLINE = 0x0E01,   //!< Unable to go online
  RETURN_CODE_EMV_AUTHORIZATION_TECHNICAL_ISSUE = 0x0E02,   //!< Technical Issue
  RETURN_CODE_EMV_AUTHORIZATION_DECLINED = 0x0E03,           //!< Declined
  RETURN_CODE_EMV_AUTHORIZATION_ISSUER_REFERRAL = 0x0E04,           //!< Issuer Referral transaction
  RETURN_CODE_EMV_APPROVED = 0x0F00,   //!< Accept the online transaction
  RETURN_CODE_EMV_DECLINED = 0x0F01,   //!< Decline the online transaction
  RETURN_CODE_EMV_GO_ONLINE = 0x0F02,          //!< Request to go online
  RETURN_CODE_EMV_FAILED = 0x0F03,             //!< Transaction is terminated
  RETURN_CODE_EMV_SYSTEM_ERROR = 0x0F05,       //!< Application was not selected by kernel or ICC format error or ICC missing data error
  RETURN_CODE_EMV_NOT_ACCEPTED = 0x0F07,       //!< ICC didn't accept transaction
  RETURN_CODE_EMV_FALLBACK = 0x0F0A,           //!< Application may fallback to magstripe technology
  RETURN_CODE_EMV_CANCEL = 0x0F0C,             //!< Transaction was cancelled
  RETURN_CODE_EMV_TIMEOUT = 0x0F0D,             //!< Timeout
  RETURN_CODE_EMV_OTHER_ERROR = 0x0F0F,        //!< Other EMV Error
  RETURN_CODE_EMV_OFFLINE_APPROVED = 0x0F10,   //!< Accept the offline transaction
  RETURN_CODE_EMV_OFFLINE_DECLINED = 0x0F11,   //!< Decline the offline transaction
  RETURN_CODE_EMV_NEW_SELECTION = 0x0F21,      //!< ICC detected tah the conditions of use are not satisfied
  RETURN_CODE_EMV_NO_AVAILABLE_APPS = 0x0F22,   //!< No app were found on card matching terminal configuration
  RETURN_CODE_EMV_NO_TERMINAL_FILE = 0x0F23,   //!< Terminal file does not exist
  RETURN_CODE_EMV_NO_CAPK_FILE = 0x0F24,       //!< CAPK file does not exist
  RETURN_CODE_EMV_NO_CRL_ENTRY = 0x0F25,       //!< CRL Entry does not exist
  RETURN_CODE_BLOCKING_DISABLED = 0x0FFE,        //!< Return code when blocking is disabled
  RETURN_CODE_CM100_WITHOUT_ERROR = 0xBBE0,        //!< CM100 Success
  RETURN_CODE_CM100_PARAMETER =  0xBBE1,        //!< CM100 Parameter Error
  RETURN_CODE_CM100_LOWOUTBUFFER =  0xBBE2,        //!< CM100 Low Output Buffer
  RETURN_CODE_CM100_CARD_NOT_FOUND =  0xBBE3,        //!< CM100 Card Not Found
  RETURN_CODE_CM100_COLLISION_CARD_EXIST = 0xBBE4,        //!< CM100 Collision Card Exists
  RETURN_CODE_CM100_TOO_MANY_CARDS_EXIST = 0xBBE5,        //!< CM100 Too Many Cards Exist
  RETURN_CODE_CM100_SAVED_DATA_NOT_EXIST =  0xBBE6,        //!< CM100 Saved Data Does Not Exist
  RETURN_CODE_CM100_NO_DATA_AVAILABLE =  0xBBE8,        //!< CM100 No Data Available
  RETURN_CODE_CM100_INVALID_CID_RETURNED =  0xBBE9,        //!< CM100 Invalid CID Returned
  RETURN_CODE_CM100_INVALID_CARD_EXIST = 0xBBEA,        //!< CM100 Invalid Card Exists
  RETURN_CODE_CM100_COMMAND_UNSUPPORTED =  0xBBEC,        //!< CM100 Command Unsupported
  RETURN_CODE_CM100_COMMAND_PROCESS = 0xBBED,        //!< CM100 Error In Command Process
  RETURN_CODE_CM100_INVALID_COMMAND =  0xBBEE,        //!< CM100 Invalid Command

#endif
  RETURN_CODE_COMMAND_UNAVAILABLE = 0x0FFF       //!< Return code when command is not applicable on the selected device
};

enum IDG_P2_STATUS_CODE
{
  IDG_P2_STATUS_CODE_DO_SUCCESS = 0,
  IDG_P2_STATUS_CODE_INCORRECT_HEADER_TAG = 0x01,
  IDG_P2_STATUS_CODE_UNKNOWN_COMMAND = 0x02,
  IDG_P2_STATUS_CODE_UNKNOWN_SUB_COMMAND = 0x03,
  IDG_P2_STATUS_CODE_CRC_ERROR_IN_FRAME = 0x04,
  IDG_P2_STATUS_CODE_INCORRECT_PARAMETER = 0x05,
  IDG_P2_STATUS_CODE_PARAMETER_NOT_SUPPORTED = 0x06,
  IDG_P2_STATUS_CODE_MAL_FORMATTED_DATA = 0X07,
  IDG_P2_STATUS_CODE_TIMEOUT = 0x08,
  IDG_P2_STATUS_CODE_FAILED_NACK = 0x0A,
  IDG_P2_STATUS_CODE_COMMAND_NOT_ALLOWED = 0x0B,
  IDG_P2_STATUS_CODE_SUB_COMMAND_NOT_ALLOWED = 0x0C,
  IDG_P2_STATUS_CODE_BUFFER_OVERFLOW = 0x0D,
  IDG_P2_STATUS_CODE_USER_INTERFACE_EVENT = 0x0E,
  IDG_P2_STATUS_CODE_NEED_CLEAR_FIRMWARE = 0x10,
  IDG_P2_STATUS_CODE_COMMUNICATION_TYPE_NOT_SUPPORTED = 0x11,
  IDG_P2_STATUS_CODE_SECURE_INTERFACE_NOT_FUNCTIONAL = 0x12,
  IDG_P2_STATUS_CODE_DATA_FIELD_NOT_MOD_8 = 0x13,
  IDG_P2_STATUS_CODE_PAD_0x80_NOT_FOUND = 0x14,
  IDG_P2_STATUS_CODE_SPECIFIED_KEY_TYPE_INVALID = 0x15,
  IDG_P2_STATUS_CODE_FAIL_RETRIEVE_KEY_FROM_SAM = 0x16,
  IDG_P2_STATUS_CODE_HASH_CODE_PROBLEM = 0x17,
  IDG_P2_STATUS_CODE_FAIL_STORE_KEY_INTO_SAM = 0x18,
  IDG_P2_STATUS_CODE_FRAME_TOO_LARGE = 0x19,
  IDG_P2_STATUS_CODE_POS_RESEND_INITSECURECOMM = 0x1A,
  IDG_P2_STATUS_CODE_EEPROM_NOT_INITIALIZED = 0x1B,
  IDG_P2_STATUS_CODE_PROBLEM_ENCODING_APDU = 0x1C,
  IDG_P2_STATUS_CODE_UNSUPPORTED_INDEX_SAM = 0x20,
  IDG_P2_STATUS_CODE_UNEXPECTED_SEQUENCE_COUNTER = 0x21,
  IDG_P2_STATUS_CODE_IMPROPER_BIT_MAP = 0x22,
  IDG_P2_STATUS_CODE_REQUEST_ONLINE_AUTHORIZATION = 0x23,
  IDG_P2_STATUS_CODE_VIVOCARD3_RAW_DATA_READ_SUCCESSFUL = 0x24,
  IDG_P2_STATUS_CODE_MESSAGE_INDEX_NOT_AVAILABLE = 0x25,
  IDG_P2_STATUS_CODE_VERSION_INFORMATION_MISMATCH = 0x26,
  IDG_P2_STATUS_CODE_NOT_SENDIG_COMMANDS_IN_CORRECT_INDEX_MESSAGE = 0x27,
  IDG_P2_STATUS_CODE_EXPECTED_MESSAGE_NOT_RECEIVED = 0x28,
  IDG_P2_STATUS_CODE_ILM_LANGUAGES_NOT_AVAILABLE = 0x29,
  IDG_P2_STATUS_CODE_OTHER_LANGUAGE_NOT_SUPPORTED = 0x2A,
  IDG_P2_STATUS_CODE_MODULE_SPECIFIC_ERROR_FOR_KEY_MANAGER_1 = 0x41,
  IDG_P2_STATUS_CODE_MODULE_SPECIFIC_ERROR_FOR_KEY_MANAGER_2 = 0x42,
  IDG_P2_STATUS_CODE_MODULE_SPECIFIC_ERROR_FOR_KEY_MANAGER_3 = 0x43,
  IDG_P2_STATUS_CODE_MODULE_SPECIFIC_ERROR_FOR_KEY_MANAGER_4 = 0x44,
  IDG_P2_STATUS_CODE_MODULE_SPECIFIC_ERROR_FOR_KEY_MANAGER_5 = 0x45,
  IDG_P2_STATUS_CODE_MODULE_SPECIFIC_ERROR_FOR_KEY_MANAGER_6 = 0x46,
  IDG_P2_STATUS_CODE_MODULE_SPECIFIC_ERROR_FOR_KEY_MANAGER_7 = 0x47,
  IDG_P2_STATUS_CODE_MODULE_SPECIFIC_ERROR_FOR_KEY_MANAGER_8 = 0x48,
  IDG_P2_STATUS_CODE_MODULE_SPECIFIC_ERROR_FOR_KEY_MANAGER_9 = 0x49,
  IDG_P2_STATUS_CODE_MODULE_SPECIFIC_ERROR_FOR_KEY_MANAGER_10 = 0x4A,
  IDG_P2_STATUS_CODE_MODULE_SPECIFIC_ERROR_FOR_KEY_MANAGER_11 = 0x4B,
  IDG_P2_STATUS_CODE_MODULE_SPECIFIC_ERROR_FOR_KEY_MANAGER_12 = 0x4C,
  IDG_P2_STATUS_CODE_MODULE_SPECIFIC_ERROR_FOR_KEY_MANAGER_13 = 0x4D,
  IDG_P2_STATUS_CODE_MODULE_SPECIFIC_ERROR_FOR_KEY_MANAGER_14 = 0x4E,
  IDG_P2_STATUS_CODE_MODULE_SPECIFIC_ERROR_FOR_KEY_MANAGER_15 = 0x4F,
  IDG_P2_STATUS_CODE_AUTO_SWITCH_OK = 0x50,
  IDG_P2_STATUS_CODE_AUTO_SWITCH_FAILED = 0x51,
  IDG_P2_STATUS_CODE_OK_AND_HAVE_NEXT_COMMAND = 0x63,
  IDG_P2_STATUS_CODE_ANTENNA_ERROR = 0x70,
  IDG_P2_STATUS_CODE_USE_ANOTHER_CARD = 0x80,
  IDG_P2_STATUS_CODE_INSERT_OR_SWIPE_CARD = 0x81,
  IDG_P2_STATUS_CODE_DATA_ENCRYPTION_KEY_NOT_EXIST = 0x90,
  IDG_P2_STATUS_CODE_DATA_ENCRYPTION_KEY_KSN_EXHAUSTED = 0x91,
  IDG_P2_STATUS_CODE_BUFFER_NOT_ENOUGH = 0x92,
  IDG_P2_STATUS_CODE_DISCONNECTED = 0x93,
  IDG_P2_STATUS_CODE_SDK_BUSY_MSR = 0x94,
  IDG_P2_STATUS_CODE_SDK_BUSY_PINPAD = 0x95,
  IDG_P2_STATUS_CODE_SDK_BUSY_EMV = 0x96,
  IDG_P2_STATUS_CODE_SDK_BUSY_CTLS = 0x97,
  IDG_P2_STATUS_CODE_FW_INIT_SUCCESS = 0x98,
  IDG_P2_STATUS_CODE_NO_DRIVE_SPACE = 0x99,

  IDG_P2_USER_NOT_VALIDATED_TO_PERFORM_THIS_FUNCTION = 0x9A,
  IDG_P2_SERVER_NOT_RESPONDING = 0x9B,
  IDG_P2_INVALID_URL = 0x9C,
  IDG_P2_UNABLE_TO_VALIDATE_WITH_SERVER = 0x9D,
  IDG_P2_FILE_NOT_FOUND_ON_SERVER = 0x9E,
  IDG_P2_FILE_NOT_FOUND_ON_PATH = 0x9F,

  IDG_P2_STATUS_CODE_CDCVM = 0xA0,  //Error code for CDCVM
  IDG_P2_STATUS_CODE_CARD_INSERTED = 0xA3  //Error code for card insertion
};

#endif /* __IDTECHSDK_DEF_H__ */

/*! \file IDTDef.h
  \brief Common Definitions.

  Common Definitions for Global API methods.
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
