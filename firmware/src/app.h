/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef _APP_H
#define _APP_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "string.h"
#include "configuration.h"
#include "definitions.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************
    
#define ENCODER_CW   ((0x01 << 6) | (0x03 << 4) | (0x02 << 2) | 0x00)
#define ENCODER_CCW   ((0x02 << 6) | (0x03 << 4) | (0x01 << 2) | 0x00)

typedef union
{   
    struct {
        uint8_t next: 1;
        uint8_t prev: 1;
        uint8_t play: 1;
        uint8_t mute: 1;
        uint8_t volumeUp: 1;
        uint8_t volumeDown: 1;
        uint8_t reserved: 1;
        uint8_t func: 1;
    } flags;

    uint8_t code;

} MEDIA_CONTROLLER_KEYCODE_T;

typedef union
{
    
     struct {
         uint8_t reportId;
         uint8_t code;
     };
    
    uint8_t data[2];
    
} MEDIA_CONTROLLER_INPUT_REPORT_T;

typedef union
{
    struct {
        uint8_t reportId;
        uint8_t command;
        uint8_t values[62];
    };
    
    uint8_t data[64];
    
} MEDIA_CONTROLLER_OUTPUT_REPORT_T;


// *****************************************************************************
/* Application states

  Summary:
    Application states enumeration

  Description:
    This enumeration defines the valid application states.  These states
    determine the behavior of the application at various times.
*/

typedef enum
{
    /* Application's state machine's initial state. */
    APP_STATE_INIT = 0,

    /* Application waits for configuration in this state */
    APP_STATE_WAIT_FOR_CONFIGURATION,

    /* Application checks if an output report is available */
    APP_STATE_CHECK_FOR_OUTPUT_REPORT,

    /* Application checks if it is still configured*/
    APP_STATE_CHECK_IF_CONFIGURED,

    /* Application emulates keyboard */
    APP_STATE_EMULATE_KEYBOARD,

    /* Application error state */
    APP_STATE_ERROR
            
} APP_STATES;


// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    Application strings and buffers are be defined outside this structure.
 */

typedef struct
{
    /* The application's current state */
    APP_STATES state;

    /* TODO: Define any additional data used by the application. */

        /* Handle to the device layer */
    USB_DEVICE_HANDLE deviceHandle;

    /* Application HID instance */
    USB_DEVICE_HID_INDEX hidInstance;

    /* Is device configured */
    bool isConfigured;

    /* Track the send report status */
    bool isReportSentComplete;

    /* Track if a report was received */
    bool isReportReceived;

    /* USB HID current Idle */
    uint8_t idleRate;

    /* Flag determines SOF event occurrence */
    bool sofEventHasOccurred;

    /* Receive transfer handle */
    USB_DEVICE_HID_TRANSFER_HANDLE receiveTransferHandle;

    /* Send transfer handle */
    USB_DEVICE_HID_TRANSFER_HANDLE sendTransferHandle;

    /* Key code to be sent */
    USB_HID_KEYBOARD_KEYPAD key;

    /* USB HID active Protocol */
    USB_HID_PROTOCOL_CODE activeProtocol;

    MEDIA_CONTROLLER_KEYCODE_T controllerKeycode;
    bool isYoutubeMode;
    
    uint8_t previousEncoderPortValue; 
    uint8_t previousKeycode;
    uint8_t encoderValue;
    
    uint8_t fullScreenSqeunceNumber;   // for youtube full screen toggle
    
} APP_DATA;

void APP_Initialize ( void );

void APP_Tasks( void );

void APP_ReadEncoder();

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _APP_H */

/*******************************************************************************
 End of File
 */

