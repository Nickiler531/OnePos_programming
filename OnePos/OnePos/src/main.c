/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
 /**
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include "onepos.h"

#define FALSE 0
#define TRUE 1
 
#define USART_RS485						 &USARTE0
#define USART_BLE                        &USARTD0
#define USART_SERIAL_BAUDRATE            115200
#define USART_SERIAL_CHAR_LENGTH         USART_CHSIZE_8BIT_gc
#define USART_SERIAL_PARITY              USART_PMODE_DISABLED_gc
#define USART_SERIAL_STOP_BIT            false


struct spi_device spi_device_conf = {
	.id = UWB_CS
};

void spi_init_module(void)
{
	spi_device_conf.id = UWB_CS;
	spi_master_init(&SPIC);
	spi_master_setup_device(&SPIC, &spi_device_conf, SPI_MODE_0, 1000000, 0);
	spi_enable(&SPIC);
}

typedef struct DW1000_struct
{
	/* register caches. */
	uint8_t       syscfg[LEN_SYS_CFG];
	uint8_t       sysctrl[LEN_SYS_CTRL];
	uint8_t       sysstatus[LEN_SYS_STATUS];
	uint8_t       txfctrl[LEN_TX_FCTRL];
	uint8_t       sysmask[LEN_SYS_MASK];
	uint8_t       chanctrl[LEN_CHAN_CTRL];
	
	/* internal helper that guide tuning the chip. */
	uint8_t       smartPower;
	uint8_t       extendedFrameLength;
	uint8_t       preambleCode;
	uint8_t       channel;
	uint8_t       preambleLength;
	uint8_t       pulseFrequency;
	uint8_t       dataRate;
	uint8_t       pacSize;
	//static DW1000Time _antennaDelay;
	
	/* internal helper to remember how to properly act. */
	uint8_t permanentReceive;
	uint8_t frameCheck;
	
	/* PAN and short address. */
	uint8_t       networkAndAddress[LEN_PANADR];
	
	// whether RX or TX is active
	uint8_t		  deviceMode;
	
	
}DW1000_struct;

DW1000_struct DWM1000_str;


/*
 * Set the value of a bit in an array of bytes that are considered
 * consecutive and stored from MSB to LSB.
 * @param data
 * 		The number as byte array.
 * @param n
 * 		The number of bytes in the array.
 * @param bit
 * 		The position of the bit to be set.
 * @param val
 *		The uint8_t value to be set to the given bit position.
 */
void DW1000_setBit(uint8_t * data, uint16_t n, uint16_t bit, uint8_t val) {
	uint16_t idx;
	uint8_t shift;
	
	idx = bit/8;
	if(idx >= n) {
		return; // TODO proper error handling: out of bounds
	}
	uint8_t* targetByte = &data[idx];
	shift = bit%8;
	if(val) {
		*targetByte |= (1<<shift); //Bit Set
	} else {
		*targetByte &= ~(1<<shift);//bit clear
	}
}

/*
 * Read bytes from the DW1000. Number of bytes depend on register length.
 * @param cmd
 * 		The register address (see Chapter 7 in the DW1000 user manual).
 * @param data
 *		The data array to be read into.
 * @param n
 *		The number of bytes expected to be received.
 */
// TODO incomplete doc
void DW1000_readBytes(uint8_t cmd, uint16_t offset, uint8_t * data, uint16_t n) {
	uint8_t header[3];
	uint8_t headerLen = 1;
	
	// build SPI header
	if(offset == NO_SUB) 
	{
		header[0] = READ | cmd;
	} 
	else 
	{
		header[0] = READ_SUB | cmd;
		if(offset < 128) 
		{
			header[1] = (uint8_t)offset;
			headerLen++;
		} 
		else 
		{
			header[1] = RW_SUB_EXT | (uint8_t)offset;
			header[2] = (uint8_t)(offset >> 7);
			headerLen += 2;
		}
	}
	
	spi_select_device(&SPIC, &spi_device_conf);
	spi_write_packet(&SPIC, header, headerLen);
	spi_read_packet(&SPIC, data, n);
	spi_deselect_device(&SPIC, &spi_device_conf);
}

/*
 * Write bytes to the DW1000. Single bytes can be written to registers via sub-addressing.
 * @param cmd
 * 		The register address (see Chapter 7 in the DW1000 user manual).
 * @param offset
 *		The offset to select register sub-parts for writing, or 0x00 to disable
 * 		sub-adressing.
 * @param data
 *		The data array to be written.
 * @param data_size
 *		The number of bytes to be written (take care not to go out of bounds of
 * 		the register).
 */
// TODO offset really bigger than byte?
void DW1000_writeBytes(uint8_t cmd, uint16_t offset, uint8_t * data, uint16_t data_size) {
	uint8_t header[3];
	uint8_t  headerLen = 1;
	
	// TODO proper error handling: address out of bounds
	// build SPI header
	if(offset == NO_SUB) {
		header[0] = WRITE | cmd;
	} else {
		header[0] = WRITE_SUB | cmd;
		if(offset < 128) {
			header[1] = (uint8_t)offset;
			headerLen++;
		} else {
			header[1] = RW_SUB_EXT | (uint8_t)offset;
			header[2] = (uint8_t)(offset >> 7);
			headerLen += 2;
		}
	}
	
	
	spi_select_device(&SPIC, &spi_device_conf);
	spi_write_packet(&SPIC, header, headerLen);
	spi_write_packet(&SPIC, data, data_size);
	spi_deselect_device(&SPIC, &spi_device_conf);
}

// Helper to set a single register
void DW1000_writeByte(uint8_t cmd, uint16_t offset, uint8_t data) {
	DW1000_writeBytes(cmd, offset, &data, 1);
}

void DW1000_writeValueToBytes(uint8_t * data, int32_t val, uint16_t n) {
	uint16_t i;
	for(i = 0; i < n; i++) {
		data[i] = ((val >> (i*8)) & 0xFF); // TODO bad types - signed unsigned problem
	}
}

void DW1000_idle() 
{
	memset(DWM1000_str.sysctrl, 0, LEN_SYS_CTRL);
	DW1000_setBit(DWM1000_str.sysctrl, LEN_SYS_CTRL, TRXOFF_BIT, 1);
	DWM1000_str.deviceMode = IDLE_MODE;
	DW1000_writeBytes(SYS_CTRL, NO_SUB, DWM1000_str.sysctrl, LEN_SYS_CTRL);
}


// always 4 bytes
// TODO why always 4 bytes? can be different, see p. 58 table 10 otp memory map
void DW1000_readBytesOTP(uint16_t address, uint8_t * data) {
	uint8_t addressBytes[LEN_OTP_ADDR];
	
	// p60 - 6.3.3 Reading a value from OTP memory
	// bytes of address
	addressBytes[0] = (address & 0xFF);
	addressBytes[1] = ((address >> 8) & 0xFF);
	// set address
	DW1000_writeBytes(OTP_IF, OTP_ADDR_SUB, addressBytes, LEN_OTP_ADDR);
	// switch into read mode
	DW1000_writeByte(OTP_IF, OTP_CTRL_SUB, 0x03); // OTPRDEN | OTPREAD
	DW1000_writeByte(OTP_IF, OTP_CTRL_SUB, 0x01); // OTPRDEN
	// read value/block - 4 bytes
	DW1000_readBytes(OTP_IF, OTP_RDAT_SUB, data, LEN_OTP_RDAT);
	// end read mode
	DW1000_writeByte(OTP_IF, OTP_CTRL_SUB, 0x00);
}

