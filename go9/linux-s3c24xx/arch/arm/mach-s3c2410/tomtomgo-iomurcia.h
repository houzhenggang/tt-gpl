#define SUPPORT_MURCIA
inline void IO_InitMurcia(void)
{
	VALUE_SET(caseid             , GOCASE_VALENCIA);
	VALUE_SET(cputype            , GOCPU_S3C2442);
	VALUE_SET(btchip             , GOBT_BC4);
	VALUE_SET(btspeed            , 921600);
	VALUE_SET(btclock            , 16000000);
	VALUE_SET(btclass            , 0x280408);
	VALUE_SET(handsfree          , 1);
	VALUE_SET(headsetgw          , 1);
	VALUE_SET(a2dp               , 1);
	VALUE_SET(batterytype	     , GOBATTERY_ICR18650_2200);
	VALUE_SET(chargertype        , GOCHARGER_LTC1733);
	VALUE_SET(chargerresistor    , 1300);
	VALUE_SET(sdcard             , 1);
        VALUE_SET(sdslot             , 1);
	VALUE_SET(harddisk           , 1);
	VALUE_SET(tfttype            , GOTFT_SAMSUNG_LTP400);
	VALUE_SET(backlighttype	     , GOBACKLIGHT_CH0_30K_TPS61042DRBR_400);
	VALUE_SET(tsxchannel         , 5);
	VALUE_SET(tsychannel         , 7);
	VALUE_SET(gpstype            , GOGPS_SIRF3);
	VALUE_SET(gldetected         , 1);
	VALUE_SET(codectype          , GOCODEC_WM8971);
	VALUE_SET(usbslavetype       , GOUSB_NET2272);
	VALUE_SET(usbdevicehostcapable,0);
	VALUE_SET(harddisktiming     , 4);
	VALUE_SET(ohciports          , 0x02);
	VALUE_SET(regulatorforcodec  , 1);
	VALUE_SET(codecmaster        , GOCODECCFG_EXTERNAL_MASTER);
	VALUE_SET(backlightfreq      , 30000);
	VALUE_SET(keeprtcpowered     , 1);
	VALUE_SET(canrecordaudio     , 1);
	VALUE_SET(pnp                , 1);

	STRING_SET(familyname,"TomTom GO");
	STRING_SET(projectname,"MURCIA");
	STRING_SET(requiredbootloader,"4.66");
	STRING_SET(dockdev,"ttySAC0");
	STRING_SET(gpsdev,"ttySAC1");
	STRING_SET(btdev,"ttySAC2");
	STRING_SET(gprsmodemdev,"ttyS0");

	PIN_SET(SD_PWR_ON        , PIN_GPG2 | PIN_INVERTED);
	PIN_SET(WP_SD            , PIN_GPH8);
	PIN_SET(CD_SD            , PIN_GPF2  | PIN_INVERTED);
	PIN_SET(GPS_RESET        , PIN_GPJ10 | PIN_INVERTED);
	PIN_SET(GPS_ON           , PIN_GPJ6);
	PIN_SET(HDD_PWR_ON       , PIN_GPB7 | PIN_INVERTED);
	PIN_SET(HDD_RST          , PIN_GPG3 | PIN_INVERTED);
	PIN_SET(HDD_BUF_EN       , PIN_GPA15 | PIN_INVERTED);
	PIN_SET(HDD_IRQ          , PIN_GPG11);
	PIN_SET(HDD_CS           , PIN_GPA14 | PIN_INVERTED);
	PIN_SET(CHARGEFAULT      , PIN_GPJ0 | PIN_INVERTED);
	PIN_SET(CHARGE_OUT       , PIN_GPJ7 | PIN_OPEN_EMITTER);
	PIN_SET(CHARGING         , PIN_GPJ12 | PIN_INVERTED);
	PIN_SET(AIN4_PWR         , PIN_GPA13);
	PIN_SET(DOCK_PWREN       , PIN_GPJ9 | PIN_INVERTED);
	PIN_SET(DOCK_INT         , PIN_GPF3 | PIN_INVERTED);
	PIN_SET(SW_SCL           , PIN_GPH0);
	PIN_SET(SW_SDA           , PIN_GPH1);
	PIN_SET(DOCK_PWREN       , PIN_GPJ9 | PIN_INVERTED);
	PIN_SET(FSK_IRQ          , PIN_GPG8 | PIN_INVERTED);
	PIN_SET(SPICLK           , PIN_GPE13);
	PIN_SET(SPIMSI           , PIN_GPE12);
	PIN_SET(SPIMSO           , PIN_GPE11);
	PIN_SET(FSK_EN           , PIN_GPJ2);
	PIN_SET(FSK_FFS          , PIN_GPG6);
	PIN_SET(ACC_PWR_ON       , PIN_GPJ4 | PIN_INVERTED);
	PIN_SET(MIC_SW           , PIN_GPJ8);
	PIN_SET(AMP_ON           , PIN_GPJ3);
	PIN_SET(DAC_PWR_ON       , PIN_GPG7);
	PIN_SET(I2SSDO           , PIN_GPE4);
	PIN_SET(CDCLK            , PIN_GPE2);
	PIN_SET(CDCLK_12MHZ      , PIN_GPH9);
	PIN_SET(I2SSCLK          , PIN_GPE1);
	PIN_SET(I2SLRCK          , PIN_GPE0);
	PIN_SET(I2SSDI           , PIN_GPE3);
	PIN_SET(L3CLOCK          , PIN_GPB4);
	PIN_SET(L3MODE           , PIN_GPB3);
	PIN_SET(L3DATA           , PIN_GPB2);
	PIN_SET(TXD_DOCK         , PIN_GPH2);
	PIN_SET(RXD_DOCK         , PIN_GPH3);
	PIN_SET(RXD_DOCK_INT     , PIN_GPF4);
	PIN_SET(TXD_BT           , PIN_GPH6);
	PIN_SET(RXD_BT           , PIN_GPH7);
	PIN_SET(TXD_GPS          , PIN_GPH4);
	PIN_SET(RXD_GPS          , PIN_GPH5);
	PIN_SET(RTS_GPS          , PIN_GPG9);
	PIN_SET(CTS_GPS          , PIN_GPG10);
	PIN_SET(BT_RESET         , PIN_GPJ5 | PIN_INVERTED);
	PIN_SET(BT_MODE          , PIN_GPB1);
	PIN_SET(LCD_VCC_PWREN    , PIN_GPG4 | PIN_INVERTED);
	PIN_SET(LCD_RESET        , PIN_GPC7 | PIN_INVERTED);
	PIN_SET(BACKLIGHT_PWM    , PIN_GPB0);
	PIN_SET(USB_DACK         , PIN_GPB9);
	PIN_SET(USB_DREQ         , PIN_GPB10);
	PIN_SET(USB_IRQ          , PIN_GPG5 | PIN_INVERTED);
	PIN_SET(USB_VBUS         , PIN_GPJ1);
	PIN_SET(USB_RESET        , PIN_GPA18 | PIN_INVERTED);
	PIN_SET(ON_OFF           , PIN_GPF0 | PIN_INVERTED);
	PIN_SET(ACPWR            , PIN_GPG1 | PIN_INVERTED);
	PIN_SET(IGNITION         , PIN_GPG1 | PIN_INVERTED);
	PIN_SET(XMON             , PIN_GPG13 | PIN_OPEN_COLLECTOR);
	PIN_SET(XPON             , PIN_GPG12 | PIN_OPEN_EMITTER);
	PIN_SET(YMON             , PIN_GPG15 | PIN_OPEN_COLLECTOR);
	PIN_SET(YPON             , PIN_GPG14 | PIN_OPEN_EMITTER);
	PIN_SET(TSDOWN           , PIN_GPG15 | PIN_OPEN_COLLECTOR);
	PIN_SET(LOW_CORE         , PIN_GPB8);
	PIN_SET(FLEX_ID1         , PIN_GPF6 | PIN_INVERTED);
	PIN_SET(LX_EN            , PIN_GPC5);
	PIN_SET(UART_PWRSAVE     , PIN_GPA17);
	PIN_SET(UART_INTA        , PIN_GPF5);
	PIN_SET(UART_CSA         , PIN_GPA16 | PIN_INVERTED);
	PIN_SET(UART_RESET       , PIN_GPJ4);
	PIN_SET(UART_CLK       	 , PIN_GPH10);
	PIN_SET(GSM_ON           , PIN_GPC6);
	PIN_SET(GSM_OFF          , PIN_GPG0);
	PIN_SET(GSM_SYNC         , PIN_GPF1);
	PIN_SET(GSM_WAKEUP       , PIN_GPF7);

	// Pins of IO expander in Valencia dock
	PIN_SET(DOCK_CRIB_SENSE  , PIN_GPIIC0 | PIN_INVERTED);
	PIN_SET(MUTE_EXT         , PIN_GPIIC2);
	PIN_SET(HEADPHONE_DETECT , PIN_GPIIC3 | PIN_INVERTED);
	PIN_SET(LINEIN_DETECT    , PIN_GPIIC4 | PIN_INVERTED);
	PIN_SET(EXTMIC_DETECT    , PIN_GPIIC5 | PIN_INVERTED);
	PIN_SET(LIGHTS_DETECT    , PIN_GPIIC6 | PIN_INVERTED);
	PIN_SET(TMC_POWER        , PIN_GPIIC7 | PIN_INVERTED);

	// These "pins" will be simulated in software to make life easier
	PIN_SET(USB_HOST_DETECT  , PIN_GPIIC8);
	PIN_SET(DOCK_SENSE       , PIN_GPIIC9);
	PIN_SET(DOCK_DESK_SENSE  , PIN_GPIIC10);
	PIN_SET(DOCK_VIB_SENSE   , PIN_GPIIC11);
}

inline void IO_InitMurcia500(void)
{
	VALUE_SET(id                 , GOTYPE_MURCIA500);
	VALUE_SET(sdcard             , 1);
	VALUE_SET(harddisk           , 0);

	STRING_SET(name,"TomTom GO 715");
	STRING_SET(usbname,"GO 715");
	STRING_SET(btname,"TomTom GO");
}

inline void IO_InitMurcia700(void)
{
	VALUE_SET(id                 , GOTYPE_MURCIA700);
	VALUE_SET(sdcard             , 0);
	VALUE_SET(harddisk           , 1);
	VALUE_SET(loquendo           , 1);
	VALUE_SET(mp3                , 1);

	STRING_SET(name,"TomTom GO 915");
	STRING_SET(usbname,"GO 915");
	STRING_SET(btname,"TomTom GO 915");
}
