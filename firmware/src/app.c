/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

#include "app.h"

APP_DATA appData;

MEDIA_CONTROLLER_INPUT_REPORT_T  __attribute__((aligned(16))) controllerInputReport USB_ALIGN;
MEDIA_CONTROLLER_OUTPUT_REPORT_T  __attribute__((aligned(16))) controllerOutputReport USB_ALIGN;

USB_DEVICE_HID_EVENT_RESPONSE APP_USBDeviceHIDEventHandler
(
    USB_DEVICE_HID_INDEX hidInstance,
    USB_DEVICE_HID_EVENT event,
    void * eventData,
    uintptr_t userData
) {
    APP_DATA * appDataObject = (APP_DATA *)userData;

    switch(event)
    {
        case USB_DEVICE_HID_EVENT_REPORT_SENT:

            /* This means the mouse report was sent.
             We are free to send another report */

            appDataObject->isReportSentComplete = true;
            break;

        case USB_DEVICE_HID_EVENT_REPORT_RECEIVED:

            /* This means we have received a report */
            appDataObject->isReportReceived = true;
            break;

        case USB_DEVICE_HID_EVENT_SET_IDLE:

             /* Acknowledge the Control Write Transfer */
           USB_DEVICE_ControlStatus(appDataObject->deviceHandle, USB_DEVICE_CONTROL_STATUS_OK);

            /* save Idle rate received from Host */
            appDataObject->idleRate
                   = ((USB_DEVICE_HID_EVENT_DATA_SET_IDLE*)eventData)->duration;
            break;

        case USB_DEVICE_HID_EVENT_GET_IDLE:

            /* Host is requesting for Idle rate. Now send the Idle rate */
            USB_DEVICE_ControlSend(appDataObject->deviceHandle, & (appDataObject->idleRate),1);

            /* On successfully receiving Idle rate, the Host would acknowledge back with a
               Zero Length packet. The HID function driver returns an event
               USB_DEVICE_HID_EVENT_CONTROL_TRANSFER_DATA_SENT to the application upon
               receiving this Zero Length packet from Host.
               USB_DEVICE_HID_EVENT_CONTROL_TRANSFER_DATA_SENT event indicates this control transfer
               event is complete */

            break;

        case USB_DEVICE_HID_EVENT_SET_PROTOCOL:
            /* Host is trying set protocol. Now receive the protocol and save */
            appDataObject->activeProtocol
                = ((USB_DEVICE_HID_EVENT_DATA_SET_PROTOCOL *)eventData)->protocolCode;

              /* Acknowledge the Control Write Transfer */
            USB_DEVICE_ControlStatus(appDataObject->deviceHandle, USB_DEVICE_CONTROL_STATUS_OK);
            break;

        case  USB_DEVICE_HID_EVENT_GET_PROTOCOL:

            /* Host is requesting for Current Protocol. Now send the Idle rate */
             USB_DEVICE_ControlSend(appDataObject->deviceHandle, &(appDataObject->activeProtocol), 1);

             /* On successfully receiving Idle rate, the Host would acknowledge
               back with a Zero Length packet. The HID function driver returns
               an event USB_DEVICE_HID_EVENT_CONTROL_TRANSFER_DATA_SENT to the
               application upon receiving this Zero Length packet from Host.
               USB_DEVICE_HID_EVENT_CONTROL_TRANSFER_DATA_SENT event indicates
               this control transfer event is complete */
             break;

        case USB_DEVICE_HID_EVENT_CONTROL_TRANSFER_DATA_SENT:
            break;

        default:
            break;
    }

    return USB_DEVICE_HID_EVENT_RESPONSE_NONE;
}