/* ###########################################################################
 * #### DW1000 register read/write ###########################################
 * ######################################################################### */

void DW1000_readSystemConfigurationRegister() {
	DW1000_readBytes(SYS_CFG, NO_SUB, DWM1000_str.syscfg, LEN_SYS_CFG);
}

void DW1000_writeSystemConfigurationRegister() {
	DW1000_writeBytes(SYS_CFG, NO_SUB, DWM1000_str.syscfg, LEN_SYS_CFG);
}

void DW1000_readSystemEventStatusRegister() {
	DW1000_readBytes(SYS_STATUS, NO_SUB, DWM1000_str.sysstatus, LEN_SYS_STATUS);
}

void DW1000_readNetworkIdAndDeviceAddress() {
	DW1000_readBytes(PANADR, NO_SUB, DWM1000_str.networkAndAddress, LEN_PANADR);
}

void DW1000_writeNetworkIdAndDeviceAddress() {
	DW1000_writeBytes(PANADR, NO_SUB, DWM1000_str.networkAndAddress, LEN_PANADR);
}

void DW1000_readSystemEventMaskRegister() {
	DW1000_readBytes(SYS_MASK, NO_SUB, DWM1000_str.sysmask, LEN_SYS_MASK);
}

void DW1000_writeSystemEventMaskRegister() {
	DW1000_writeBytes(SYS_MASK, NO_SUB, DWM1000_str.sysmask, LEN_SYS_MASK);
}

void DW1000_readChannelControlRegister() {
	DW1000_readBytes(CHAN_CTRL, NO_SUB, DWM1000_str.chanctrl, LEN_CHAN_CTRL);
}

void DW1000_writeChannelControlRegister() {
	DW1000_writeBytes(CHAN_CTRL, NO_SUB, DWM1000_str.chanctrl, LEN_CHAN_CTRL);
}

void DW1000_readTransmitFrameControlRegister() {
	DW1000_readBytes(TX_FCTRL, NO_SUB, DWM1000_str.txfctrl, LEN_TX_FCTRL);
}

void DW1000_writeTransmitFrameControlRegister() {
	DW1000_writeBytes(TX_FCTRL, NO_SUB, DWM1000_str.txfctrl, LEN_TX_FCTRL);
}


/* ###########################################################################
 * #### DW1000 operation functions ###########################################
 * ######################################################################### */

void DW1000_setNetworkId(uint16_t val) {
	DWM1000_str.networkAndAddress[2] = (uint8_t)(val & 0xFF);
	DWM1000_str.networkAndAddress[3] = (uint8_t)((val >> 8) & 0xFF);
}

void DW1000_setDeviceAddress(uint16_t val) {
	DWM1000_str.networkAndAddress[0] = (uint8_t)(val & 0xFF);
	DWM1000_str.networkAndAddress[1] = (uint8_t)((val >> 8) & 0xFF);
}

void DW1000_setReceiverAutoReenable(uint8_t val) {
	DW1000_setBit(DWM1000_str.syscfg, LEN_SYS_CFG, RXAUTR_BIT, val);
}

void DW1000_useExtendedFrameLength(uint8_t val) {
	DWM1000_str.extendedFrameLength = (val ? FRAME_LENGTH_EXTENDED : FRAME_LENGTH_NORMAL);
	DWM1000_str.syscfg[2] &= 0xFC;
	DWM1000_str.syscfg[2] |= DWM1000_str.extendedFrameLength;
}

void DW1000_receivePermanently(uint8_t val) {
	DWM1000_str.permanentReceive = val;
	if(val) {
		// in case permanent, also reenable receiver once failed
		DW1000_setReceiverAutoReenable(true);
		DW1000_writeSystemConfigurationRegister();
	}
}

void DW1000_useSmartPower(uint8_t smartPower) {
	DWM1000_str.smartPower = smartPower;
	DW1000_setBit(DWM1000_str.syscfg, LEN_SYS_CFG, DIS_STXP_BIT, !smartPower);
}

void DW1000_suppressFrameCheck(uint8_t val) {
	DWM1000_str.frameCheck = !val;
}

//Frame Filtering BIT in the SYS_CFG register
void DW1000_setFrameFilter(uint8_t val) {
	DW1000_setBit(DWM1000_str.syscfg, LEN_SYS_CFG, FFEN_BIT, val);
}

void DW1000_interruptOnSent(uint8_t val) {
	DW1000_setBit(DWM1000_str.sysmask, LEN_SYS_MASK, TXFRS_BIT, val);
}

void DW1000_interruptOnReceived(uint8_t val) {
	DW1000_setBit(DWM1000_str.sysmask, LEN_SYS_MASK, RXDFR_BIT, val);
	DW1000_setBit(DWM1000_str.sysmask, LEN_SYS_MASK, RXFCG_BIT, val);
}

void DW1000_interruptOnReceiveFailed(uint8_t val) {
	DW1000_setBit(DWM1000_str.sysmask, LEN_SYS_STATUS, LDEERR_BIT, val);
	DW1000_setBit(DWM1000_str.sysmask, LEN_SYS_STATUS, RXFCE_BIT, val);
	DW1000_setBit(DWM1000_str.sysmask, LEN_SYS_STATUS, RXPHE_BIT, val);
	DW1000_setBit(DWM1000_str.sysmask, LEN_SYS_STATUS, RXRFSL_BIT, val);
}

void DW1000_interruptOnReceiveTimeout(uint8_t val) {
	DW1000_setBit(DWM1000_str.sysmask, LEN_SYS_MASK, RXRFTO_BIT, val);
}

void DW1000_interruptOnReceiveTimestampAvailable(uint8_t val) {
	DW1000_setBit(DWM1000_str.sysmask, LEN_SYS_MASK, LDEDONE_BIT, val);
}

void DW1000_interruptOnAutomaticAcknowledgeTrigger(uint8_t val) {
	DW1000_setBit(DWM1000_str.sysmask, LEN_SYS_MASK, AAT_BIT, val);
}

void DW1000_clearInterrupts() {
	memset(DWM1000_str.sysmask, 0, LEN_SYS_MASK);
}


