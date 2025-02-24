#ifndef NRF24_H_
#define NRF24_H_

#include <Arduino.h>
#include <SPI.h> 	//must be included in the ino file (where the main is)

/* Memory Map */
#define CONFIG      0x00
#define EN_AA       0x01
#define EN_RXADDR   0x02
#define SETUP_AW    0x03
#define SETUP_RETR  0x04
#define RF_CH       0x05
#define RF_SETUP    0x06
#define STATUS      0x07
#define OBSERVE_TX  0x08
#define CD          0x09
#define RX_ADDR_P0  0x0A
#define RX_ADDR_P1  0x0B
#define RX_ADDR_P2  0x0C
#define RX_ADDR_P3  0x0D
#define RX_ADDR_P4  0x0E
#define RX_ADDR_P5  0x0F
#define TX_ADDR     0x10
#define RX_PW_P0    0x11
#define RX_PW_P1    0x12
#define RX_PW_P2    0x13
#define RX_PW_P3    0x14
#define RX_PW_P4    0x15
#define RX_PW_P5    0x16
#define FIFO_STATUS 0x17
#define DYNPD	    0x1C
#define FEATURE	    0x1D

/* Bit Mnemonics */
#define MASK_RX_DR  6
#define MASK_TX_DS  5
#define MASK_MAX_RT 4
#define EN_CRC      3
#define CRCO        2
#define PWR_UP      1
#define PRIM_RX     0
#define RX_DR       6
#define TX_DS       5
#define FIFO_FULL   5
#define TX_EMPTY    4
#define RX_FULL     1
#define RX_EMPTY    0

/* Instruction Mnemonics */
#define R_REGISTER    0x00
#define W_REGISTER    0x20
#define REGISTER_MASK 0x1F
#define ACTIVATE      0x50
#define R_RX_PL_WID   0x60
#define R_RX_PAYLOAD  0x61
#define W_TX_PAYLOAD  0xA0
#define W_ACK_PAYLOAD 0xA8
#define FLUSH_TX      0xE1
#define FLUSH_RX      0xE2
#define REUSE_TX_PL   0xE3
#define NRF24_NOP     0xFF

class nRF24
{
  bool dynamic_payloads_enabled;
public :
  nRF24();
  ~nRF24();
  void setPins(uint8_t cePin, uint8_t csPin);
  void init(uint8_t payloadSize);
  inline void setCeLow() {digitalWrite(mCePin,LOW);} 
  inline void setCeHigh() {digitalWrite(mCePin,HIGH);} 
  inline void setCsLow() {digitalWrite(mCsPin,LOW);} 
  inline void setCsHigh() {digitalWrite(mCsPin,HIGH);}
  inline void switchFreq(uint8_t freq){writeRegister(RF_CH, freq);}
  inline bool rxFlag(){return (readRegister(STATUS) & _BV(RX_DR));} 
  inline void resetRxFlag(){writeRegister(STATUS, _BV(RX_DR));}
  inline bool rxEmpty(){return (readRegister(FIFO_STATUS) & _BV(RX_EMPTY));} 
  uint8_t readPayload(void* buf, uint8_t len);
  uint8_t writePayload(const void* buf, uint8_t len);
  void rxMode(uint8_t initialFreq);
  void txMode(uint8_t initialFreq);
  uint8_t flushRx();
  uint8_t flushTx();
  void activate(uint8_t code);
  uint8_t readRegister(uint8_t reg, uint8_t* buf, uint8_t len);
  uint8_t readRegister(uint8_t reg);
  uint8_t writeRegister(uint8_t reg, const uint8_t* buf, uint8_t len);
  uint8_t writeRegister(uint8_t reg, uint8_t value);
protected:  

  uint8_t mCePin;
  uint8_t mCsPin;
  uint8_t mPayloadSize;
};

#endif /* NRF24_H_ */