void APP_USBDeviceEventHandler(USB_DEVICE_EVENT event,
        void * eventData, uintptr_t context)
{
    USB_DEVICE_EVENT_DATA_CONFIGURED *configurationValue;

    switch(event)
    {
        case USB_DEVICE_EVENT_SOF:
            
            /* This event is used for switch de-bounce. This flag is reset
             * by the switch process routine. */
            appData.sofEventHasOccurred = true;
            APP_ReadEncoder();
            break;
            
        case USB_DEVICE_EVENT_RESET:
        case USB_DEVICE_EVENT_DECONFIGURED:

            /* Device got de-configured */
            appData.isConfigured = false;
            appData.state = APP_STATE_WAIT_FOR_CONFIGURATION;            
            break;

        case USB_DEVICE_EVENT_CONFIGURED:

            /* Device is configured */
            configurationValue = (USB_DEVICE_EVENT_DATA_CONFIGURED *)eventData;
            if(configurationValue->configurationValue == 1)
            {
                appData.isConfigured = true;

                /* Register the Application HID Event Handler. */
                USB_DEVICE_HID_EventHandlerSet(appData.hidInstance,
                        APP_USBDeviceHIDEventHandler, (uintptr_t)&appData);
            }
            break;

        case USB_DEVICE_EVENT_SUSPENDED:
            break;

        case USB_DEVICE_EVENT_RESUMED:
            break; 

        case USB_DEVICE_EVENT_POWER_DETECTED:
            
            /* Attach the device */
            USB_DEVICE_Attach (appData.deviceHandle);
            break;

        case USB_DEVICE_EVENT_POWER_REMOVED:
            
            /* There is no VBUS. We can detach the device */
            USB_DEVICE_Detach(appData.deviceHandle);           
            break;
            
        case USB_DEVICE_EVENT_ERROR:
        default:            
            break;

    }
}

void APP_ChangeMode(bool isYoutube) {
    
    appData.isYoutubeMode = isYoutube;
    
    if (appData.isYoutubeMode) {
        LED_INDICATOR_Set();    
    } else {
        LED_INDICATOR_Clear();    
    } 
    
}

void APP_OutputReportHandler() {
    
    SYS_CONSOLE_PRINT("output report: %02x %02x\r\n", 
            controllerOutputReport.reportId, 
            controllerOutputReport.command);
    
    if (controllerOutputReport.reportId == 0x01) {                    
        
        switch (controllerOutputReport.command) {                        
            case 0x01: // Youtube
                APP_ChangeMode(true);
                break;
            case 0x02:
                APP_ChangeMode(false);
                break;
        }
        
    }
    
}

void APP_KeyInputHandler(GPIO_PIN pin, uintptr_t context)
{    
    switch(pin) {
        
        case MECH_SW_NEXT_PIN: // scan next
            SYS_CONSOLE_PRINT("Next\r\n");
            appData.controllerKeycode.flags.next = MECH_SW_NEXT_Get() ? 0 : 1;
            break;
        
        case MECH_SW_PREV_PIN:  // scan previous            
            SYS_CONSOLE_PRINT("Previous\r\n");
            appData.controllerKeycode.flags.prev  = MECH_SW_PREV_Get() ? 0 : 1;
            break;
            
        case MECH_SW_PLAY_PIN:  // play and pause            
            SYS_CONSOLE_PRINT("Play/Pause\r\n");
            appData.controllerKeycode.flags.play = MECH_SW_PLAY_Get() ? 0 : 1;
            break;
            
        case ENCODER_SW_PIN:            
            SYS_CONSOLE_PRINT("Encodor\r\n");
            appData.controllerKeycode.flags.mute = ENCODER_SW_Get() ? 0 : 1;
            break;
            
        case MODE_SW_PIN:
            if (MODE_SW_Get() == 0) {
                SYS_CONSOLE_PRINT("Mode\r\n");
                APP_ChangeMode(!appData.isYoutubeMode);
            }
            break;
        default:
            break;
    }
}