void DW1000_setDataRate(uint8_t rate) {
	rate &= 0x03;
	DWM1000_str.txfctrl[1] &= 0x83;
	DWM1000_str.txfctrl[1] |= (uint8_t)((rate << 5) & 0xFF);
	// special 110kbps flag
	if(rate == TRX_RATE_110KBPS) {
		DW1000_setBit(DWM1000_str.syscfg, LEN_SYS_CFG, RXM110K_BIT, TRUE);
		} else {
		DW1000_setBit(DWM1000_str.syscfg, LEN_SYS_CFG, RXM110K_BIT, TRUE);
	}
	// SFD mode and type (non-configurable, as in Table )
	if(rate == TRX_RATE_6800KBPS) {
		DW1000_setBit(DWM1000_str.chanctrl, LEN_CHAN_CTRL, DWSFD_BIT, FALSE);
		DW1000_setBit(DWM1000_str.chanctrl, LEN_CHAN_CTRL, TNSSFD_BIT, FALSE);
		DW1000_setBit(DWM1000_str.chanctrl, LEN_CHAN_CTRL, RNSSFD_BIT, FALSE);
		} else {
		DW1000_setBit(DWM1000_str.chanctrl, LEN_CHAN_CTRL, DWSFD_BIT, TRUE);
		DW1000_setBit(DWM1000_str.chanctrl, LEN_CHAN_CTRL, TNSSFD_BIT, TRUE);
		DW1000_setBit(DWM1000_str.chanctrl, LEN_CHAN_CTRL, RNSSFD_BIT, TRUE);
	}
	uint8_t sfdLength;
	if(rate == TRX_RATE_6800KBPS) {
		sfdLength = 0x08;
		} else if(rate == TRX_RATE_850KBPS) {
		sfdLength = 0x10;
		} else {
		sfdLength = 0x40;
	}
	DW1000_writeBytes(USR_SFD, SFD_LENGTH_SUB, &sfdLength, LEN_SFD_LENGTH);
	DWM1000_str.dataRate = rate;
}

void DW1000_setPulseFrequency(uint8_t freq) {
	freq &= 0x03;
	DWM1000_str.txfctrl[2] &= 0xFC;
	DWM1000_str.txfctrl[2] |= (uint8_t)(freq & 0xFF);
	DWM1000_str.chanctrl[2] &= 0xF3;
	DWM1000_str.chanctrl[2] |= (uint8_t)((freq << 2) & 0xFF);
	DWM1000_str.pulseFrequency = freq;
}

uint8_t DW1000_getPulseFrequency() {
	return DWM1000_str.pulseFrequency;
}

void DW1000_setPreambleLength(uint8_t prealen) {
	prealen &= 0x0F;
	DWM1000_str.txfctrl[2] &= 0xC3;
	DWM1000_str.txfctrl[2] |= (uint8_t)((prealen << 2) & 0xFF);
	if(prealen == TX_PREAMBLE_LEN_64 || prealen == TX_PREAMBLE_LEN_128) {
		DWM1000_str.pacSize = PAC_SIZE_8;
		} else if(prealen == TX_PREAMBLE_LEN_256 || prealen == TX_PREAMBLE_LEN_512) {
		DWM1000_str.pacSize = PAC_SIZE_16;
		} else if(prealen == TX_PREAMBLE_LEN_1024) {
		DWM1000_str.pacSize = PAC_SIZE_32;
		} else {
		DWM1000_str.pacSize = PAC_SIZE_64;
	}
	DWM1000_str.preambleLength = prealen;
}

void DW1000_setChannel(uint8_t channel) {
	channel &= 0xF;
	DWM1000_str.chanctrl[0] = ((channel | (channel << 4)) & 0xFF);
	DWM1000_str.channel = channel;
}

void DW1000_setPreambleCode(uint8_t preacode) {
	preacode &= 0x1F;
	DWM1000_str.chanctrl[2] &= 0x3F;
	DWM1000_str.chanctrl[2] |= ((preacode << 6) & 0xFF);
	DWM1000_str.chanctrl[3] = 0x00;
	DWM1000_str.chanctrl[3] = ((((preacode >> 2) & 0x07) | (preacode << 3)) & 0xFF);
	DWM1000_str.preambleCode = preacode;
}

void DW1000_enableMode(uint8_t * mode) 
{
	DW1000_setDataRate(mode[0]);
	DW1000_setPulseFrequency(mode[1]);
	DW1000_setPreambleLength(mode[2]);
	// TODO add channel and code to mode tuples
	// TODO add channel and code settings with checks (see Table 58)
	DW1000_setChannel(CHANNEL_5);
	if(mode[1] == TX_PULSE_FREQ_16MHZ) {
		DW1000_setPreambleCode(PREAMBLE_CODE_16MHZ_4);
		} else {
		DW1000_setPreambleCode(PREAMBLE_CODE_64MHZ_10);
	}
}


