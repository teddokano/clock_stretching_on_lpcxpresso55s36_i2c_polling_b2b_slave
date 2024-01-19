/*
 * Copyright 2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*  Standard C Included Files */

#include "setting.h"

#include <stdio.h>
#include <string.h>
#include "pin_mux.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_i2c.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define EXAMPLE_I2C_SLAVE_BASE    (I2C2_BASE)
#define I2C_SLAVE_CLOCK_FREQUENCY (12000000)
#define EXAMPLE_I2C_SLAVE ((I2C_Type *)EXAMPLE_I2C_SLAVE_BASE)

//#define I2C_MASTER_SLAVE_ADDR_7BIT 0x7EU
#define I2C_DATA_LENGTH            128U
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

uint8_t g_slave_buff[I2C_DATA_LENGTH];

/*******************************************************************************
 * Code

 ******************************************************************************/

void clock_stretch( unsigned int wait );
void init_I2C_target( i2c_slave_config_t *scp );
int handling_I2C_target( i2c_slave_config_t *scp );
status_t target_virtual_transfer( I2C_Type *base );

void clock_stretch( unsigned int wait )
{
    for ( volatile int i = 0; i < wait; i++ )
    	;
}

int main( void )
{
	i2c_slave_config_t slaveConfig;

	init_I2C_target( &slaveConfig );

    while (1)
    	handling_I2C_target( &slaveConfig );
}

void init_I2C_target( i2c_slave_config_t *scp )
{
    /* attach 12 MHz clock to FLEXCOMM0 (debug console) */
    CLOCK_SetClkDiv(kCLOCK_DivFlexcom0Clk, 0u, false);
    CLOCK_SetClkDiv(kCLOCK_DivFlexcom0Clk, 1u, true);
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

    /* attach 12 MHz clock to FLEXCOMM2 (I2C master) */
    CLOCK_SetClkDiv(kCLOCK_DivFlexcom2Clk, 0u, false);
    CLOCK_SetClkDiv(kCLOCK_DivFlexcom2Clk, 1u, true);
    CLOCK_AttachClk(kFRO12M_to_FLEXCOMM2);

    /* reset FLEXCOMM for I2C */
    RESET_PeripheralReset(kFC2_RST_SHIFT_RSTn);

    BOARD_InitPins();
    BOARD_BootClockPLL150M();
    BOARD_InitDebugConsole();

    /* Set up i2c slave first*/
    I2C_SlaveGetDefaultConfig( scp );

    /* Change the slave address */
    scp->address0.address = I2C_MASTER_SLAVE_ADDR_7BIT;

    /* Initialize the I2C slave peripheral */
    I2C_SlaveInit(EXAMPLE_I2C_SLAVE, scp, I2C_SLAVE_CLOCK_FREQUENCY);

    PRINTF("\r\nI2C board2board polling example -- Slave transfer.\r\n\r\n");
}

int handling_I2C_target( i2c_slave_config_t *scp )
{
    status_t reVal = kStatus_Fail;

    /* Start accepting I2C transfers on the I2C slave peripheral */
    reVal = target_virtual_transfer( EXAMPLE_I2C_SLAVE );

    if (reVal != kStatus_Success)
        return -1;
}

status_t target_virtual_transfer( I2C_Type *base )
{
    uint32_t	stat;
    uint8_t		val		= 0xAA;

    I2C_SlaveEnable(EXAMPLE_I2C_SLAVE, true);

    if ( (uint32_t)kStatus_I2C_Timeout == (stat = I2C_SlavePollPending(base)) )
        return kStatus_I2C_Timeout;

    switch ( ((stat & I2C_STAT_SLVSTATE_MASK) >> I2C_STAT_SLVSTATE_SHIFT) )
	{
		case I2C_STAT_SLVST_ADDR:
	        clock_stretch( ADDRESS_ACK_STRETCH );
	        base->SLVCTL = I2C_SLVCTL_SLVCONTINUE_MASK;
	        break;
		case I2C_STAT_SLVST_RX:
    		val = (uint8_t)base->SLVDAT;
            clock_stretch( WRITE_DATA_ACK_STRETCH );
    		base->SLVCTL = I2C_SLVCTL_SLVCONTINUE_MASK;
			break;
		case I2C_STAT_SLVST_TX:
            base->SLVDAT = I2C_SLVDAT_DATA(val);
            clock_stretch( READ_DATA_ACK_STRETCH );
            base->SLVCTL = I2C_SLVCTL_SLVCONTINUE_MASK;
			break;
	}

    return kStatus_Success;
}
