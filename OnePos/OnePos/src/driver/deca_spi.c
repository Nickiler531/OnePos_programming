/*! ----------------------------------------------------------------------------
 * @file	deca_spi.c
 * @brief	SPI access functions
 *
 * @attention
 *
 * Copyright 2013 (c) DecaWave Ltd, Dublin, Ireland.
 *
 * All rights reserved.
 *
 * @author DecaWave
 */
#include <string.h>

#include "../include/deca_spi.h"
#include "../include/deca_device_api.h"

/*! ------------------------------------------------------------------------------------------------------------------
 * Function: openspi()
 *
 * Low level abstract function to open and initialise access to the SPI device.
 * returns 0 for success, or -1 for error
 */
int openspi(void)
{
	spi_device_conf.id = UWB_CS;
    spi_master_init(&SPIC);
    spi_master_setup_device(&SPIC, &spi_device_conf, SPI_MODE_0, 500000, 0);
    spi_enable(&SPIC);

	return 0;

} // end openspi()

void fast_spi(void)
{
	spi_master_setup_device(&SPIC, &spi_device_conf, SPI_MODE_0, 15000000, 0);
}

/*! ------------------------------------------------------------------------------------------------------------------
 * Function: closespi()
 *
 * Low level abstract function to close the the SPI device.
 * returns 0 for success, or -1 for error
 */
int closespi(void)
{
	spi_disable(&SPIC);


	return 0;

} // end closespi()

/*! ------------------------------------------------------------------------------------------------------------------
 * Function: writetospi()
 *
 * Low level abstract function to write to the SPI
 * Takes two separate byte buffers for write header and write data
 * returns 0 for success, or -1 for error
 */
#pragma GCC optimize ("O3")
int writetospi(uint16 headerLength, const uint8 *headerBuffer, uint32 bodylength, const uint8 *bodyBuffer)
{

	int i=0;

    decaIrqStatus_t  stat ;

    stat = decamutexon() ;

    spi_select_device(&SPIC, &spi_device_conf);
    spi_write_packet(&SPIC, headerBuffer, headerLength);
    spi_write_packet(&SPIC, bodyBuffer, bodylength);
    spi_deselect_device(&SPIC, &spi_device_conf);

    decamutexoff(stat) ;

    return 0;
} // end writetospi()


/*! ------------------------------------------------------------------------------------------------------------------
 * Function: readfromspi()
 *
 * Low level abstract function to read from the SPI
 * Takes two separate byte buffers for write header and read data
 * returns the offset into read buffer where first byte of read data may be found,
 * or returns -1 if there was an error
 */
#pragma GCC optimize ("O3")
int readfromspi(uint16 headerLength, const uint8 *headerBuffer, uint32 readlength, uint8 *readBuffer)
{

	int i=0;

    decaIrqStatus_t  stat ;

    stat = decamutexon() ;

    spi_select_device(&SPIC, &spi_device_conf);
    spi_write_packet(&SPIC, headerBuffer, headerLength);
    spi_read_packet(&SPIC, readBuffer, readlength);
    spi_deselect_device(&SPIC, &spi_device_conf);

    decamutexoff(stat) ;

    return 0;
} // end readfromspi()