void DW1000_tune() {
	// these registers are going to be tuned/configured
	uint8_t agctune1[LEN_AGC_TUNE1];
	uint8_t agctune2[LEN_AGC_TUNE2];
	uint8_t agctune3[LEN_AGC_TUNE3];
	uint8_t drxtune0b[LEN_DRX_TUNE0b];
	uint8_t drxtune1a[LEN_DRX_TUNE1a];
	uint8_t drxtune1b[LEN_DRX_TUNE1b];
	uint8_t drxtune2[LEN_DRX_TUNE2];
	uint8_t drxtune4H[LEN_DRX_TUNE4H];
	uint8_t ldecfg1[LEN_LDE_CFG1];
	uint8_t ldecfg2[LEN_LDE_CFG2];
	uint8_t lderepc[LEN_LDE_REPC];
	uint8_t txpower[LEN_TX_POWER];
	uint8_t rfrxctrlh[LEN_RF_RXCTRLH];
	uint8_t rftxctrl[LEN_RF_TXCTRL];
	uint8_t tcpgdelay[LEN_TC_PGDELAY];
	uint8_t fspllcfg[LEN_FS_PLLCFG];
	uint8_t fsplltune[LEN_FS_PLLTUNE];
	uint8_t fsxtalt[LEN_FS_XTALT];
	// AGC_TUNE1
	if(DWM1000_str.pulseFrequency == TX_PULSE_FREQ_16MHZ) {
		DW1000_writeValueToBytes(agctune1, 0x8870, LEN_AGC_TUNE1);
		} else if(DWM1000_str.pulseFrequency == TX_PULSE_FREQ_64MHZ) {
		DW1000_writeValueToBytes(agctune1, 0x889B, LEN_AGC_TUNE1);
		} else {
		// TODO proper error/warning handling
	}
	// AGC_TUNE2
	DW1000_writeValueToBytes(agctune2, 0x2502A907L, LEN_AGC_TUNE2);
	// AGC_TUNE3
	DW1000_writeValueToBytes(agctune3, 0x0035, LEN_AGC_TUNE3);
	// DRX_TUNE0b (already optimized according to Table 20 of user manual)
	if(DWM1000_str.dataRate == TRX_RATE_110KBPS) {
		DW1000_writeValueToBytes(drxtune0b, 0x0016, LEN_DRX_TUNE0b);
		} else if(DWM1000_str.dataRate == TRX_RATE_850KBPS) {
		DW1000_writeValueToBytes(drxtune0b, 0x0006, LEN_DRX_TUNE0b);
		} else if(DWM1000_str.dataRate == TRX_RATE_6800KBPS) {
		DW1000_writeValueToBytes(drxtune0b, 0x0001, LEN_DRX_TUNE0b);
		} else {
		// TODO proper error/warning handling
	}
	// DRX_TUNE1a
	if(DWM1000_str.pulseFrequency == TX_PULSE_FREQ_16MHZ) {
		DW1000_writeValueToBytes(drxtune1a, 0x0087, LEN_DRX_TUNE1a);
		} else if(DWM1000_str.pulseFrequency == TX_PULSE_FREQ_64MHZ) {
		DW1000_writeValueToBytes(drxtune1a, 0x008D, LEN_DRX_TUNE1a);
		} else {
		// TODO proper error/warning handling
	}
	// DRX_TUNE1b
	if(DWM1000_str.preambleLength == TX_PREAMBLE_LEN_1536 || DWM1000_str.preambleLength == TX_PREAMBLE_LEN_2048 ||
	DWM1000_str.preambleLength == TX_PREAMBLE_LEN_4096) {
		if(DWM1000_str.dataRate == TRX_RATE_110KBPS) {
			DW1000_writeValueToBytes(drxtune1b, 0x0064, LEN_DRX_TUNE1b);
			} else {
			// TODO proper error/warning handling
		}
		} else if(DWM1000_str.preambleLength != TX_PREAMBLE_LEN_64) {
		if(DWM1000_str.dataRate == TRX_RATE_850KBPS || DWM1000_str.dataRate == TRX_RATE_6800KBPS) {
			DW1000_writeValueToBytes(drxtune1b, 0x0020, LEN_DRX_TUNE1b);
			} else {
			// TODO proper error/warning handling
		}
		} else {
		if(DWM1000_str.dataRate == TRX_RATE_6800KBPS) {
			DW1000_writeValueToBytes(drxtune1b, 0x0010, LEN_DRX_TUNE1b);
			} else {
			// TODO proper error/warning handling
		}
	}
	// DRX_TUNE2
	if(DWM1000_str.pacSize == PAC_SIZE_8) {
		if(DWM1000_str.pulseFrequency == TX_PULSE_FREQ_16MHZ) {
			DW1000_writeValueToBytes(drxtune2, 0x311A002DL, LEN_DRX_TUNE2);
			} else if(DWM1000_str.pulseFrequency == TX_PULSE_FREQ_64MHZ) {
			DW1000_writeValueToBytes(drxtune2, 0x313B006BL, LEN_DRX_TUNE2);
			} else {
			// TODO proper error/warning handling
		}
		} else if(DWM1000_str.pacSize == PAC_SIZE_16) {
		if(DWM1000_str.pulseFrequency == TX_PULSE_FREQ_16MHZ) {
			DW1000_writeValueToBytes(drxtune2, 0x331A0052L, LEN_DRX_TUNE2);
			} else if(DWM1000_str.pulseFrequency == TX_PULSE_FREQ_64MHZ) {
			DW1000_writeValueToBytes(drxtune2, 0x333B00BEL, LEN_DRX_TUNE2);
			} else {
			// TODO proper error/warning handling
		}
		} else if(DWM1000_str.pacSize == PAC_SIZE_32) {
		if(DWM1000_str.pulseFrequency == TX_PULSE_FREQ_16MHZ) {
			DW1000_writeValueToBytes(drxtune2, 0x351A009AL, LEN_DRX_TUNE2);
			} else if(DWM1000_str.pulseFrequency == TX_PULSE_FREQ_64MHZ) {
			DW1000_writeValueToBytes(drxtune2, 0x353B015EL, LEN_DRX_TUNE2);
			} else {
			// TODO proper error/warning handling
		}
		} else if(DWM1000_str.pacSize == PAC_SIZE_64) {
		if(DWM1000_str.pulseFrequency == TX_PULSE_FREQ_16MHZ) {
			DW1000_writeValueToBytes(drxtune2, 0x371A011DL, LEN_DRX_TUNE2);
			} else if(DWM1000_str.pulseFrequency == TX_PULSE_FREQ_64MHZ) {
			DW1000_writeValueToBytes(drxtune2, 0x373B0296L, LEN_DRX_TUNE2);
			} else {
			// TODO proper error/warning handling
		}
		} else {
		// TODO proper error/warning handling
	}
	// DRX_TUNE4H
	if(DWM1000_str.preambleLength == TX_PREAMBLE_LEN_64) {
		DW1000_writeValueToBytes(drxtune4H, 0x0010, LEN_DRX_TUNE4H);
		} else {
		DW1000_writeValueToBytes(drxtune4H, 0x0028, LEN_DRX_TUNE4H);
	}
	// RF_RXCTRLH
	if(DWM1000_str.channel != CHANNEL_4 && DWM1000_str.channel != CHANNEL_7) {
		DW1000_writeValueToBytes(rfrxctrlh, 0xD8, LEN_RF_RXCTRLH);
		} else {
		DW1000_writeValueToBytes(rfrxctrlh, 0xBC, LEN_RF_RXCTRLH);
	}
	// RX_TXCTRL
	if(DWM1000_str.channel == CHANNEL_1) {
		DW1000_writeValueToBytes(rftxctrl, 0x00005C40L, LEN_RF_TXCTRL);
		} else if(DWM1000_str.channel == CHANNEL_2) {
		DW1000_writeValueToBytes(rftxctrl, 0x00045CA0L, LEN_RF_TXCTRL);
		} else if(DWM1000_str.channel == CHANNEL_3) {
		DW1000_writeValueToBytes(rftxctrl, 0x00086CC0L, LEN_RF_TXCTRL);
		} else if(DWM1000_str.channel == CHANNEL_4) {
		DW1000_writeValueToBytes(rftxctrl, 0x00045C80L, LEN_RF_TXCTRL);
		} else if(DWM1000_str.channel == CHANNEL_5) {
		DW1000_writeValueToBytes(rftxctrl, 0x001E3FE0L, LEN_RF_TXCTRL);
		} else if(DWM1000_str.channel == CHANNEL_7) {
		DW1000_writeValueToBytes(rftxctrl, 0x001E7DE0L, LEN_RF_TXCTRL);
		} else {
		// TODO proper error/warning handling
	}
	// TC_PGDELAY
	if(DWM1000_str.channel == CHANNEL_1) {
		DW1000_writeValueToBytes(tcpgdelay, 0xC9, LEN_TC_PGDELAY);
		} else if(DWM1000_str.channel == CHANNEL_2) {
		DW1000_writeValueToBytes(tcpgdelay, 0xC2, LEN_TC_PGDELAY);
		} else if(DWM1000_str.channel == CHANNEL_3) {
		DW1000_writeValueToBytes(tcpgdelay, 0xC5, LEN_TC_PGDELAY);
		} else if(DWM1000_str.channel == CHANNEL_4) {
		DW1000_writeValueToBytes(tcpgdelay, 0x95, LEN_TC_PGDELAY);
		} else if(DWM1000_str.channel == CHANNEL_5) {
		DW1000_writeValueToBytes(tcpgdelay, 0xC0, LEN_TC_PGDELAY);
		} else if(DWM1000_str.channel == CHANNEL_7) {
		DW1000_writeValueToBytes(tcpgdelay, 0x93, LEN_TC_PGDELAY);
		} else {
		// TODO proper error/warning handling
	}
	// FS_PLLCFG and FS_PLLTUNE
	if(DWM1000_str.channel == CHANNEL_1) {
		DW1000_writeValueToBytes(fspllcfg, 0x09000407L, LEN_FS_PLLCFG);
		DW1000_writeValueToBytes(fsplltune, 0x1E, LEN_FS_PLLTUNE);
		} else if(DWM1000_str.channel == CHANNEL_2 || DWM1000_str.channel == CHANNEL_4) {
		DW1000_writeValueToBytes(fspllcfg, 0x08400508L, LEN_FS_PLLCFG);
		DW1000_writeValueToBytes(fsplltune, 0x26, LEN_FS_PLLTUNE);
		} else if(DWM1000_str.channel == CHANNEL_3) {
		DW1000_writeValueToBytes(fspllcfg, 0x08401009L, LEN_FS_PLLCFG);
		DW1000_writeValueToBytes(fsplltune, 0x5E, LEN_FS_PLLTUNE);
		} else if(DWM1000_str.channel == CHANNEL_5 || DWM1000_str.channel == CHANNEL_7) {
		DW1000_writeValueToBytes(fspllcfg, 0x0800041DL, LEN_FS_PLLCFG);
		DW1000_writeValueToBytes(fsplltune, 0xBE, LEN_FS_PLLTUNE);
		} else {
		// TODO proper error/warning handling
	}
	// LDE_CFG1
	DW1000_writeValueToBytes(ldecfg1, 0xD, LEN_LDE_CFG1);
	// LDE_CFG2
	if(DWM1000_str.pulseFrequency == TX_PULSE_FREQ_16MHZ) {
		DW1000_writeValueToBytes(ldecfg2, 0x1607, LEN_LDE_CFG2);
		} else if(DWM1000_str.pulseFrequency == TX_PULSE_FREQ_64MHZ) {
		DW1000_writeValueToBytes(ldecfg2, 0x0607, LEN_LDE_CFG2);
		} else {
		// TODO proper error/warning handling
	}
	// LDE_REPC
	if(DWM1000_str.preambleCode == PREAMBLE_CODE_16MHZ_1 || DWM1000_str.preambleCode == PREAMBLE_CODE_16MHZ_2) {
		if(DWM1000_str.dataRate == TRX_RATE_110KBPS) {
			DW1000_writeValueToBytes(lderepc, ((0x5998 >> 3) & 0xFFFF), LEN_LDE_REPC);
			} else {
			DW1000_writeValueToBytes(lderepc, 0x5998, LEN_LDE_REPC);
		}
		} else if(DWM1000_str.preambleCode == PREAMBLE_CODE_16MHZ_3 || DWM1000_str.preambleCode == PREAMBLE_CODE_16MHZ_8) {
		if(DWM1000_str.dataRate == TRX_RATE_110KBPS) {
			DW1000_writeValueToBytes(lderepc, ((0x51EA >> 3) & 0xFFFF), LEN_LDE_REPC);
			} else {
			DW1000_writeValueToBytes(lderepc, 0x51EA, LEN_LDE_REPC);
		}
		} else if(DWM1000_str.preambleCode == PREAMBLE_CODE_16MHZ_4) {
		if(DWM1000_str.dataRate == TRX_RATE_110KBPS) {
			DW1000_writeValueToBytes(lderepc, ((0x428E >> 3) & 0xFFFF), LEN_LDE_REPC);
			} else {
			DW1000_writeValueToBytes(lderepc, 0x428E, LEN_LDE_REPC);
		}
		} else if(DWM1000_str.preambleCode == PREAMBLE_CODE_16MHZ_5) {
		if(DWM1000_str.dataRate == TRX_RATE_110KBPS) {
			DW1000_writeValueToBytes(lderepc, ((0x451E >> 3) & 0xFFFF), LEN_LDE_REPC);
			} else {
			DW1000_writeValueToBytes(lderepc, 0x451E, LEN_LDE_REPC);
		}
		} else if(DWM1000_str.preambleCode == PREAMBLE_CODE_16MHZ_6) {
		if(DWM1000_str.dataRate == TRX_RATE_110KBPS) {
			DW1000_writeValueToBytes(lderepc, ((0x2E14 >> 3) & 0xFFFF), LEN_LDE_REPC);
			} else {
			DW1000_writeValueToBytes(lderepc, 0x2E14, LEN_LDE_REPC);
		}
		} else if(DWM1000_str.preambleCode == PREAMBLE_CODE_16MHZ_7) {
		if(DWM1000_str.dataRate == TRX_RATE_110KBPS) {
			DW1000_writeValueToBytes(lderepc, ((0x8000 >> 3) & 0xFFFF), LEN_LDE_REPC);
			} else {
			DW1000_writeValueToBytes(lderepc, 0x8000, LEN_LDE_REPC);
		}
		} else if(DWM1000_str.preambleCode == PREAMBLE_CODE_64MHZ_9) {
		if(DWM1000_str.dataRate == TRX_RATE_110KBPS) {
			DW1000_writeValueToBytes(lderepc, ((0x28F4 >> 3) & 0xFFFF), LEN_LDE_REPC);
			} else {
			DW1000_writeValueToBytes(lderepc, 0x28F4, LEN_LDE_REPC);
		}
		} else if(DWM1000_str.preambleCode == PREAMBLE_CODE_64MHZ_10 || DWM1000_str.preambleCode == PREAMBLE_CODE_64MHZ_17) {
		if(DWM1000_str.dataRate == TRX_RATE_110KBPS) {
			DW1000_writeValueToBytes(lderepc, ((0x3332 >> 3) & 0xFFFF), LEN_LDE_REPC);
			} else {
			DW1000_writeValueToBytes(lderepc, 0x3332, LEN_LDE_REPC);
		}
		} else if(DWM1000_str.preambleCode == PREAMBLE_CODE_64MHZ_11) {
		if(DWM1000_str.dataRate == TRX_RATE_110KBPS) {
			DW1000_writeValueToBytes(lderepc, ((0x3AE0 >> 3) & 0xFFFF), LEN_LDE_REPC);
			} else {
			DW1000_writeValueToBytes(lderepc, 0x3AE0, LEN_LDE_REPC);
		}
		} else if(DWM1000_str.preambleCode == PREAMBLE_CODE_64MHZ_12) {
		if(DWM1000_str.dataRate == TRX_RATE_110KBPS) {
			DW1000_writeValueToBytes(lderepc, ((0x3D70 >> 3) & 0xFFFF), LEN_LDE_REPC);
			} else {
			DW1000_writeValueToBytes(lderepc, 0x3D70, LEN_LDE_REPC);
		}
		} else if(DWM1000_str.preambleCode == PREAMBLE_CODE_64MHZ_18 || DWM1000_str.preambleCode == PREAMBLE_CODE_64MHZ_19) {
		if(DWM1000_str.dataRate == TRX_RATE_110KBPS) {
			DW1000_writeValueToBytes(lderepc, ((0x35C2 >> 3) & 0xFFFF), LEN_LDE_REPC);
			} else {
			DW1000_writeValueToBytes(lderepc, 0x35C2, LEN_LDE_REPC);
		}
		} else if(DWM1000_str.preambleCode == PREAMBLE_CODE_64MHZ_20) {
		if(DWM1000_str.dataRate == TRX_RATE_110KBPS) {
			DW1000_writeValueToBytes(lderepc, ((0x47AE >> 3) & 0xFFFF), LEN_LDE_REPC);
			} else {
			DW1000_writeValueToBytes(lderepc, 0x47AE, LEN_LDE_REPC);
		}
		} else {
		// TODO proper error/warning handling
	}
	// TX_POWER (enabled smart transmit power control)
	if(DWM1000_str.channel == CHANNEL_1 || DWM1000_str.channel == CHANNEL_2) {
		if(DWM1000_str.pulseFrequency == TX_PULSE_FREQ_16MHZ) {
			if(DWM1000_str.smartPower) {
				DW1000_writeValueToBytes(txpower, 0x15355575L, LEN_TX_POWER);
				} else {
				DW1000_writeValueToBytes(txpower, 0x75757575L, LEN_TX_POWER);
			}
			} else if(DWM1000_str.pulseFrequency == TX_PULSE_FREQ_64MHZ) {
			if(DWM1000_str.smartPower) {
				DW1000_writeValueToBytes(txpower, 0x07274767L, LEN_TX_POWER);
				} else {
				DW1000_writeValueToBytes(txpower, 0x67676767L, LEN_TX_POWER);
			}
			} else {
			// TODO proper error/warning handling
		}
		} else if(DWM1000_str.channel == CHANNEL_3) {
		if(DWM1000_str.pulseFrequency == TX_PULSE_FREQ_16MHZ) {
			if(DWM1000_str.smartPower) {
				DW1000_writeValueToBytes(txpower, 0x0F2F4F6FL, LEN_TX_POWER);
				} else {
				DW1000_writeValueToBytes(txpower, 0x6F6F6F6FL, LEN_TX_POWER);
			}
			} else if(DWM1000_str.pulseFrequency == TX_PULSE_FREQ_64MHZ) {
			if(DWM1000_str.smartPower) {
				DW1000_writeValueToBytes(txpower, 0x2B4B6B8BL, LEN_TX_POWER);
				} else {
				DW1000_writeValueToBytes(txpower, 0x8B8B8B8BL, LEN_TX_POWER);
			}
			} else {
			// TODO proper error/warning handling
		}
		} else if(DWM1000_str.channel == CHANNEL_4) {
		if(DWM1000_str.pulseFrequency == TX_PULSE_FREQ_16MHZ) {
			if(DWM1000_str.smartPower) {
				DW1000_writeValueToBytes(txpower, 0x1F1F3F5FL, LEN_TX_POWER);
				} else {
				DW1000_writeValueToBytes(txpower, 0x5F5F5F5FL, LEN_TX_POWER);
			}
			} else if(DWM1000_str.pulseFrequency == TX_PULSE_FREQ_64MHZ) {
			if(DWM1000_str.smartPower) {
				DW1000_writeValueToBytes(txpower, 0x3A5A7A9AL, LEN_TX_POWER);
				} else {
				DW1000_writeValueToBytes(txpower, 0x9A9A9A9AL, LEN_TX_POWER);
			}
			} else {
			// TODO proper error/warning handling
		}
		} else if(DWM1000_str.channel == CHANNEL_5) {
		if(DWM1000_str.pulseFrequency == TX_PULSE_FREQ_16MHZ) {
			if(DWM1000_str.smartPower) {
				DW1000_writeValueToBytes(txpower, 0x0E082848L, LEN_TX_POWER);
				} else {
				DW1000_writeValueToBytes(txpower, 0x48484848L, LEN_TX_POWER);
			}
			} else if(DWM1000_str.pulseFrequency == TX_PULSE_FREQ_64MHZ) {
			if(DWM1000_str.smartPower) {
				DW1000_writeValueToBytes(txpower, 0x25456585L, LEN_TX_POWER);
				} else {
				DW1000_writeValueToBytes(txpower, 0x85858585L, LEN_TX_POWER);
			}
			} else {
			// TODO proper error/warning handling
		}
		} else if(DWM1000_str.channel == CHANNEL_7) {
		if(DWM1000_str.pulseFrequency == TX_PULSE_FREQ_16MHZ) {
			if(DWM1000_str.smartPower) {
				DW1000_writeValueToBytes(txpower, 0x32527292L, LEN_TX_POWER);
				} else {
				DW1000_writeValueToBytes(txpower, 0x92929292L, LEN_TX_POWER);
			}
			} else if(DWM1000_str.pulseFrequency == TX_PULSE_FREQ_64MHZ) {
			if(DWM1000_str.smartPower) {
				DW1000_writeValueToBytes(txpower, 0x5171B1D1L, LEN_TX_POWER);
				} else {
				DW1000_writeValueToBytes(txpower, 0xD1D1D1D1L, LEN_TX_POWER);
			}
			} else {
			// TODO proper error/warning handling
		}
		} else {
		// TODO proper error/warning handling
	}
	// Crystal calibration from OTP (if available)
	uint8_t buf_otp[4];
	DW1000_readBytesOTP(0x01E, buf_otp);
	if (buf_otp[0] == 0) {
		// No trim value available from OTP, use midrange value of 0x10
		DW1000_writeValueToBytes(fsxtalt, ((0x10 & 0x1F) | 0x60), LEN_FS_XTALT);
		} else {
		DW1000_writeValueToBytes(fsxtalt, ((buf_otp[0] & 0x1F) | 0x60), LEN_FS_XTALT);
	}
	// write configuration back to chip
	DW1000_writeBytes(AGC_TUNE, AGC_TUNE1_SUB, agctune1, LEN_AGC_TUNE1);
	DW1000_writeBytes(AGC_TUNE, AGC_TUNE2_SUB, agctune2, LEN_AGC_TUNE2);
	DW1000_writeBytes(AGC_TUNE, AGC_TUNE3_SUB, agctune3, LEN_AGC_TUNE3);
	DW1000_writeBytes(DRX_TUNE, DRX_TUNE0b_SUB, drxtune0b, LEN_DRX_TUNE0b);
	DW1000_writeBytes(DRX_TUNE, DRX_TUNE1a_SUB, drxtune1a, LEN_DRX_TUNE1a);
	DW1000_writeBytes(DRX_TUNE, DRX_TUNE1b_SUB, drxtune1b, LEN_DRX_TUNE1b);
	DW1000_writeBytes(DRX_TUNE, DRX_TUNE2_SUB, drxtune2, LEN_DRX_TUNE2);
	DW1000_writeBytes(DRX_TUNE, DRX_TUNE4H_SUB, drxtune4H, LEN_DRX_TUNE4H);
	DW1000_writeBytes(LDE_IF, LDE_CFG1_SUB, ldecfg1, LEN_LDE_CFG1);
	DW1000_writeBytes(LDE_IF, LDE_CFG2_SUB, ldecfg2, LEN_LDE_CFG2);
	DW1000_writeBytes(LDE_IF, LDE_REPC_SUB, lderepc, LEN_LDE_REPC);
	DW1000_writeBytes(TX_POWER, NO_SUB, txpower, LEN_TX_POWER);
	DW1000_writeBytes(RF_CONF, RF_RXCTRLH_SUB, rfrxctrlh, LEN_RF_RXCTRLH);
	DW1000_writeBytes(RF_CONF, RF_TXCTRL_SUB, rftxctrl, LEN_RF_TXCTRL);
	DW1000_writeBytes(TX_CAL, TC_PGDELAY_SUB, tcpgdelay, LEN_TC_PGDELAY);
	DW1000_writeBytes(FS_CTRL, FS_PLLTUNE_SUB, fsplltune, LEN_FS_PLLTUNE);
	DW1000_writeBytes(FS_CTRL, FS_PLLCFG_SUB, fspllcfg, LEN_FS_PLLCFG);
	DW1000_writeBytes(FS_CTRL, FS_XTALT_SUB, fsxtalt, LEN_FS_XTALT);
}



