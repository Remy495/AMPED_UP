
#ifndef _SPI_INTERFACE_
#define _SPI_INTERFACE_

#include "imxrt.h"

#include "PinConfigGroup.h"
#include "BinaryUtil.hxx"

namespace AmpedUp
{
    class SpiInterface
    {
    public:

        using InterruptHandler = void(*)();

        static void enable()
        {
            // Configure clock
            CCM_CBCMR = (CCM_CBCMR & ~(CCM_CBCMR_LPSPI_PODF_MASK | CCM_CBCMR_LPSPI_CLK_SEL_MASK)) | CCM_CBCMR_LPSPI_PODF(2) | CCM_CBCMR_LPSPI_CLK_SEL(1); 
            CCM_CCGR1 |= CCM_CCGR1_LPSPI4(CCM_CCGR_ON);

            spiPins.enableAllPins();

            // Reset SPI peripheral
            LPSPI4_CR = LPSPI_CR_RST;
            LPSPI4_CR = 0;

            // Set RX watermark to 0 and TX watermark to half the size of the TX FIFO
            LPSPI4_FCR = LPSPI_FCR_TXWATER(FIFO_SIZE / 2) | LPSPI_FCR_RXWATER(0);

            // Use master mode, enable delayed sampling
            LPSPI4_CFGR1 = LPSPI_CFGR1_MASTER | LPSPI_CFGR1_SAMPLE;
            // Set SCK to run at 4 MHz
            setClockFreq(4000000);
            // Configure SPI peripheral for 32 bit words, MSB first, mode 0
            LPSPI4_TCR = IDLE_BUS_CONFIGURATION;

            // Enable the peripheral
            LPSPI4_CR = LPSPI_CR_MEN;
        }

        static void enableInterrupts()
        {
            NVIC_ENABLE_IRQ(IRQ_LPSPI4);
        }

        static void disableInterrupts()
        {
            NVIC_DISABLE_IRQ(IRQ_LPSPI4);
        }

        static void setInterruptHandler(InterruptHandler handler)
        {
            attachInterruptVector(IRQ_LPSPI4, handler);
        }

        static void beginTransaction()
        {
            LPSPI4_TCR = RUNNING_BUS_CONFIGURATION;
        }

        static void endTransaction()
        {
            LPSPI4_TCR = IDLE_BUS_CONFIGURATION;
        }

        static void sendWord(BinaryUtil::uword_t word)
        {
            LPSPI4_TDR = word;
        }

        static BinaryUtil::uword_t recieveWord()
        {
            return LPSPI4_RDR;
        }

        static bool canTransmitData()
        {
            return getTxCount() < FIFO_SIZE;
        }

        static bool canReceiveData()
        {
            return getRxCount() > 0;
        }

    private:

        using SpiPinGroup = PinConfigGroup<4>;
        static constexpr PadDriverConfiguration SPI_PAD_CONFIG{false, InputPullupMode::NONE, OutputImpedance::DRIVE_21_OHMS, DriveSpeed::HIGH_150MHZ};

        static inline SpiPinGroup spiPins = 
        {
            PinConfiguration(IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_01, 3U | 0x10U, IOMUXC_SW_PAD_CTL_PAD_GPIO_B0_01, SPI_PAD_CONFIG, IOMUXC_LPSPI4_SDI_SELECT_INPUT, 0), // MISO: 12
            PinConfiguration(IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_02, 3U | 0x10U, IOMUXC_SW_PAD_CTL_PAD_GPIO_B0_02, SPI_PAD_CONFIG, IOMUXC_LPSPI4_SDO_SELECT_INPUT, 0), // MOSI: 11
            PinConfiguration(IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_03, 3U | 0x10U, IOMUXC_SW_PAD_CTL_PAD_GPIO_B0_03, SPI_PAD_CONFIG, IOMUXC_LPSPI4_SCK_SELECT_INPUT, 0), // SCK: 13
            PinConfiguration(IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_00, 3U | 0x10U, IOMUXC_SW_PAD_CTL_PAD_GPIO_B0_00, SPI_PAD_CONFIG, IOMUXC_LPSPI4_PCS0_SELECT_INPUT, 0)  // SS: 10
        };

        static constexpr uint8_t FIFO_SIZE = 16;
        // Byteswap flag is needed because for some reason the Teensy puts each word onto the wire in little endian order by default?
        static constexpr uint32_t IDLE_BUS_CONFIGURATION = LPSPI_TCR_FRAMESZ(31) | LPSPI_TCR_BYSW;
        static constexpr uint32_t RUNNING_BUS_CONFIGURATION = IDLE_BUS_CONFIGURATION | LPSPI_TCR_CONT;

        static uint8_t getTxCount()
        {
            return LPSPI4_FSR & 0x1F;
        }

        static uint8_t getRxCount()
        {
            return (LPSPI4_FSR >> 16) & 0x1F;
        }

        static void setClockFreq(uint32_t hz)
        {
            static const uint32_t clk_sel[4] =
            {
                664615384,  // PLL3 PFD1
                720000000,  // PLL3 PFD0
                528000000,  // PLL2
                396000000   // PLL2 PFD2
            };	

			uint32_t cbcmr = CCM_CBCMR;
			uint32_t clkhz = clk_sel[(cbcmr >> 4) & 0x03] / (((cbcmr >> 26 ) & 0x07 ) + 1);  // LPSPI peripheral clock
			
			uint32_t d, div;		
			d = hz ? clkhz/hz : clkhz;

			if (d && clkhz/d > hz) d++;
			if (d > 257) d= 257;  // max div
			if (d > 2) {
				div = d-2;
			} else {
				div =0;
			}

            LPSPI4_CCR = LPSPI_CCR_SCKDIV(div) | LPSPI_CCR_DBT(div/2) | LPSPI_CCR_PCSSCK(div/2);
        }

    };
}

#endif