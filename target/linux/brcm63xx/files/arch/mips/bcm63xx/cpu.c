/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2008 Maxime Bizon <mbizon@freebox.fr>
 *		 2009 Florian Fainelli <florian@openwrt.org>
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cpu.h>
#include <bcm63xx_cpu.h>
#include <bcm63xx_regs.h>
#include <bcm63xx_io.h>
#include <bcm63xx_irq.h>

const unsigned long *bcm63xx_regs_base;
EXPORT_SYMBOL(bcm63xx_regs_base);

const int *bcm63xx_irqs;
EXPORT_SYMBOL(bcm63xx_irqs);

const unsigned long *bcm63xx_regs_spi;
EXPORT_SYMBOL(bcm63xx_regs_spi);

static u16 bcm63xx_cpu_id;
static u16 bcm63xx_cpu_rev;
static unsigned int bcm63xx_cpu_freq;
static unsigned int bcm63xx_memory_size;

/*
 * 6338 register sets and irqs
 */

static const unsigned long bcm96338_regs_base[] = {
	[RSET_DSL_LMEM]		= BCM_6338_DSL_LMEM_BASE,
	[RSET_PERF]		= BCM_6338_PERF_BASE,
	[RSET_TIMER]		= BCM_6338_TIMER_BASE,
	[RSET_WDT]		= BCM_6338_WDT_BASE,
	[RSET_UART0]		= BCM_6338_UART0_BASE,
	[RSET_GPIO]		= BCM_6338_GPIO_BASE,
	[RSET_SPI]		= BCM_6338_SPI_BASE,
	[RSET_OHCI0]		= BCM_6338_OHCI0_BASE,
	[RSET_OHCI_PRIV]	= BCM_6338_OHCI_PRIV_BASE,
	[RSET_USBH_PRIV]	= BCM_6338_USBH_PRIV_BASE,
	[RSET_UDC0]		= BCM_6338_UDC0_BASE,
	[RSET_MPI]		= BCM_6338_MPI_BASE,
	[RSET_PCMCIA]		= BCM_6338_PCMCIA_BASE,
	[RSET_SDRAM]		= BCM_6338_SDRAM_BASE,
	[RSET_DSL]		= BCM_6338_DSL_BASE,
	[RSET_ENET0]		= BCM_6338_ENET0_BASE,
	[RSET_ENET1]		= BCM_6338_ENET1_BASE,
	[RSET_ENETDMA]		= BCM_6338_ENETDMA_BASE,
	[RSET_MEMC]		= BCM_6338_MEMC_BASE,
	[RSET_DDR]		= BCM_6338_DDR_BASE,
};

static const int bcm96338_irqs[] = {
	[IRQ_TIMER]		= BCM_6338_TIMER_IRQ,
	[IRQ_SPI]		= BCM_6338_SPI_IRQ,
	[IRQ_UART0]		= BCM_6338_UART0_IRQ,
	[IRQ_DSL]		= BCM_6338_DSL_IRQ,
	[IRQ_UDC0]		= BCM_6338_UDC0_IRQ,
	[IRQ_ENET0]		= BCM_6338_ENET0_IRQ,
	[IRQ_ENET_PHY]		= BCM_6338_ENET_PHY_IRQ,
	[IRQ_ENET0_RXDMA]	= BCM_6338_ENET0_RXDMA_IRQ,
	[IRQ_ENET0_TXDMA]	= BCM_6338_ENET0_TXDMA_IRQ,
};

static const unsigned long bcm96338_regs_spi[] = {
	[SPI_CMD]		= SPI_BCM_6338_SPI_CMD,
	[SPI_INT_STATUS]	= SPI_BCM_6338_SPI_INT_STATUS,
	[SPI_INT_MASK_ST]	= SPI_BCM_6338_SPI_MASK_INT_ST,
	[SPI_INT_MASK]		= SPI_BCM_6338_SPI_INT_MASK,
	[SPI_ST]		= SPI_BCM_6338_SPI_ST,
	[SPI_CLK_CFG]		= SPI_BCM_6338_SPI_CLK_CFG,
	[SPI_FILL_BYTE]		= SPI_BCM_6338_SPI_FILL_BYTE,
	[SPI_MSG_TAIL]		= SPI_BCM_6338_SPI_MSG_TAIL,
	[SPI_RX_TAIL]		= SPI_BCM_6338_SPI_RX_TAIL,
	[SPI_MSG_CTL]		= SPI_BCM_6338_SPI_MSG_CTL,
	[SPI_MSG_DATA]		= SPI_BCM_6338_SPI_MSG_DATA,
	[SPI_RX_DATA]		= SPI_BCM_6338_SPI_RX_DATA,
};