void DW1000_newConfiguration() {
	DW1000_idle();
	DW1000_readNetworkIdAndDeviceAddress();
	DW1000_readSystemConfigurationRegister();
	DW1000_readChannelControlRegister();
	DW1000_readTransmitFrameControlRegister();
	DW1000_readSystemEventMaskRegister();
}

void DW1000_commitConfiguration() {
	// write all configurations back to device
	DW1000_writeNetworkIdAndDeviceAddress();
	DW1000_writeSystemConfigurationRegister();
	DW1000_writeChannelControlRegister();
	DW1000_writeTransmitFrameControlRegister();
	DW1000_writeSystemEventMaskRegister();
	// tune according to configuration
	DW1000_tune();
	// TODO clean up code + antenna delay/calibration API
	// TODO setter + check not larger two bytes integer
	uint8_t antennaDelayBytes[LEN_STAMP];
	DW1000_writeValueToBytes(antennaDelayBytes, 16384, LEN_STAMP);
	//DW1000_setTimestamp(antennaDelayBytes);
	DW1000_writeBytes(TX_ANTD, NO_SUB, antennaDelayBytes, LEN_TX_ANTD);
	DW1000_writeBytes(LDE_IF, LDE_RXANTD_SUB, antennaDelayBytes, LEN_LDE_RXANTD);
}


