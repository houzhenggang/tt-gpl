#define SUPPORT_DUBLIN
inline void IO_InitDublin(void)
{
	VALUE_SET(id                 , GOTYPE_DUBLIN);
	VALUE_SET(caseid             , GOCASE_MILAN); /* for now */
	VALUE_SET(cputype            , GOCPU_S3C2443);

	VALUE_SET(btchip             , GOBT_BC4);
	VALUE_SET(btspeed            , 921600);
	VALUE_SET(btclock            , 16000000);
	VALUE_SET(btclass            , 0x280408);
	VALUE_SET(handsfree          , 1);
	VALUE_SET(headsetgw          , 1);
	VALUE_SET(a2dp               , 1);

	VALUE_SET(batterytype	     , GOBATTERY_UNDEFINED);
	VALUE_SET(chargertype        , GOCHARGER_LTC3455);
	VALUE_SET(chargerresistor    , 1780);
	VALUE_SET(sdcard             , 1);
	VALUE_SET(sdslot             , 1);
	VALUE_SET(internalflash      , 1);
	VALUE_SET(hsmmcinterface     , 1);
	VALUE_SET(tfttype            , GOTFT_SAMSUNG_LTE430WQ);
	VALUE_SET(backlighttype      , GOBACKLIGHT_CH0_1839_CAT3238TD_430);

	VALUE_SET(gpstype            , GOGPS_SIRF3);
	VALUE_SET(codectype          , GOCODEC_WM8971);
	VALUE_SET(codecmaster        , GOCODECCFG_SLAVE);
	VALUE_SET(usbslavetype       , GOUSB_S3C2443);
	VALUE_SET(ohciports          , 0x02);
	VALUE_SET(usbdevicehostcapable,1);
	VALUE_SET(regulatorforcodec  , 1);
	VALUE_SET(keeprtcpowered     , 1);
	VALUE_SET(canrecordaudio     , 1);
	VALUE_SET(hw_i2c             , 1);
	VALUE_SET(pnp                , 1);
	VALUE_SET(codecamptype       , GOCODECAMP_DIFFERENTIAL_HW);
	VALUE_SET(aectype            , GOAEC_FM);
	VALUE_SET(loquendo           , 1);
        VALUE_SET(gpsephemeris       , 1);

	VALUE_SET(tsxchannel         , 5);
	VALUE_SET(tsychannel         , 9);
	VALUE_SET(tsdownchannel      , 9);

	VALUE_SET(videodecoder       , GOVD_ADV7180);

	STRING_SET(familyname,"TomTom GO");
	STRING_SET(projectname,"DUBLIN");
	STRING_SET(requiredbootloader,"5.4104");
	STRING_SET(dockdev,"ttySAC0");
	STRING_SET(btdev,"ttySAC1");
	STRING_SET(gpsdev,"ttySAC2");
	STRING_SET(hudev,"ttySAC3");
	STRING_SET(name,"TomTom GO Dublin");
	STRING_SET(btname,"TomTom GO Dublin");
	STRING_SET(usbname,"GO Dublin");	

	PIN_SET(SD_PWR_ON          , PIN_GPJ13);
	PIN_SET(HS_MOVI_PWR_ON     , PIN_GPJ14 | PIN_INVERTED);
	PIN_SET(WP_SD              , PIN_GPF7);
	PIN_SET(CD_SD              , PIN_GPF1 | PIN_INVERTED);

	PIN_SET(GPS_RESET          , PIN_GPE13 | PIN_INVERTED);
	PIN_SET(GPS_ON             , PIN_GPE12);
	PIN_SET(GPS_1PPS           , PIN_GPG0);
	PIN_SET(GPS_REPRO          , PIN_GPE11);
	PIN_SET(TXD_GPS            , PIN_GPH4);
	PIN_SET(RXD_GPS            , PIN_GPH5);

	PIN_SET(ON_OFF             , PIN_GPF0 | PIN_INVERTED);
	PIN_SET(ACPWR              , PIN_GPF2 | PIN_INVERTED);
	PIN_SET(PWR_RST            , PIN_GPG10);
	PIN_SET(CHARGING           , PIN_GPG1 | PIN_INVERTED);
	PIN_SET(AIN4_PWR           , PIN_GPA11);
	PIN_SET(BATT_TEMP_OVER     , PIN_GPF4 | PIN_INVERTED);
	PIN_SET(USB_HP             , PIN_GPH12);
	PIN_SET(USB_SUSPEND_OUT    , PIN_GPH8);
	PIN_SET(USB_PWR_BYPASS     , PIN_GPG5);
	PIN_SET(PWR_MODE           , PIN_GPG6);	
	PIN_SET(LOW_DC_VCC         , PIN_GPF6 | PIN_INVERTED);

	PIN_SET(LINEIN_DETECT      , PIN_GPF3 | PIN_INVERTED);

	PIN_SET(TXD_DOCK           , PIN_GPH0);
	PIN_SET(RXD_DOCK           , PIN_GPH1);

	PIN_SET(AMP_ON             , PIN_GPG11);
	PIN_SET(DAC_PWR_ON         , PIN_GPJ15);
	PIN_SET(NAVI_MUTE          , PIN_GPL11);
	PIN_SET(TEL_MUTE           , PIN_GPL10);
	PIN_SET(HEADPHONE_DETECT   , PIN_GPG4 | PIN_INVERTED);
	PIN_SET(I2SSDO             , PIN_GPE4);
	PIN_SET(CDCLK              , PIN_GPE2);
	PIN_SET(CDCLK_12MHZ        , PIN_GPH13);
	PIN_SET(I2SSCLK            , PIN_GPE1);
	PIN_SET(I2SLRCK            , PIN_GPE0);
	PIN_SET(I2SSDI             , PIN_GPE3);
	PIN_SET(L3CLOCK            , PIN_GPB4);
	PIN_SET(L3MODE             , PIN_GPB3);
	PIN_SET(L3DATA             , PIN_GPB2);

	PIN_SET(TXD_BT             , PIN_GPH2);
	PIN_SET(RXD_BT             , PIN_GPH3);
	PIN_SET(RTS_BT             , PIN_GPH11);
	PIN_SET(CTS_BT             , PIN_GPH10);
	PIN_SET(BT_RESET           , PIN_GPG9 | PIN_INVERTED);

	PIN_SET(LCD_VCC_PWREN      , PIN_GPC6 | PIN_INVERTED);
	PIN_SET(BACKLIGHT_PWM      , PIN_GPB0 | PIN_INVERTED);
	PIN_SET(BACKLIGHT_EN       , PIN_GPB1);
	PIN_SET(LCD_RESET          , PIN_GPC0 | PIN_INVERTED);
	PIN_SET(LCD_ID             , PIN_GPC7);

	PIN_SET(USB_HOST_DETECT    , PIN_GPF2 | PIN_INVERTED);
	PIN_SET(IGNITION           , PIN_GPF2 | PIN_INVERTED);
	PIN_SET(USB_PHY_PWR_EN     , PIN_GPG8);

	PIN_SET(HW_IIC_SDA         , PIN_GPE15);
	PIN_SET(HW_IIC_SCL         , PIN_GPE14);

	PIN_SET(XMON               , PIN_GPG13 | PIN_OPEN_COLLECTOR);
	PIN_SET(XPON               , PIN_GPG12 | PIN_OPEN_EMITTER);
	PIN_SET(YMON               , PIN_GPG15 | PIN_OPEN_COLLECTOR);
	PIN_SET(YPON               , PIN_GPG14 | PIN_OPEN_EMITTER);
	PIN_SET(TSDOWN             , PIN_GPG15 | PIN_OPEN_COLLECTOR);

	PIN_SET(LOW_CORE           , PIN_GPG7 | PIN_INVERTED);
	PIN_SET(LX_EN              , PIN_GPL13);

	PIN_SET(CAM_DPWDN          , PIN_GPC5 | PIN_INVERTED);
	PIN_SET(CAM_DIRQ           , PIN_GPG3 | PIN_INVERTED);
	PIN_SET(CAMRESET           , PIN_GPJ12);
	PIN_SET(CAMCLKOUT          , PIN_GPJ11);
	PIN_SET(CAMPCLK            , PIN_GPJ8);
	PIN_SET(CAMVSYNC           , PIN_GPJ9);
	PIN_SET(CAMHREF            , PIN_GPJ10);
	PIN_SET(CAMDATA0           , PIN_GPJ0);
	PIN_SET(CAMDATA1           , PIN_GPJ1);
	PIN_SET(CAMDATA2           , PIN_GPJ2);
	PIN_SET(CAMDATA3           , PIN_GPJ3);
	PIN_SET(CAMDATA4           , PIN_GPJ4);
	PIN_SET(CAMDATA5           , PIN_GPJ5);
	PIN_SET(CAMDATA6           , PIN_GPJ6);
	PIN_SET(CAMDATA7           , PIN_GPJ7);

	PIN_SET(EN_DR_PWR          , PIN_GPL13); /* for photo sensor power */

	PIN_SET(UART_INTA          , PIN_GPG2 | PIN_INVERTED);
	PIN_SET(UART_RESET         , PIN_GPH9 | PIN_INVERTED);

	PIN_SET(DOCK_RADIO_SENSE   , PIN_GPF3 | PIN_INVERTED);
	PIN_SET(HU_PWR_ON          , PIN_GPF5 | PIN_INVERTED);
	PIN_SET(DOCK_PWREN         , PIN_GPL12 | PIN_INVERTED);
	PIN_SET(TXD_HU             , PIN_GPH6 );
	PIN_SET(RXD_HU             , PIN_GPH7 );

	PIN_SET(FACTORY_TEST_POINT , PIN_GPL14);
}