/*
 * 6345 register sets and irqs
 */

static const unsigned long bcm96345_regs_base[] = {
	[RSET_DSL_LMEM]		= BCM_6345_DSL_LMEM_BASE,
	[RSET_PERF]		= BCM_6345_PERF_BASE,
	[RSET_TIMER]		= BCM_6345_TIMER_BASE,
	[RSET_WDT]		= BCM_6345_WDT_BASE,
	[RSET_UART0]		= BCM_6345_UART0_BASE,
	[RSET_GPIO]		= BCM_6345_GPIO_BASE,
	[RSET_SPI]		= BCM_6345_SPI_BASE,
	[RSET_UDC0]		= BCM_6345_UDC0_BASE,
	[RSET_OHCI0]		= BCM_6345_OHCI0_BASE,
	[RSET_OHCI_PRIV]	= BCM_6345_OHCI_PRIV_BASE,
	[RSET_USBH_PRIV]	= BCM_6345_USBH_PRIV_BASE,
	[RSET_MPI]		= BCM_6345_MPI_BASE,
	[RSET_PCMCIA]		= BCM_6345_PCMCIA_BASE,
	[RSET_DSL]		= BCM_6345_DSL_BASE,
	[RSET_ENET0]		= BCM_6345_ENET0_BASE,
	[RSET_ENET1]		= BCM_6345_ENET1_BASE,
	[RSET_ENETDMA]		= BCM_6345_ENETDMA_BASE,
	[RSET_EHCI0]		= BCM_6345_EHCI0_BASE,
	[RSET_SDRAM]		= BCM_6345_SDRAM_BASE,
	[RSET_MEMC]		= BCM_6345_MEMC_BASE,
	[RSET_DDR]		= BCM_6345_DDR_BASE,
};

static const int bcm96345_irqs[] = {
	[IRQ_TIMER]		= BCM_6345_TIMER_IRQ,
	[IRQ_UART0]		= BCM_6345_UART0_IRQ,
	[IRQ_DSL]		= BCM_6345_DSL_IRQ,
	[IRQ_UDC0]		= BCM_6345_UDC0_IRQ,
	[IRQ_ENET0]		= BCM_6345_ENET0_IRQ,
	[IRQ_ENET_PHY]		= BCM_6345_ENET_PHY_IRQ,
	[IRQ_ENET0_RXDMA]	= BCM_6345_ENET0_RXDMA_IRQ,
	[IRQ_ENET0_TXDMA]	= BCM_6345_ENET0_TXDMA_IRQ,
};

/*
 * 6348 register sets and irqs
 */
static const unsigned long bcm96348_regs_base[] = {
	[RSET_DSL_LMEM]		= BCM_6348_DSL_LMEM_BASE,
	[RSET_PERF]		= BCM_6348_PERF_BASE,
	[RSET_TIMER]		= BCM_6348_TIMER_BASE,
	[RSET_WDT]		= BCM_6348_WDT_BASE,
	[RSET_UART0]		= BCM_6348_UART0_BASE,
	[RSET_GPIO]		= BCM_6348_GPIO_BASE,
	[RSET_SPI]		= BCM_6348_SPI_BASE,
	[RSET_OHCI0]		= BCM_6348_OHCI0_BASE,
	[RSET_OHCI_PRIV]	= BCM_6348_OHCI_PRIV_BASE,
	[RSET_USBH_PRIV]	= BCM_6348_USBH_PRIV_BASE,
	[RSET_UDC0]		= BCM_6348_UDC0_BASE,
	[RSET_MPI]		= BCM_6348_MPI_BASE,
	[RSET_PCMCIA]		= BCM_6348_PCMCIA_BASE,
	[RSET_SDRAM]		= BCM_6348_SDRAM_BASE,
	[RSET_DSL]		= BCM_6348_DSL_BASE,
	[RSET_ENET0]		= BCM_6348_ENET0_BASE,
	[RSET_ENET1]		= BCM_6348_ENET1_BASE,
	[RSET_ENETDMA]		= BCM_6348_ENETDMA_BASE,
	[RSET_MEMC]		= BCM_6348_MEMC_BASE,
	[RSET_DDR]		= BCM_6348_DDR_BASE,
};