uint8_t APP_FullScreenSequnce() {
    
    switch(appData.fullScreenSqeunceNumber) {        
        case 6: // volume up
            controllerInputReport.reportId = 0x02;
            appData.controllerKeycode.code = 0x10;
            break;
            
        case 4: // volume down
            controllerInputReport.reportId = 0x02;
            appData.controllerKeycode.code = 0x20;
            break;
            
        case 2: // custom code
            controllerInputReport.reportId = 0x01;
            appData.controllerKeycode.code = 0x81;
            break;
            
        case 5:
        case 3:
            controllerInputReport.reportId = 0x02;            
        case 1:
            appData.controllerKeycode.code = 0x00;
            break;
            
        default:
            break;
    }
    
    controllerInputReport.code = appData.controllerKeycode.code;
    
    if (appData.fullScreenSqeunceNumber > 0) {
        appData.fullScreenSqeunceNumber--;
    }
    
    return appData.fullScreenSqeunceNumber;
}

void APP_KeycodeToReport () {
    
    uint8_t funcFlag = MECH_SW_FN_Get() ? 0 : 1;
    
    controllerInputReport.reportId = appData.isYoutubeMode ? 0x01 : 0x02;
    appData.controllerKeycode.flags.func = funcFlag;
    
    if (appData.isYoutubeMode && funcFlag && appData.controllerKeycode.flags.next 
            && appData.fullScreenSqeunceNumber == 0) {
            appData.fullScreenSqeunceNumber = 6;
            appData.controllerKeycode.code = 0;
    }   
    
    controllerInputReport.code = appData.controllerKeycode.code;
    
}

void APP_ReadEncoder() {
    
    uint8_t encoder = GPIO_PortRead(GPIO_PORT_A) & 0x03;   
    
    if (appData.previousEncoderPortValue != encoder) {
        
        SYS_CONSOLE_PRINT("encoder %u\r\n", encoder);
        
         appData.encoderValue = (appData.encoderValue << 2) | encoder;        
        switch(appData.encoderValue) {
            case ENCODER_CW:                
                appData.controllerKeycode.flags.volumeUp = 1;
                break;
            case ENCODER_CCW:                
                appData.controllerKeycode.flags.volumeDown = 1;
                break;
            default:                
                break;               
        }
    } else {
        appData.controllerKeycode.flags.volumeUp = 0;
        appData.controllerKeycode.flags.volumeDown = 0;
    }
    appData.previousEncoderPortValue =  encoder;
    
}



void APP_StateReset(void)
{
    appData.isReportReceived = false;
    appData.isReportSentComplete = true;
    memset(&controllerOutputReport.data, 0, 64);
    
}

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;

    appData.deviceHandle = USB_DEVICE_HANDLE_INVALID;
    appData.isConfigured = false;

    /* Initialize the led state */
    memset(&controllerOutputReport.data, 0, 64);

    /* Initialize the HID instance index.  */
    appData.hidInstance = 0;

    /* Initialize tracking variables */
    appData.isReportReceived = false;
    appData.isReportSentComplete = true;    
    
    appData.encoderValue = 0;
    appData.isYoutubeMode = false;
    appData.previousEncoderPortValue = GPIO_PortRead(GPIO_PORT_A) & 0x03;
    appData.fullScreenSqeunceNumber = 0;
    
    GPIO_PinInterruptCallbackRegister(MECH_SW_PREV_PIN, APP_KeyInputHandler, (uintptr_t)NULL);
    GPIO_PinInterruptCallbackRegister(MECH_SW_NEXT_PIN, APP_KeyInputHandler, (uintptr_t)NULL);
    GPIO_PinInterruptCallbackRegister(MECH_SW_PLAY_PIN, APP_KeyInputHandler, (uintptr_t)NULL);
    GPIO_PinInterruptCallbackRegister(ENCODER_SW_PIN, APP_KeyInputHandler, (uintptr_t)NULL);
    GPIO_PinInterruptCallbackRegister(MODE_SW_PIN, APP_KeyInputHandler, (uintptr_t)NULL);
    MECH_SW_PREV_InterruptEnable();
    MECH_SW_NEXT_InterruptEnable();
    MECH_SW_PLAY_InterruptEnable();
    ENCODER_SW_InterruptEnable();
    MODE_SW_InterruptEnable();
    
    SYS_CONSOLE_PRINT("Youtube Media Controller %u\r\n", appData.previousEncoderPortValue);
    
}