void DW1000_setDefaults() {
	if(DWM1000_str.deviceMode == TX_MODE) {
		
	} else if(DWM1000_str.deviceMode == RX_MODE) {
		
	} else if(DWM1000_str.deviceMode == IDLE_MODE) {
		DW1000_useExtendedFrameLength(FALSE);
		DW1000_useSmartPower(FALSE);
		DW1000_suppressFrameCheck(FALSE);
		//for global frame filtering
		DW1000_setFrameFilter(FALSE);
		/* old defaults with active frame filter - better set filter in every script where you really need it
		setFrameFilter(TRUE);
		//for data frame (poll, poll_ack, range, range report, range failed) filtering
		setFrameFilterAllowData(TRUE);
		//for reserved (blink) frame filtering
		setFrameFilterAllowReserved(TRUE);
		//setFrameFilterAllowMAC(TRUE);
		//setFrameFilterAllowBeacon(TRUE);
		//setFrameFilterAllowAcknowledgement(TRUE);
		*/
		DW1000_interruptOnSent(TRUE);
		DW1000_interruptOnReceived(TRUE);
		DW1000_interruptOnReceiveFailed(TRUE);
		DW1000_interruptOnReceiveTimestampAvailable(FALSE);
		DW1000_interruptOnAutomaticAcknowledgeTrigger(TRUE);
		DW1000_setReceiverAutoReenable(TRUE);
		// default mode when powering up the chip
		// still explicitly selected for later tuning
		DW1000_enableMode(MODE_LONGDATA_RANGE_LOWPOWER);
	}
}