static const int bcm96348_irqs[] = {
	[IRQ_TIMER]		= BCM_6348_TIMER_IRQ,
	[IRQ_SPI]		= BCM_6348_SPI_IRQ,
	[IRQ_UART0]		= BCM_6348_UART0_IRQ,
	[IRQ_DSL]		= BCM_6348_DSL_IRQ,
	[IRQ_UDC0]		= BCM_6348_UDC0_IRQ,
	[IRQ_ENET0]		= BCM_6348_ENET0_IRQ,
	[IRQ_ENET1]		= BCM_6348_ENET1_IRQ,
	[IRQ_ENET_PHY]		= BCM_6348_ENET_PHY_IRQ,
	[IRQ_OHCI0]		= BCM_6348_OHCI0_IRQ,
	[IRQ_PCMCIA]		= BCM_6348_PCMCIA_IRQ,
	[IRQ_ENET0_RXDMA]	= BCM_6348_ENET0_RXDMA_IRQ,
	[IRQ_ENET0_TXDMA]	= BCM_6348_ENET0_TXDMA_IRQ,
	[IRQ_ENET1_RXDMA]	= BCM_6348_ENET1_RXDMA_IRQ,
	[IRQ_ENET1_TXDMA]	= BCM_6348_ENET1_TXDMA_IRQ,
	[IRQ_PCI]		= BCM_6348_PCI_IRQ,
};

static const unsigned long bcm96348_regs_spi[] = {
	[SPI_CMD]		= SPI_BCM_6348_SPI_CMD,
	[SPI_INT_STATUS]	= SPI_BCM_6348_SPI_INT_STATUS,
	[SPI_INT_MASK_ST]	= SPI_BCM_6348_SPI_MASK_INT_ST,
	[SPI_INT_MASK]		= SPI_BCM_6348_SPI_INT_MASK,
	[SPI_ST]		= SPI_BCM_6348_SPI_ST,
	[SPI_CLK_CFG]		= SPI_BCM_6348_SPI_CLK_CFG,
	[SPI_FILL_BYTE]		= SPI_BCM_6348_SPI_FILL_BYTE,
	[SPI_MSG_TAIL]		= SPI_BCM_6348_SPI_MSG_TAIL,
	[SPI_RX_TAIL]		= SPI_BCM_6348_SPI_RX_TAIL,
	[SPI_MSG_CTL]		= SPI_BCM_6348_SPI_MSG_CTL,
	[SPI_MSG_DATA]		= SPI_BCM_6348_SPI_MSG_DATA,
	[SPI_RX_DATA]		= SPI_BCM_6348_SPI_RX_DATA,
};

/*
 * 6358 register sets and irqs
 */
static const unsigned long bcm96358_regs_base[] = {
	[RSET_DSL_LMEM]		= BCM_6358_DSL_LMEM_BASE,
	[RSET_PERF]		= BCM_6358_PERF_BASE,
	[RSET_TIMER]		= BCM_6358_TIMER_BASE,
	[RSET_WDT]		= BCM_6358_WDT_BASE,
	[RSET_UART0]		= BCM_6358_UART0_BASE,
	[RSET_GPIO]		= BCM_6358_GPIO_BASE,
	[RSET_SPI]		= BCM_6358_SPI_BASE,
	[RSET_OHCI0]		= BCM_6358_OHCI0_BASE,
	[RSET_EHCI0]		= BCM_6358_EHCI0_BASE,
	[RSET_OHCI_PRIV]	= BCM_6358_OHCI_PRIV_BASE,
	[RSET_USBH_PRIV]	= BCM_6358_USBH_PRIV_BASE,
	[RSET_MPI]		= BCM_6358_MPI_BASE,
	[RSET_PCMCIA]		= BCM_6358_PCMCIA_BASE,
	[RSET_SDRAM]		= BCM_6358_SDRAM_BASE,
	[RSET_DSL]		= BCM_6358_DSL_BASE,
	[RSET_ENET0]		= BCM_6358_ENET0_BASE,
	[RSET_ENET1]		= BCM_6358_ENET1_BASE,
	[RSET_ENETDMA]		= BCM_6358_ENETDMA_BASE,
	[RSET_MEMC]		= BCM_6358_MEMC_BASE,
	[RSET_DDR]		= BCM_6358_DDR_BASE,
};