void APP_Tasks ( void )
{   
    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
		    /* Open the device layer */
            appData.deviceHandle = USB_DEVICE_Open( USB_DEVICE_INDEX_0,
                    DRV_IO_INTENT_READWRITE );

            if(appData.deviceHandle != USB_DEVICE_HANDLE_INVALID)
            {
                /* Register a callback with device layer to get event notification (for end point 0) */
                USB_DEVICE_EventHandlerSet(appData.deviceHandle, APP_USBDeviceEventHandler, 0);

                appData.state = APP_STATE_WAIT_FOR_CONFIGURATION;
            }
            else
            {
                /* The Device Layer is not ready to be opened. We should try
                 * again later. */
            }
            break;
        }

        case APP_STATE_WAIT_FOR_CONFIGURATION:

            /* Check if the device is configured. The
             * isConfigured flag is updated in the
             * Device Event Handler */

            if(appData.isConfigured)
            {
                /* Initialize the flag and place a request for a
                 * output report */
                appData.isReportReceived = false;

                USB_DEVICE_HID_ReportReceive(appData.hidInstance, &appData.receiveTransferHandle,
                        (uint8_t *)&controllerOutputReport, 64);

                appData.state = APP_STATE_CHECK_IF_CONFIGURED;
            }

            break;

        case APP_STATE_CHECK_IF_CONFIGURED:

            /* This state is needed because the device can get
             * unconfigured asynchronously. Any application state
             * machine reset should happen within the state machine
             * context only. */

            if(appData.isConfigured)
            {
                appData.state = APP_STATE_CHECK_FOR_OUTPUT_REPORT;
            }
            else
            {
                /* This means the device got de-configured.
                 * We reset the state and the wait for configuration */

                APP_StateReset();
                appData.state = APP_STATE_WAIT_FOR_CONFIGURATION;
            }
            break;

        case APP_STATE_CHECK_FOR_OUTPUT_REPORT:

            if (appData.isReportReceived == true) {
                
                appData.isReportReceived = false;
                USB_DEVICE_HID_ReportReceive(appData.hidInstance, &appData.receiveTransferHandle,
                        (uint8_t *)&controllerOutputReport, 64);
                
                APP_OutputReportHandler();             
            }

            appData.state = APP_STATE_EMULATE_KEYBOARD;
            break;

        case APP_STATE_EMULATE_KEYBOARD:
            
            if(appData.isReportSentComplete)
            {
                /* This means report can be sent*/
                if ((appData.controllerKeycode.code && 0x3F) != appData.previousKeycode 
                        || appData.fullScreenSqeunceNumber > 0) {
                                        
                    if (appData.fullScreenSqeunceNumber > 0) {
                        APP_FullScreenSequnce();                    
                    } else {
                        APP_KeycodeToReport();    
                    }
                    
                    USB_DEVICE_HID_ReportSend(appData.hidInstance, &appData.sendTransferHandle,
                        (uint8_t *)&controllerInputReport, sizeof(MEDIA_CONTROLLER_INPUT_REPORT_T));
                    
                    appData.isReportSentComplete = false;
                }
                
                appData.previousKeycode = appData.controllerKeycode.code & 0x3F;
                appData.controllerKeycode.code = 0;
             }
            
            appData.state = APP_STATE_CHECK_IF_CONFIGURED;
            break;

        case APP_STATE_ERROR:
            break;

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}


/*******************************************************************************
 End of File
 */