void DW1000_clearTransmitStatus() {
	// clear latched TX bits
	DW1000_setBit(DWM1000_str.sysstatus, LEN_SYS_STATUS, TXFRB_BIT, TRUE);
	DW1000_setBit(DWM1000_str.sysstatus, LEN_SYS_STATUS, TXPRS_BIT, TRUE);
	DW1000_setBit(DWM1000_str.sysstatus, LEN_SYS_STATUS, TXPHS_BIT, TRUE);
	DW1000_setBit(DWM1000_str.sysstatus, LEN_SYS_STATUS, TXFRS_BIT, TRUE);
	DW1000_writeBytes(SYS_STATUS, NO_SUB, DWM1000_str.sysstatus, LEN_SYS_STATUS);
}

void DW1000_newTransmit() {
	DW1000_idle();
	memset(DWM1000_str.sysctrl, 0, LEN_SYS_CTRL);
	DW1000_clearTransmitStatus();
	DWM1000_str.deviceMode = TX_MODE;
}

void DW1000_startReceive() {
	DW1000_setBit(DWM1000_str.sysctrl, LEN_SYS_CTRL, SFCST_BIT, !DWM1000_str.frameCheck);
	DW1000_setBit(DWM1000_str.sysctrl, LEN_SYS_CTRL, RXENAB_BIT, TRUE);
	DW1000_writeBytes(SYS_CTRL, NO_SUB, DWM1000_str.sysctrl, LEN_SYS_CTRL);
}

void DW1000_startTransmit() {
	DW1000_writeTransmitFrameControlRegister();
	DW1000_setBit(DWM1000_str.sysctrl, LEN_SYS_CTRL, SFCST_BIT, !DWM1000_str.frameCheck);
	DW1000_setBit(DWM1000_str.sysctrl, LEN_SYS_CTRL, TXSTRT_BIT, TRUE);
	DW1000_writeBytes(SYS_CTRL, NO_SUB, DWM1000_str.sysctrl, LEN_SYS_CTRL);
	if(DWM1000_str.permanentReceive) {
		memset(DWM1000_str.sysctrl, 0, LEN_SYS_CTRL);
		DWM1000_str.deviceMode = RX_MODE;
		DW1000_startReceive();
		} else {
		DWM1000_str.deviceMode = IDLE_MODE;
	}
}

void DW1000_setData(uint8_t * data, uint16_t n) {
	if(DWM1000_str.frameCheck) {
		n += 2; // two bytes CRC-16
	}
	if(n > LEN_EXT_UWB_FRAMES) {
		return; // TODO proper error handling: frame/buffer size
	}
	if(n > LEN_UWB_FRAMES && !DWM1000_str.extendedFrameLength) {
		return; // TODO proper error handling: frame/buffer size
	}
	// transmit data and length
	DW1000_writeBytes(TX_BUFFER, NO_SUB, data, n);
	DWM1000_str.txfctrl[0] = (uint8_t)(n & 0xFF); // 1 byte (regular length + 1 bit)
	DWM1000_str.txfctrl[1] &= 0xE0;
	DWM1000_str.txfctrl[1] |= (uint8_t)((n >> 8) & 0x03);  // 2 added bits if extended length
}

/* ###########################################################################
 * #### Pretty printed device information ####################################
 * ######################################################################### */