static const int bcm96358_irqs[] = {
	[IRQ_TIMER]		= BCM_6358_TIMER_IRQ,
	[IRQ_SPI]		= BCM_6358_SPI_IRQ,
	[IRQ_UART0]		= BCM_6358_UART0_IRQ,
	[IRQ_DSL]		= BCM_6358_DSL_IRQ,
	[IRQ_ENET0]		= BCM_6358_ENET0_IRQ,
	[IRQ_ENET1]		= BCM_6358_ENET1_IRQ,
	[IRQ_ENET_PHY]		= BCM_6358_ENET_PHY_IRQ,
	[IRQ_OHCI0]		= BCM_6358_OHCI0_IRQ,
	[IRQ_EHCI0]		= BCM_6358_EHCI0_IRQ,
	[IRQ_PCMCIA]		= BCM_6358_PCMCIA_IRQ,
	[IRQ_ENET0_RXDMA]	= BCM_6358_ENET0_RXDMA_IRQ,
	[IRQ_ENET0_TXDMA]	= BCM_6358_ENET0_TXDMA_IRQ,
	[IRQ_ENET1_RXDMA]	= BCM_6358_ENET1_RXDMA_IRQ,
	[IRQ_ENET1_TXDMA]	= BCM_6358_ENET1_TXDMA_IRQ,
	[IRQ_PCI]		= BCM_6358_PCI_IRQ,
};

static const unsigned long bcm96358_regs_spi[] = {
	[SPI_CMD]		= SPI_BCM_6358_SPI_CMD,
	[SPI_INT_STATUS]	= SPI_BCM_6358_SPI_INT_STATUS,
	[SPI_INT_MASK_ST]	= SPI_BCM_6358_SPI_MASK_INT_ST,
	[SPI_INT_MASK]		= SPI_BCM_6358_SPI_INT_MASK,
	[SPI_ST]		= SPI_BCM_6358_SPI_STATUS,
	[SPI_CLK_CFG]		= SPI_BCM_6358_SPI_CLK_CFG,
	[SPI_FILL_BYTE]		= SPI_BCM_6358_SPI_FILL_BYTE,
	[SPI_MSG_TAIL]		= SPI_BCM_6358_SPI_MSG_TAIL,
	[SPI_RX_TAIL]		= SPI_BCM_6358_SPI_RX_TAIL,
	[SPI_MSG_CTL]		= SPI_BCM_6358_MSG_CTL,
	[SPI_MSG_DATA]		= SPI_BCM_6358_SPI_MSG_DATA,
	[SPI_RX_DATA]		= SPI_BCM_6358_SPI_RX_DATA,
};

u16 __bcm63xx_get_cpu_id(void)
{
	return bcm63xx_cpu_id;
}

EXPORT_SYMBOL(__bcm63xx_get_cpu_id);

u16 bcm63xx_get_cpu_rev(void)
{
	return bcm63xx_cpu_rev;
}

EXPORT_SYMBOL(bcm63xx_get_cpu_rev);

unsigned int bcm63xx_get_cpu_freq(void)
{
	return bcm63xx_cpu_freq;
}

unsigned int bcm63xx_get_memory_size(void)
{
	return bcm63xx_memory_size;
}

static unsigned int detect_cpu_clock(void)
{
	unsigned int tmp, n1 = 0, n2 = 0, m1 = 0;

	if (BCMCPU_IS_6338())
		return 240000000;

	if (BCMCPU_IS_6345())
		return 140000000;

	/*
	 * frequency depends on PLL configuration:
	 */
	if (BCMCPU_IS_6348()) {
		/* 16MHz * (N1 + 1) * (N2 + 2) / (M1_CPU + 1) */
		tmp = bcm_perf_readl(PERF_MIPSPLLCTL_REG);
		n1 = (tmp & MIPSPLLCTL_N1_MASK) >> MIPSPLLCTL_N1_SHIFT;
		n2 = (tmp & MIPSPLLCTL_N2_MASK) >> MIPSPLLCTL_N2_SHIFT;
		m1 = (tmp & MIPSPLLCTL_M1CPU_MASK) >> MIPSPLLCTL_M1CPU_SHIFT;
		n1 += 1;
		n2 += 2;
		m1 += 1;
	}

	if (BCMCPU_IS_6358()) {
		/* 16MHz * N1 * N2 / M1_CPU */
		tmp = bcm_ddr_readl(DDR_DMIPSPLLCFG_REG);
		n1 = (tmp & DMIPSPLLCFG_N1_MASK) >> DMIPSPLLCFG_N1_SHIFT;
		n2 = (tmp & DMIPSPLLCFG_N2_MASK) >> DMIPSPLLCFG_N2_SHIFT;
		m1 = (tmp & DMIPSPLLCFG_M1_MASK) >> DMIPSPLLCFG_M1_SHIFT;
	}

	return (16 * 1000000 * n1 * n2) / m1;
}

