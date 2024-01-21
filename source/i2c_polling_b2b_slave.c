#include "setting.h"

#include <stdio.h>
#include <string.h>
#include "pin_mux.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_i2c.h"

#define EXAMPLE_I2C_SLAVE_BASE		(I2C2_BASE)
#define I2C_SLAVE_CLOCK_FREQUENCY	(12000000)
#define EXAMPLE_I2C_SLAVE			((I2C_Type *)EXAMPLE_I2C_SLAVE_BASE)

#define MEM_LENGTH					256

uint8_t		g_mem[ MEM_LENGTH ];

uint32_t	g_stretch_address		= ADDRESS_ACK_STRETCH;
uint32_t	g_stretch_write			= WRITE_DATA_ACK_STRETCH;
uint32_t	g_stretch_read			= READ_DATA_ACK_STRETCH;

uint32_t	*g_stretch_p[]	= { &g_stretch_address, &g_stretch_write, &g_stretch_read };

volatile uint32_t	g_systickCounter;


void		clock_stretch( uint32_t wait );
void		init_I2C_target( i2c_slave_config_t *scp );
int			handling_I2C_target( i2c_slave_config_t *scp );
status_t	target_mem_transfer( I2C_Type *base );
status_t	target_virtual_transfer( I2C_Type *base );

void		address_0_behavior( I2C_Type *base, uint8_t targ_stat );
void		address_1_behavior( I2C_Type *base, uint8_t targ_stat );

void		SysTick_Handler( void );
void		SysTick_DelayTicks( uint32_t n );


void clock_stretch( uint32_t wait )
{
	SysTick_DelayTicks( wait );
}

int main( void )
{
	i2c_slave_config_t slaveConfig;

	init_I2C_target( &slaveConfig );

    while (1)
    	handling_I2C_target( &slaveConfig );
}

int handling_I2C_target( i2c_slave_config_t *scp )
{
    status_t reVal = kStatus_Fail;

    reVal = target_mem_transfer( EXAMPLE_I2C_SLAVE );

    if (reVal != kStatus_Success)
        return -1;
}

status_t target_mem_transfer( I2C_Type *base )
{
    uint32_t	stat;
    uint8_t		address_idx;
    uint8_t		targ_stat;

    I2C_SlaveEnable(EXAMPLE_I2C_SLAVE, true);

    if ( (uint32_t)kStatus_I2C_Timeout == (stat = I2C_SlavePollPending(base)) )
        return kStatus_I2C_Timeout;

    address_idx	= (stat & I2C_STAT_SLVIDX_MASK) >> I2C_STAT_SLVIDX_SHIFT;
    targ_stat	= (stat & I2C_STAT_SLVSTATE_MASK) >> I2C_STAT_SLVSTATE_SHIFT;

    switch ( address_idx )
    {
    	case 0:
    		address_0_behavior( base, targ_stat );
    		break;

    	case 1:
    		address_1_behavior( base, targ_stat );
    		break;

    	default:
    		break;
    }

    base->SLVCTL = I2C_SLVCTL_SLVCONTINUE_MASK;

    return kStatus_Success;
}

void address_0_behavior( I2C_Type *base, uint8_t targ_stat )
{
    static uint8_t	ptr;
    static bool		first_byte;

	switch ( targ_stat )
	{
		case I2C_STAT_SLVST_ADDR:
			first_byte	= true;

			clock_stretch( g_stretch_address );
			break;
		case I2C_STAT_SLVST_RX:
			if ( first_byte ) {
				ptr	= (uint8_t)base->SLVDAT;
				first_byte	= false;
			}
			else {
				*(g_mem + ptr++) = (uint8_t)base->SLVDAT;
			}

			clock_stretch( g_stretch_write );
			break;
		case I2C_STAT_SLVST_TX:
			base->SLVDAT = I2C_SLVDAT_DATA( *(g_mem + ptr++) );

			clock_stretch( g_stretch_read );
			break;
	}
}

void address_1_behavior( I2C_Type *base, uint8_t targ_stat )
{
    static uint32_t		*targ_param_p;
    static uint8_t		ptr;
    static uint8_t		idx;
    static bool			first_byte;

	switch ( targ_stat )
	{
		case I2C_STAT_SLVST_ADDR:
			first_byte	= true;
			break;
		case I2C_STAT_SLVST_RX:
			if ( first_byte ) {
				idx	= (uint32_t)base->SLVDAT;

				targ_param_p	= g_stretch_p[ (2 < idx) ? 0 : idx ];
				*targ_param_p	= 0;
				ptr				= 0;
				first_byte		= false;
			}
			else {
				*targ_param_p	|= (uint32_t)((uint8_t)base->SLVDAT) << (ptr++ * 8);
			}

			break;
		case I2C_STAT_SLVST_TX:
			base->SLVDAT = I2C_SLVDAT_DATA( (uint8_t)(*targ_param_p >> (ptr++ * 8)) );

			break;
	}
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

    SysTick_Config( SystemCoreClock / 1000000 );

    /* Set up i2c slave first*/
    I2C_SlaveGetDefaultConfig( scp );

    /* Change the slave address */
    scp->address0.address = I2C_MASTER_SLAVE_ADDR_7BIT;
    scp->address1.address = I2C_SUBADDR0_7BIT;

    /* Initialize the I2C slave peripheral */
    I2C_SlaveInit(EXAMPLE_I2C_SLAVE, scp, I2C_SLAVE_CLOCK_FREQUENCY);

    PRINTF("\r\nI2C board2board polling example -- Slave transfer.\r\n\r\n");
}

void SysTick_Handler( void )
{
    if (g_systickCounter != 0U)
        g_systickCounter--;
}

void SysTick_DelayTicks( uint32_t n )
{
    g_systickCounter = n;
    while (g_systickCounter != 0U)
    	;
}



