// Tested: NUCLEO-F401RE
 
#include "mbed.h"
#include "ble/BLE.h"
#include "ble/services/UARTService.h"
#include "Serial.h"
 
#define UART_BUFFER (UARTService::BLE_UART_SERVICE_MAX_DATA_LEN*10)
 
const static char DEVICE_NAME[] = "UART";
UARTService *uartServicePtr;
 
// Periferiche
DigitalOut Led1(LED1);              // LED su scheda
Serial pcUSB(USBTX,USBRX);          // USB di comunicazione con PC
DigitalOut myLED(PB_2);             //LED aggiunto esternamente
DigitalIn myButton(USER_BUTTON);    // USER BUTTON su scheda
DigitalOut myRelay(PA_15);          // Relay Esterno
 
 
// Array e indice di comunicazione con UART
static uint8_t uartBuff[UART_BUFFER];
static uint8_t uartBuffPos=0;
 
// routine di Error Handling
void onBleError(ble_error_t error);
 
 
// Ticker segnale di vita
Ticker LifeTicker;
 
 
 
/****************************************************/
/* Ticker attivo quando presente la connessione BLE */
/****************************************************/
void ConnectedBLE(void)
{
    // segnale di vita: accende e spegne il LED
    Led1 = !Led1;
}
 
/************************/
/* Rx da USB e Tx a BLE */
/************************/
void uartRx(void)
{
    // cattura i caratteri dalla USB fino alla fine del messaggio:  a '\r' oppure '\n' oppure fino al raggiungimento della massima dimensione del messaggio
    if(pcUSB.readable())
    {
        uartBuff[uartBuffPos] = pcUSB.getc();
        if((uartBuff[uartBuffPos] == '\r') || (uartBuff[uartBuffPos] == '\n') || (uartBuffPos >= UART_BUFFER)) 
        {
            // inserisce un NUL nell'ultima posizine della stringa
            uartBuff[uartBuffPos] = '\0';
            // We are sending the whole string even if less than BLE_UART_SERVICE_MAX_DATA_LEN, otherwise we need to wait 
            uartServicePtr->write(uartBuff, (uartBuffPos/UARTService::BLE_UART_SERVICE_MAX_DATA_LEN +1) * UARTService::BLE_UART_SERVICE_MAX_DATA_LEN);
            // invia conferma alla USB-PC con la stringa inviata al BLE
            pcUSB.printf("TX : %s to BLE\r\n", uartBuff);
            // resetta la stringa uartBuff inserendo 0 ai primi UART_BUFFER caratteri
            memset(uartBuff, 0, UART_BUFFER);
            uartBuffPos = 0;
        }
        else
        {
            // se non è stata raggiunta la fine del messaggio, incrementa il contatore per ricevere il prossimo carattere
            uartBuffPos++;
        }
    }
}
 
/*****************************************/
/* Connessione BLE avvenuta con successo */
/*****************************************/
void BleConnectionCallback(const Gap::ConnectionCallbackParams_t *params) 
{
    pcUSB.printf("BLE Client Connected!\n\r");
    pcUSB.printf("Please type a string and press return\r\n");
 
    // segnale di connessione BLE: accende/spegne il LED con periodo 1sec
    LifeTicker.attach(ConnectedBLE, 1);
}
 
/**********************************/
/* disconnessione del Cliente BLE */
/**********************************/
void BleDisconnectionCallback(const Gap::DisconnectionCallbackParams_t *params)
{
    (void)params;
    pcUSB.printf("BLE Client Disconnected!\r\n");
    // spegne il segnale di connessione BLE: accende/spegne il LED con periodo 1sec
    LifeTicker.detach();
    
    // restart advertising
    BLE::Instance().gap().startAdvertising(); 
    Led1=0;
}
 