/*
 * attempt to detect the amount of memory installed
 */
static unsigned int detect_memory_size(void)
{
	unsigned int cols = 0, rows = 0, is_32bits = 0, banks = 0;
	u32 val;

	if (BCMCPU_IS_6345())
		return (8 * 1024 * 1024);

	if (BCMCPU_IS_6338() || BCMCPU_IS_6348()) {
		val = bcm_sdram_readl(SDRAM_CFG_REG);
		rows = (val & SDRAM_CFG_ROW_MASK) >> SDRAM_CFG_ROW_SHIFT;
		cols = (val & SDRAM_CFG_COL_MASK) >> SDRAM_CFG_COL_SHIFT;
		is_32bits = (val & SDRAM_CFG_32B_MASK) ? 1 : 0;
		banks = (val & SDRAM_CFG_BANK_MASK) ? 2 : 1;
	}

	if (BCMCPU_IS_6358()) {
		val = bcm_memc_readl(MEMC_CFG_REG);
		rows = (val & MEMC_CFG_ROW_MASK) >> MEMC_CFG_ROW_SHIFT;
		cols = (val & MEMC_CFG_COL_MASK) >> MEMC_CFG_COL_SHIFT;
		is_32bits = (val & MEMC_CFG_32B_MASK) ? 0 : 1;
		banks = 2;
	}

	/* 0 => 11 address bits ... 2 => 13 address bits */
	rows += 11;

	/* 0 => 8 address bits ... 2 => 10 address bits */
	cols += 8;

	return 1 << (cols + rows + (is_32bits + 1) + banks);
}

void __init bcm63xx_cpu_init(void)
{
	unsigned int tmp, expected_cpu_id;
	struct cpuinfo_mips *c = &current_cpu_data;

	/* soc registers location depends on cpu type */
	expected_cpu_id = 0;

	switch (c->cputype) {
	case CPU_BCM3302:
		expected_cpu_id = BCM6338_CPU_ID;
		bcm63xx_regs_base = bcm96338_regs_base;
		bcm63xx_irqs = bcm96338_irqs;
		bcm63xx_regs_spi = bcm96338_regs_spi;
		break;
	case CPU_BCM6345:
		expected_cpu_id = BCM6345_CPU_ID;
		bcm63xx_regs_base = bcm96345_regs_base;
		bcm63xx_irqs = bcm96345_irqs;
		break;
	case CPU_BCM6348:
		expected_cpu_id = BCM6348_CPU_ID;
		bcm63xx_regs_base = bcm96348_regs_base;
		bcm63xx_irqs = bcm96348_irqs;
		bcm63xx_regs_spi = bcm96348_regs_spi;
		break;
	case CPU_BCM6358:
		expected_cpu_id = BCM6358_CPU_ID;
		bcm63xx_regs_base = bcm96358_regs_base;
		bcm63xx_irqs = bcm96358_irqs;
		bcm63xx_regs_spi = bcm96358_regs_spi;
		break;
	}

	/* really early to panic, but delaying panic would not help
	 * since we will never get any working console */
	if (!expected_cpu_id)
		panic("unsupported Broadcom CPU");

	/*
	 * bcm63xx_regs_base is set, we can access soc registers
	 */

	/* double check CPU type */
	tmp = bcm_perf_readl(PERF_REV_REG);
	bcm63xx_cpu_id = (tmp & REV_CHIPID_MASK) >> REV_CHIPID_SHIFT;
	bcm63xx_cpu_rev = (tmp & REV_REVID_MASK) >> REV_REVID_SHIFT;

	if (bcm63xx_cpu_id != expected_cpu_id)
		panic("bcm63xx CPU id mismatch");

	bcm63xx_cpu_freq = detect_cpu_clock();
	bcm63xx_memory_size = detect_memory_size();

	printk(KERN_INFO "Detected Broadcom 0x%04x CPU revision %02x\n",
	       bcm63xx_cpu_id, bcm63xx_cpu_rev);
	printk(KERN_INFO "CPU frequency is %u Hz\n",
	       bcm63xx_cpu_freq);
	printk(KERN_INFO "%uMB of RAM installed\n",
	       bcm63xx_memory_size >> 20);
}