void DW1000_getPrintableDeviceIdentifier(char msgBuffer[]) {
	uint8_t data[LEN_DEV_ID];
	DW1000_readBytes(DEV_ID, NO_SUB, data, LEN_DEV_ID);
	sprintf(msgBuffer, "%02X - model: %d, version: %d, revision: %d",
					(uint16_t)((data[3] << 8) | data[2]), data[1], (data[0] >> 4) & 0x0F, data[0] & 0x0F);
}

void DW1000_getPrintableExtendedUniqueIdentifier(char msgBuffer[]) {
	uint8_t data[LEN_EUI];
	DW1000_readBytes(EUI, NO_SUB, data, LEN_EUI);
	sprintf(msgBuffer, "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
					data[7], data[6], data[5], data[4], data[3], data[2], data[1], data[0]);
}

void DW1000_getPrintableNetworkIdAndShortAddress(char msgBuffer[]) {
	uint8_t data[LEN_PANADR];
	DW1000_readBytes(PANADR, NO_SUB, data, LEN_PANADR);
	sprintf(msgBuffer, "PAN: %02X, Short Address: %02X",
					(uint16_t)((data[3] << 8) | data[2]), (uint16_t)((data[1] << 8) | data[0]));
}

void DW1000_getPrintableDeviceMode(char msgBuffer[]) {
	// data not read from device! data is from class
	// TODO
	uint8_t prf;
	uint16_t plen;
	uint16_t dr;
	uint8_t ch;
	uint8_t pcode;
	if(DWM1000_str.pulseFrequency == TX_PULSE_FREQ_16MHZ) {
		prf = 16;
	} else if(DWM1000_str.pulseFrequency == TX_PULSE_FREQ_64MHZ) {
		prf = 64;
	} else {
		prf = 0; // error
	}
	if(DWM1000_str.preambleLength == TX_PREAMBLE_LEN_64) {
		plen = 64;
	} else if(DWM1000_str.preambleLength == TX_PREAMBLE_LEN_128) {
		plen = 128;
	} else if(DWM1000_str.preambleLength == TX_PREAMBLE_LEN_256) {
		plen = 256;
	} else if(DWM1000_str.preambleLength == TX_PREAMBLE_LEN_512) {
		plen = 512;
	} else if(DWM1000_str.preambleLength == TX_PREAMBLE_LEN_1024) {
		plen = 1024;
	} else if(DWM1000_str.preambleLength == TX_PREAMBLE_LEN_1536) {
		plen = 1536;
	} else if(DWM1000_str.preambleLength == TX_PREAMBLE_LEN_2048) {
		plen = 2048;
	} else if(DWM1000_str.preambleLength == TX_PREAMBLE_LEN_4096) {
		plen = 4096;
	} else {
		plen = 0; // error
	}
	if(DWM1000_str.dataRate == TRX_RATE_110KBPS) {
		dr = 110;
	} else if(DWM1000_str.dataRate == TRX_RATE_850KBPS) {
		dr = 850;
	} else if(DWM1000_str.dataRate == TRX_RATE_6800KBPS) {
		dr = 6800;
	} else {
		dr = 0; // error
	}
	ch    = (uint8_t)DWM1000_str.channel;
	pcode = (uint8_t)DWM1000_str.preambleCode;
	sprintf(msgBuffer, "Data rate: %u kb/s, PRF: %u MHz, Preamble: %u symbols (code #%u), Channel: #%u", dr, prf, plen, pcode, ch);
}



void rs485_send(char a)
{
	
}

int main (void)
{
	init_onepos();
	spi_init_module();
	
	static usart_rs232_options_t BLE_SERIAL_OPTIONS = {
		.baudrate = USART_SERIAL_BAUDRATE,
		.charlength = USART_SERIAL_CHAR_LENGTH,
		.paritytype = USART_SERIAL_PARITY,
		.stopbits = USART_SERIAL_STOP_BIT
	};
	usart_serial_init(USART_BLE, &BLE_SERIAL_OPTIONS);
	
	static usart_rs232_options_t RS485_SERIAL_OPTIONS = {
		.baudrate = USART_SERIAL_BAUDRATE,
		.charlength = USART_SERIAL_CHAR_LENGTH,
		.paritytype = USART_SERIAL_PARITY,
		.stopbits = USART_SERIAL_STOP_BIT
	};
	usart_serial_init(USART_RS485, &RS485_SERIAL_OPTIONS);
	
	
	
	
	init_animation();
	
	
	// DEBUG monitoring
	printf("### DW1000-arduino-sender-test ###\n");
	// initialize the driver
	printf("DW1000 initialized ...\n");
	// general configuration
	DW1000_newConfiguration();
	DW1000_setDefaults();
	DW1000_setDeviceAddress(5);
	DW1000_setNetworkId(10);
	DW1000_enableMode(MODE_LONGDATA_RANGE_LOWPOWER);
	DW1000_commitConfiguration();
	printf("Committed configuration ...\n");
	
	// DEBUG chip info and registers pretty printed
	char msg[128];
	DW1000_getPrintableDeviceIdentifier(msg);
	printf("Device ID: "); 
	printf(msg);
	DW1000_getPrintableExtendedUniqueIdentifier(msg);
	printf("Unique ID: "); 
	printf(msg);
	DW1000_getPrintableNetworkIdAndShortAddress(msg);
	printf("Network ID & Device Address: "); 
	printf(msg);
	DW1000_getPrintableDeviceMode(msg);
	printf("Device mode: "); 
	printf(msg);
	
	// attach callback for (successfully) sent messages
	//DW1000_attachSentHandler(handleSent);
	// start a transmission
	
	uint8_t sentNum = 0;
	for (;;)
	{
		// transmit some data
		printf("Transmitting packet ... #"); 
		printf("%d\n",sentNum);
		DW1000_newTransmit();
		DW1000_setDefaults();
		char msg[200] = "Hello DW1000, it's #";
		DW1000_setData(msg,20);
		// delay sending the message for the given amount
		//DW1000Time deltaTime = DW1000Time(10, DW1000Time::MILLISECONDS);
		//DW1000.setDelay(deltaTime);
		DW1000_startTransmit();
		//delaySent = millis();
		delay_ms(500);
	}
	
	
		
	for(;;) //DWM1000 test
	{
		
	}
	
	uint8_t data_send[1] = {0x00};
	uint8_t data_receive[LEN_DEV_ID];
	
	for (;;) //spi test
	{
		DW1000_readBytes(DEV_ID, NO_SUB, data_receive, LEN_DEV_ID);
		delay_ms(1000);
		printf("%#02X %#02X %#02X %#02X \n", data_receive[3], data_receive[2], data_receive[1], data_receive[0]);
		led1_toogle();
	}
	
	
	for(;;) //USB-USART_BLE bridge
	{
		if (usb_is_rx_ready())
		{
			usart_putchar(USART_BLE, usb_getchar());
			led1_toogle();
		}
		if (usart_rx_is_complete(USART_BLE))
		{
			usb_putchar(usart_getchar(USART_BLE));
			led2_toogle();
		}
		
	}
	
	for (;;) //Basic USB printf example
	{
		printf("hello!\n");
		delay_ms(500);
	}
}