/************************/
/* Rx da BLE e Rx a USB */
/************************/
void BleOnDataWrittenCallback(const GattWriteCallbackParams *params) 
{
    //  Stringa di risposta in caso di ricezione not OK
    #define REPLYNOTOK "Rx not Ok :("  
    //  Stringa di risposta in caso di ricezione OK 
    #define REPLYOK "Rx Ok :)"         
    //  Stringa di riferimento
    #define REFERENCESTRING "bho"       
   
    char caReply[UART_BUFFER];
    //uint8_t uaAux[UART_BUFFER];
    int nReplyLength;
    
    //Risultato del confronto tra srtinghe
    int nResult;   
   
    // resetta stringa ausiliaria per comporre le risposte 
    memset(caReply, 0, UART_BUFFER);
    
    // riceve stringa da BLE
    if (params->handle == uartServicePtr->getTXCharacteristicHandle())
    {
        // invia al PC la stringa ricevuta
        pcUSB.printf("RX: %s from BLE\r\n", params->data);
        
        //confronta la stringa ricevuta con una stringa di riferimento
        nResult=strcmp((char*)(params->data),REFERENCESTRING);
        if(nResult==0) // ==0 in caso di confronto di stringhe con esito positivo
        {
            // se la stringa ricevuta è uguale a quella di riferimento, accende LED
            myLED = 1;      // accendi LED
            myRelay = 1;    // accendi Relay
            
            // comunica al PC l'azione eseguita
            pcUSB.printf("Relay = ON ; LED = ON \r\n");
             
             // Aggiunge NULL alla stringa di risposta OK 
            strcpy(caReply,REPLYOK);
            nReplyLength = strlen(REPLYOK);
            caReply[nReplyLength]='\0';
           
            // We are sending the whole string even if less than BLE_UART_SERVICE_MAX_DATA_LEN, otherwise we need to wait 
            uartServicePtr->write(caReply, (nReplyLength/UARTService::BLE_UART_SERVICE_MAX_DATA_LEN +1) * UARTService::BLE_UART_SERVICE_MAX_DATA_LEN);
        }
        else
        {
            // se la stringa ricevuta è uguale a quella di riferimento, spegne LED
            myLED = 0;      // spegni LED
            myRelay = 0;    // spegni Relay           
            
            // comunica al PC l'azione eseguita
            pcUSB.printf("Relay = OFF ; LED = OFF \r\n");
            
            // Aggiunge NULL alla stringa di risposta NOT OK 
            strcpy(caReply,REPLYNOTOK);
            nReplyLength = strlen(REPLYNOTOK);
            caReply[nReplyLength]='\0';
                       
            // We are sending the whole string even if less than BLE_UART_SERVICE_MAX_DATA_LEN, otherwise we need to wait 
            uartServicePtr->write(caReply, (nReplyLength/UARTService::BLE_UART_SERVICE_MAX_DATA_LEN +1) * UARTService::BLE_UART_SERVICE_MAX_DATA_LEN);
            
        }
        // !!! PROVARE A INSERIRE IL CASE OF DELL'ESERCIZIO 5
        // !!!!! PROVARE in questo punto a inviare un feedback al BLE !!!!!!!
        // !!!! uartServicePtr->write(uartBuff, (uartBuffPos/UARTService::BLE_UART_SERVICE_MAX_DATA_LEN +1) * UARTService::BLE_UART_SERVICE_MAX_DATA_LEN);
            
    }
}
 
/************************/
/* Errore su canale BLE */
/************************/
void onBleError(ble_error_t error) 
{ 
    pcUSB.printf("BLE Error: %d\r\n");
    /* Inserire Handling di errore */
} 
 
/******************************************/
/* Inizializzazione del servizio BLE UART */
/******************************************/
void bleInitComplete(BLE::InitializationCompleteCallbackContext *params) 
{
    BLE&        ble   = params->ble;
    ble_error_t error = params->error;
 
    if (error != BLE_ERROR_NONE) 
    {
        /* In case of error, forward the error handling to onBleInitError */
        onBleError(error);
        return;
    }
 
    /* Ensure that it is the default instance of BLE */
    if(ble.getInstanceID() != BLE::DEFAULT_INSTANCE) 
    {
        return;
    }
 
    ble.gap().onConnection(BleConnectionCallback);
    ble.gap().onDisconnection(BleDisconnectionCallback);
    ble.gattServer().onDataWritten(BleOnDataWrittenCallback);
 
    pcUSB.printf("BLE UARTService: ");
    /* Setup primary service. */
    UARTService uartService(ble);
    uartServicePtr = &uartService;
    pcUSB.printf("Started\r\n");
 
    /* setup advertising */
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t *)UARTServiceUUID_reversed, sizeof(UARTServiceUUID_reversed));
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME));
    ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    ble.gap().setAdvertisingInterval(500); /* 500ms. */
    ble.gap().startAdvertising();
 
    // rimane in attesa di eventi su BLE
    while (true) 
    {
        ble.waitForEvent();
    }
}
 
/********/
/* MAIN */
/********/
int main(void)
{
    
    // configura velocità della comunicazione seriale su USB-VirtualCom e invia messaggio di benvenuto
    pcUSB.baud(9600); //921600 bps
    // messaggio di benvenuto
    pcUSB.printf("\r\nHallo Amaldi Students - Exercise 6 \r\n");
    pcUSB.printf("\r\n*** Bluetooth - PC serial communication  ***\r\n");
    
    //imposta il funzionamento del pulsante come "PullDown": Aperto = '0'. L'altra modalità di funzinamento è PullUp
    myButton.mode(PullDown);
    // inizializza variabili
    Led1=0;
        
   
    
    // IRQ associata alla ricezione di caratteri da seriale su USB da PC
    pcUSB.attach(uartRx,Serial::RxIrq);
    
    /****** START Inizializza BLE **********/
    BLE &ble = BLE::Instance();
    ble.init(bleInitComplete);
    /******* END Inizializza BLE ***********/
}
